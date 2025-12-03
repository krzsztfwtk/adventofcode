#include "../types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void info(char *programName) {
  printf("Usage: %s <part>, where <part> in {1, 2}\n", programName);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    info(argv[0]);
    return 0;
  }

  const u8 part = atoi(argv[1]);
  if (part > 2 || part < 1) {
    info(argv[0]);
    return 0;
  }

  FILE *file = fopen("day3.txt", "r");

  if (file == NULL) {
    perror("Error opening file");
    return 1;
  }

  char line[256];

  if (part == 1) {
    u32 sum = 0;
    while (fgets(line, sizeof(line), file)) {
      char firstDigit = line[0];
      char secondDigit = line[1];
      char *c = line + 2;

      while (*c && *c != '\n') {
        if (*(c - 1) > firstDigit) {
          firstDigit = *(c - 1);
          secondDigit = *c;
        } else if (*c > secondDigit) {
          secondDigit = *c;
        }
        ++c;
      }

      const u8 joltage = (firstDigit - '0') * 10 + (secondDigit - '0');
      sum += joltage;
    }
    fclose(file);
    printf("%u\n", sum);
    return 0;
  }

  u64 sum = 0;
  char digits[12];

  while (fgets(line, sizeof(line), file)) {
    strncpy(digits, line, 12);
    char *c = line + 12;

    while (*c && *c != '\n') {
      u8 updated = 0;

      for (u8 i = 0; !updated && i < 12; ++i) {

        if (*(c - 11 + i) > digits[i]) {
          digits[i] = *(c - 11 + i);
          for (u8 j = i + 1; j < 12; ++j)
            digits[j] = *(c - 11 + j);

          ++updated;
        }
      }
      ++c;
    }

    u64 joltage = digits[0] - '0';

    for (u8 i = 1; i < 12; ++i) {
      joltage *= 10;
      joltage += digits[i] - '0';
    }

    sum += joltage;
  }

  fclose(file);
  printf("%lu\n", sum);
}