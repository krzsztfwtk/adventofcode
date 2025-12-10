#include "../types.h"
#include <ankerl/unordered_dense.h>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

using Bitmask = u16;
using Cost = u32;

constexpr inline Bitmask toggleBit(const Bitmask mask, const u8 bitIdx) {
  return mask ^ u16(Bitmask(1) << bitIdx);
}

constexpr inline Bitmask pressButton(const Bitmask lights,
                                     const Bitmask button) {
  return lights ^ button;
}

void pressButton2(vector<u8> &counters, const vector<u8> &button) {
  for (const u8 i : button) {
    ++counters[i];
  }
}

struct Machine1 {
  Bitmask desiredLights;
  vector<Bitmask> buttons;
};

struct Machine2 {
  Bitmask desiredLights;
  vector<u8> desiredCounters;
  vector<vector<u8>> buttons;
};

Bitmask parseButtons1(const string &segment);
vector<u8> parseButtons2(const string &segment);
vector<u8> parseCounters(const string &segment);
Machine1 parseMachine1(const string &line);
Machine2 parseMachine2(const string &line);

u32 dijkstra1(const Machine1 m) {
  struct State {
    Bitmask lights;
    Cost cost;

    bool operator<(const State &other) const { return cost > other.cost; }
  };

  priority_queue<State> openList;
  unordered_map<Bitmask, Cost> gCost;
  gCost[0] = 0;
  openList.push({0, 0});

  while (!openList.empty()) {
    const State current = openList.top();
    openList.pop();
    const Cost currentGCost = gCost[current.lights];

    if (current.lights == m.desiredLights) {
      return currentGCost;
    }

    const Cost newGCost = currentGCost + 1;
    for (const Bitmask button : m.buttons) {
      Bitmask newLights = pressButton(current.lights, button);

      // If state was alredy visited with lower cost
      if (gCost.find(newLights) != gCost.end() && gCost[newLights] <= newGCost)
        continue;

      gCost[newLights] = newGCost;
      openList.push({newLights, newGCost});
    }
  }

  return UINT32_MAX;
}

u32 astar2(const Machine2 &m) {
  struct VectorHash {
    u64 operator()(const std::vector<uint8_t> &vec) const {
      u64 hash = 0;
      for (u64 b : vec) {
        hash ^=
            std::hash<uint8_t>()(b) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
      }
      return hash;
    }
  };

  struct State {
    vector<u8> counters;
    Cost fCost;

    bool operator<(const State &other) const { return fCost > other.fCost; }
  };

  auto heuristic = [desired = m.desiredCounters](const vector<u8> &counters) {
    u32 maxDiff = 0;

    for (size_t i = 0; i < counters.size(); ++i) {
      if (counters[i] > desired[i]) {
        return 1024u * 1024u;
      }

      u32 diff = desired[i] - counters[i];
      if (diff > maxDiff) {
        maxDiff = diff;
      }
    }

    return maxDiff;
  };

  priority_queue<State> openList;
  ankerl::unordered_dense::map<vector<u8>, Cost, VectorHash> gCost;
  vector<u8> initialCounters(m.desiredCounters.size(), {});

  gCost[initialCounters] = 0;
  openList.push({initialCounters, heuristic(initialCounters)});
  u32 count = 0;

  while (!openList.empty()) {

    const State current = openList.top();
    openList.pop();
    const Cost currentGCost = gCost[current.counters];

    ++count;
    if (count % (1024u * 1024u) == 0) {
      cout << '\n'
           << count << ' ' << currentGCost << ' '
           << current.fCost - currentGCost << '\n';
      for (u8 i : current.counters)
        cout << (u32)i << ' ';
    }

    if (current.counters == m.desiredCounters) {
      return currentGCost;
    }

    const Cost newGCost = currentGCost + 1;
    for (const auto &button : m.buttons) {
      vector<u8> newCounters = current.counters;
      pressButton2(newCounters, button);

      const auto [it, inserted] = gCost.try_emplace(newCounters, newGCost);
      if (!inserted) {
        if (it->second <= newGCost)
          continue;
        it->second = newGCost;
      }

      const Cost fCost = newGCost + heuristic(newCounters);
      openList.push({newCounters, fCost});
    }
  }

  return UINT32_MAX;
}

int main() {
  ifstream file("dayA.txt");
  if (!file)
    return 1;

  string line;
  vector<Machine1> machines1;
  vector<Machine2> machines2;

  while (getline(file, line)) {
    if (!line.empty()) {
      machines1.push_back(parseMachine1(line));
      machines2.push_back(parseMachine2(line));
    }
  }
  file.close();

  u32 sum1 = 0;
  u32 sum2 = 0;

  for (const auto m : machines1) {
    sum1 += dijkstra1(m);
  }

  cout << sum1 << endl;

  for (const auto m : machines2) {
    const u32 result = astar2(m);
    cout << result << endl;
    sum2 += result;
  }

  cout << sum2 << '\n';

  return 0;
}

Bitmask parseLights(const string &segment) {
  Bitmask mask = 0;

  size_t start = segment.find('[');
  size_t end = segment.find(']');

  for (size_t i = start + 1; i < end; ++i) {
    if (segment[i] == '#') {
      mask = toggleBit(mask, i - (start + 1));
    }
  }
  return mask;
}

Bitmask parseButtons1(const string &segment) {
  Bitmask mask = 0;

  size_t start = segment.find('(');
  size_t end = segment.find(')');

  string numbersStr = segment.substr(start + 1, end - (start + 1));
  stringstream ss(numbersStr);
  string token;

  while (getline(ss, token, ',')) {
    if (!token.empty()) {
      u8 bitIdx = stoi(token);
      mask = toggleBit(mask, bitIdx);
    }
  }

  return mask;
}

vector<u8> parseButtons2(const string &segment) {
  vector<u8> indices;

  size_t start = segment.find('(');
  size_t end = segment.find(')');

  string numbersStr = segment.substr(start + 1, end - (start + 1));
  stringstream ss(numbersStr);
  string token;

  while (getline(ss, token, ',')) {
    if (!token.empty()) {
      u8 bitIdx = stoi(token);
      indices.push_back(bitIdx);
    }
  }

  return indices;
}

vector<u8> parseCounters(const string &segment) {
  vector<u8> counters;

  size_t start = segment.find('{');
  size_t end = segment.find('}');

  string numbersStr = segment.substr(start + 1, end - (start + 1));
  stringstream ss(numbersStr);
  string token;

  while (getline(ss, token, ',')) {
    if (!token.empty()) {
      u8 counter = stoi(token);
      counters.push_back(counter);
    }
  }
  return counters;
}

Machine1 parseMachine1(const string &line) {
  Machine1 result = {};
  size_t pos = 0;

  size_t bracket_start = line.find('[');
  size_t bracket_end = line.find(']');
  result.desiredLights =
      parseLights(line.substr(bracket_start, bracket_end - bracket_start + 1));
  pos = bracket_end + 1;

  while (true) {
    size_t paren_start = line.find('(', pos);
    if (paren_start == string::npos || line.find('{', pos) < paren_start)
      break;

    size_t paren_end = line.find(')', paren_start);
    if (paren_end == string::npos)
      break;

    result.buttons.push_back(
        parseButtons1(line.substr(paren_start, paren_end - paren_start + 1)));
    pos = paren_end + 1;
  }

  return result;
}

Machine2 parseMachine2(const string &line) {
  Machine2 result = {};
  size_t pos = 0;

  size_t bracket_start = line.find('[');
  size_t bracket_end = line.find(']');
  result.desiredLights =
      parseLights(line.substr(bracket_start, bracket_end - bracket_start + 1));
  pos = bracket_end + 1;

  while (true) {
    size_t paren_start = line.find('(', pos);
    if (paren_start == string::npos || line.find('{', pos) < paren_start)
      break;

    size_t paren_end = line.find(')', paren_start);
    if (paren_end == string::npos)
      break;

    result.buttons.push_back(
        parseButtons2(line.substr(paren_start, paren_end - paren_start + 1)));
    pos = paren_end + 1;
  }

  size_t brace_start = line.find('{', pos);
  if (brace_start != string::npos) {
    size_t brace_end = line.find('}', brace_start);
    if (brace_end != string::npos) {
      result.desiredCounters =
          parseCounters(line.substr(brace_start, brace_end - brace_start + 1));
    }
  }

  return result;
}