#include "../types.h"
#include <stdio.h>

#define SIZE 135

int main() {
  u8 locations[SIZE * SIZE];

  FILE *file = fopen("day4.txt", "r");
  if (file == NULL) {
    perror("Error opening file");
    return 1;
  }

  char line[256];
  u32 sum1 = 0;
  u32 i = 0;

  while (fgets(line, sizeof(line), file)) {
    char *c = line;
    while (*c) {
      if (*c == '.')
        locations[i] = 0;
      else if (*c == '@')
        locations[i] = 1;
      else
        break;

      ++i;
      ++c;
    }
  }

  fclose(file);

  for (u32 y = 0; y < SIZE; ++y) {
    for (u32 x = 0; x < SIZE; ++x) {
      u32 idx = y * SIZE + x;
      if (locations[idx] == 0)
        continue;

      u8 neighborRollsCount = 0;

      for (i32 dy = -1; dy <= 1; ++dy) {
        const u32 newY = y + dy;
        const u32 rowIdx = newY * SIZE;

        for (i32 dx = -1; dx <= 1; ++dx) {
          const u32 newX = x + dx;

          if (newX < SIZE && newY < SIZE) {
            const u32 newIdx = rowIdx + newX;
            neighborRollsCount += locations[newIdx];
          }
        }
      }

      // +1 becuase it adds itself
      if (neighborRollsCount < 4 + 1)
        ++sum1;
    }
  }

  printf("%u\n", sum1);

  u32 sum2 = 0;
  u32 removedThisIterationCount = 1;

  while (removedThisIterationCount) {
    removedThisIterationCount = 0;
    for (u32 y = 0; y < SIZE; ++y) {
      for (u32 x = 0; x < SIZE; ++x) {
        u32 idx = y * SIZE + x;
        if (locations[idx] == 0)
          continue;

        u8 neighborRollsCount = 0;

        for (i32 dy = -1; dy <= 1; ++dy) {
          for (i32 dx = -1; dx <= 1; ++dx) {
            const u32 newX = x + dx;
            const u32 newY = y + dy;
            const u32 rowIdx = newY * SIZE;

            if (newX < SIZE && newY < SIZE) {
              const u32 newIdx = rowIdx + newX;
              neighborRollsCount += locations[newIdx];
            }
          }
        }

        // +1 becuase it adds itself
        if (neighborRollsCount < 4 + 1) {
          locations[idx] = 0;
          ++removedThisIterationCount;
          ++sum2;
        }
      }
    }
  }

  printf("%u\n", sum2);

  return 0;
}