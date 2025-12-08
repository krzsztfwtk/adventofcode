#include "../types.h"
#include <stdio.h>
#include <stdlib.h>

#define BOXES_NUM 1000

#define CONNECTIONS_NUM 1000

#define ALL_DISTANCES_NUM ((BOXES_NUM * (BOXES_NUM - 1)) / 2)

struct Box {
  i32 x;
  i32 y;
  i32 z;
};

struct Distance {
  u32 box1Idx;
  u32 box2Idx;
  i64 value;
};

struct Distance connectionsHeap[CONNECTIONS_NUM];

u32 heapSize = 0;

static inline void swap(u32 i, u32 j) {
  struct Distance temp = connectionsHeap[i];
  connectionsHeap[i] = connectionsHeap[j];
  connectionsHeap[j] = temp;
}

void siftUp(u32 idx) {
  while (idx > 0) {
    u32 parentIdx = (idx - 1) >> 1;
    if (connectionsHeap[parentIdx].value > connectionsHeap[idx].value)
      break;
    swap(parentIdx, idx);
    idx = parentIdx;
  }
}

void siftDown(u32 idx) {
  while (1) {
    u32 leftChild = (idx << 1) + 1;
    u32 rightChild = leftChild + 1;
    u32 largest = idx;

    if (leftChild < heapSize &&
        connectionsHeap[largest].value < connectionsHeap[leftChild].value)
      largest = leftChild;
    if (rightChild < heapSize &&
        connectionsHeap[largest].value < connectionsHeap[rightChild].value)
      largest = rightChild;

    if (largest == idx)
      break;

    swap(idx, largest);
    idx = largest;
  }
}

void heapAdd(struct Distance d) {
  if (heapSize < CONNECTIONS_NUM) {
    connectionsHeap[heapSize] = d;
    siftUp(heapSize);
    heapSize++;
    return;
  }

  if (d.value >= connectionsHeap[0].value)
    return;

  connectionsHeap[0] = d;
  siftDown(0);
}

static inline i64 sqeuclid(const struct Box b1, const struct Box b2) {
  const i64 dx = (b1.x - b2.x);
  const i64 dy = (b1.y - b2.y);
  const i64 dz = (b1.z - b2.z);
  return dx * dx + dy * dy + dz * dz;
}

int main() {
  FILE *file = fopen("day8.txt", "r");
  if (!file)
    return 1;

  char line[64];
  struct Box *boxes = malloc(BOXES_NUM * sizeof(struct Box));

  for (u32 i = 0; i < BOXES_NUM; ++i) {
    fgets(line, sizeof(line), file);
    char *end;

    boxes[i] = (struct Box){strtol(line, &end, 10), strtol(end + 1, &end, 10),
                            strtol(end + 1, NULL, 10)};
  }
  fclose(file);

  // for (u32 i = 0; i < BOXES_NUM; ++i) {
  //   const struct Box box = boxes[i];
  //   printf("X=%u, Y=%u, Z=%u\n", box.x, box.y, box.z);
  // }

  for (u32 box1Idx = 0; box1Idx < BOXES_NUM; ++box1Idx) {
    for (u32 box2Idx = box1Idx + 1; box2Idx < BOXES_NUM; ++box2Idx) {
      if (box1Idx != box2Idx) {
        heapAdd((struct Distance){box1Idx, box2Idx,
                                  sqeuclid(boxes[box1Idx], boxes[box2Idx])});
      }
    }
  }
  free(boxes);

  // printf("**** Connections Heap  *****\n");
  // for (u32 i = 0; i < CONNECTIONS_NUM; ++i) {
  //   const struct Distance con = connectionsHeap[i];
  //   printf("id1:%u id2:%u distance:%lu\n", con.box1Idx, con.box2Idx, con.value);
  // }

  // highly optimized for time, but not for space...
  // one connection may have BOXES_NUM max boxes
  const u32 maxCircuitsNum = CONNECTIONS_NUM;
  u32 *circuits = calloc(BOXES_NUM * maxCircuitsNum, sizeof(u32));
  u32 *circuitsSizes = calloc(maxCircuitsNum, sizeof(u32));
  u32 circuitsNum = 0;

  // It is not exactly cleanest solution...
  for (u32 connectionIdx = 0; connectionIdx < CONNECTIONS_NUM;
       ++connectionIdx) {
    const u32 box1Id = connectionsHeap[connectionIdx].box1Idx;
    const u32 box2Id = connectionsHeap[connectionIdx].box2Idx;

    // Find which circuits contain box1 and box2
    u32 circuit1Idx = -1;
    u32 circuit2Idx = -1;

    for (u32 circuitIdx = 0; circuitIdx < circuitsNum; ++circuitIdx) {
      u32 *const circuit = circuits + circuitIdx * BOXES_NUM;
      if (circuit[box1Id])
        circuit1Idx = circuitIdx;
      if (circuit[box2Id])
        circuit2Idx = circuitIdx;
    }

    if (circuit1Idx != -1 && circuit2Idx != -1) {
      // Both boxes already in circuits
      if (circuit1Idx != circuit2Idx) {
        // MERGE: Move all boxes from circuit2 into circuit1
        // Make sure we always merge the higher index into the lower
        if (circuit2Idx < circuit1Idx) {
          u32 temp = circuit1Idx;
          circuit1Idx = circuit2Idx;
          circuit2Idx = temp;
        }
        u32 *const c1 = circuits + circuit1Idx * BOXES_NUM;
        u32 *const c2 = circuits + circuit2Idx * BOXES_NUM;

        for (u32 boxIdx = 0; boxIdx < BOXES_NUM; ++boxIdx) {
          if (c2[boxIdx]) {
            c1[boxIdx] = 1;
          }
        }

        circuitsSizes[circuit1Idx] += circuitsSizes[circuit2Idx];

        // Remove circuit2 by moving last circuit into its place
        if (circuit2Idx != circuitsNum - 1) {
          u32 *const cLast = circuits + (circuitsNum - 1) * BOXES_NUM;
          for (u32 boxIdx = 0; boxIdx < BOXES_NUM; ++boxIdx) {
            c2[boxIdx] = cLast[boxIdx];
            cLast[boxIdx] = 0;
          }
          circuitsSizes[circuit2Idx] = circuitsSizes[circuitsNum - 1];
        } else {
          // circuit2 is the last one, just clear it
          for (u32 boxIdx = 0; boxIdx < BOXES_NUM; ++boxIdx) {
            c2[boxIdx] = 0;
          }
        }
        circuitsSizes[circuitsNum - 1] = 0;
        --circuitsNum;
      }
      // else: already in same circuit, nothing to do
    } else if (circuit1Idx != -1) {
      // Only box1 in a circuit, add box2
      u32 *const circuit = circuits + circuit1Idx * BOXES_NUM;
      circuit[box2Id] = 1;
      ++circuitsSizes[circuit1Idx];
    } else if (circuit2Idx != -1) {
      // Only box2 in a circuit, add box1
      u32 *const circuit = circuits + circuit2Idx * BOXES_NUM;
      circuit[box1Id] = 1;
      ++circuitsSizes[circuit2Idx];
    } else {
      // Neither box in any circuit, create new one
      u32 *const circuit = circuits + circuitsNum * BOXES_NUM;
      circuit[box1Id] = 1;
      circuit[box2Id] = 1;
      circuitsSizes[circuitsNum] = 2;
      ++circuitsNum;
    }
  }

  // printf("**** circuitsSizes *****\n");
  // for (u32 i = 0; i < circuitsNum; ++i) {
  //   printf("%u:%u\n", i, circuitsSizes[i]);
  // }

  free(circuits);
  u32 m1 = 0, m2 = 0, m3 = 0;

  for (u32 i = 0; i < circuitsNum; ++i) {
    const u32 v = circuitsSizes[i];

    if (v > m1) {
      m3 = m2;
      m2 = m1;
      m1 = v;
    } else if (v > m2) {
      m3 = m2;
      m2 = v;
    } else if (v > m3) {
      m3 = v;
    }
  }

  free(circuitsSizes);

  u32 product = m1 * m2 * m3;
  // printf("%u\n", m1);
  // printf("%u\n", m2);
  // printf("%u\n", m3);
  printf("%u\n", product);

  return 0;
}