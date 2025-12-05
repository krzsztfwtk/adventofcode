#include "../types.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Range {
  u64 start;
  u64 end;
};

int main() {
  FILE *file = fopen("day5.txt", "r");
  if (file == NULL)
    return 1;

  u8 i = 0;
  struct Range *ranges = malloc(256 * sizeof(struct Range));
  char line[64];

  while (fgets(line, sizeof(line), file) && *line != '\n') {
    char *end;
    ranges[i] =
        (struct Range){strtoul(line, &end, 10), strtoul(end + 1, NULL, 10)};
    ++i;
  }

  const u8 rangesNum = i;
  u32 sum1 = 0;

  while (fgets(line, sizeof(line), file)) {
    u64 id = strtoul(line, NULL, 10);

    for (i = 0; i < rangesNum; ++i) {
      const struct Range range = ranges[i];

      if (id >= range.start && id <= range.end) {
        ++sum1;
        break;
      }
    }
  }
  fclose(file);

  u64 sum2 = 0;

  for (i = 0; i < rangesNum; ++i) {
    const struct Range first = ranges[i];

    for (u8 j = i + 1; ranges[i].start <= ranges[i].end && j < rangesNum; ++j) {
      const struct Range second = ranges[j];

      //     --------
      // --------
      if (second.start <= first.start && second.end >= first.start &&
          second.end <= first.end) {
        ranges[j].end = first.start - 1;
      }

      // --------
      //     --------
      else if (second.start <= first.end && second.start >= first.start &&
               second.end >= first.end) {
        ranges[j].start = first.end + 1;
      }

      //   --------
      // ------------
      else if (second.start <= first.start && second.end >= first.end) {
        ranges[i].start = second.end + 1;
        ranges[i].end = second.start - 1;
      }

      // --------
      //   ----
      else if (second.start >= first.start && second.end <= first.end) {
        ranges[j].start = first.end + 1;
        ranges[j].end = first.start - 1;
      }
    }

    if (ranges[i].start <= ranges[i].end)
      sum2 += ranges[i].end - ranges[i].start +
              1; // +1 because ranges are inclusive
  }

  free(ranges);
  printf("%u\n", sum1);
  printf("%lu\n", sum2);

  return 0;
}