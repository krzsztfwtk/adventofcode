#include "../types.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unordered_map>
#include <vector>

using namespace std;
using DeviceCode = u32;

unordered_map<DeviceCode, vector<DeviceCode>> devices;
unordered_map<DeviceCode, u32> pathsCount1;
unordered_map<DeviceCode, u64> pathsCount2;

constexpr u32 alphabetLen = 'z' - 'a' + 1;

constexpr inline DeviceCode encode(const char *s) {
  return (s[0] - 'a') + (s[1] - 'a') * alphabetLen +
         (s[2] - 'a') * alphabetLen * alphabetLen;
}

constexpr DeviceCode destination = encode("out");

constexpr DeviceCode dac = encode("dac");
constexpr DeviceCode fft = encode("fft");

u32 countPaths1(const DeviceCode from) {
  const auto it = pathsCount1.find(from);
  if (it != pathsCount1.end())
    return it->second;

  const vector<DeviceCode> connections = devices[from];

  u32 sum = 0;

  for (const DeviceCode neighbor : connections) {
    if (neighbor == destination) [[unlikely]]
      sum += 1;
    else
      sum += countPaths1(neighbor);
  }

  pathsCount1[from] = sum;
  return sum;
}

constexpr u32 alphabetLenMag3 = alphabetLen * alphabetLen * alphabetLen;

inline u32 getKey(DeviceCode from, bool dacVisited, bool fftVisited) {
  return from + (dacVisited ? alphabetLenMag3 : 0) +
         (fftVisited ? alphabetLenMag3 * 2 : 0);
}

u64 countPaths2(const DeviceCode from, bool dacVisited, bool fftVisited) {
  const DeviceCode key = getKey(from, dacVisited, fftVisited);

  const auto it = pathsCount2.find(key);
  if (it != pathsCount2.end())
    return it->second;

  const vector<DeviceCode> &connections = devices[from];

  u64 sum = 0;

  for (const DeviceCode neighbor : connections) {
    if (neighbor == destination) [[unlikely]] {
      if (dacVisited && fftVisited)
        ++sum;
    } else {
      sum += countPaths2(neighbor, dacVisited || neighbor == dac,
                         fftVisited || neighbor == fft);
    }
  }

  pathsCount2[key] = sum;
  return sum;
}

int main() {
  FILE *file = fopen("dayB.txt", "r");

  char line[256];

  while (fgets(line, sizeof(line), file)) {
    DeviceCode device = encode(line);

    vector<DeviceCode> connections;

    char *p = line + 5;
    while (*p) {
      connections.push_back(encode(p));
      p += 4;
    }

    devices[device] = connections;
  }
  fclose(file);

  constexpr DeviceCode you = encode("you");
  constexpr DeviceCode svr = encode("svr");

  u32 numberOfPaths1 = countPaths1(you);
  printf("%u\n", numberOfPaths1);
  u64 numberOfPaths2 = countPaths2(svr, false, false);
  printf("%lu\n", numberOfPaths2);

  return 0;
}