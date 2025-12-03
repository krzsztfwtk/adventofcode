#include "../types.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void info(char *programName) {
  printf("Usage: %s <part>, where <part> in {1, 2}\n", programName);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    info(argv[0]);
    return 0;
  }

  const u8 mode = atoi(argv[1]);
  if (mode > 2 || mode < 1) {
    info(argv[0]);
    return 0;
  }

  FILE *file = fopen("day1.txt", "r");

  if (file == NULL) {
    perror("Error opening file");
    return 1;
  }

  u32 count = 0;
  i32 pos = 50;
  char line[256];

  if (mode == 1) {
    while (fgets(line, sizeof(line), file)) {
      const char *num_str = &line[1];
      i32 rotation = atoi(num_str);

      if (line[0] == 'R') {
        pos += rotation;
        if (pos >= 100)
          pos = pos % 100;
      } else {
        pos -= rotation;
        if (pos < 0)
          pos = 100 + pos % 100;
        if (pos == 100)
          pos = 0;
      }

      if (pos == 0)
        ++count;
    }
  } else if (mode == 2) {
    while (fgets(line, sizeof(line), file)) {
      const char *num_str = &line[1];
      const i32 clicks = atoi(num_str);
      const i32 previousPos = pos;

      if (line[0] == 'R') {
        count += (pos + clicks) / 100;
        pos = (pos + clicks) % 100;
        continue;
      }

      pos = ((pos - clicks) % 100 + 100) % 100;
      if (previousPos == 0) {
        count += clicks / 100;
      } else if (clicks > previousPos) {
        count += ((clicks - previousPos - 1) / 100) + 1;
        if (pos == 0) {
          ++count;
        }
      } else if (clicks == previousPos) {
        ++count;
      }
    }
  }

  printf("%u\n", count);
  fclose(file);

  return 0;
}