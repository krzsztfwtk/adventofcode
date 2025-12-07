#include "../types.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 143

// Usefull for debugging
void prettyLines(const char *const lines) {
  for (u32 i = 0; i < SIZE; ++i) {
    const u32 idx = i * SIZE;
    printf("%s", lines + idx);
  }
}

static const char DIGITS[] =
    "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@";

void prettyNumbers64(const u32 *numbers) {
  const u32 MAX_LEN = 6;

  for (u32 i = 0; i < SIZE; ++i) {
    for (u32 j = 0; j < SIZE; ++j) {
      u32 v = numbers[i * SIZE + j];

      char buf[16];
      u32 p = 0;

      do {
        buf[p++] = DIGITS[v % 64];
        v /= 64;
      } while (v);

      for (u32 a = 0, b = p - 1; a < b; ++a, --b) {
        char t = buf[a];
        buf[a] = buf[b];
        buf[b] = t;
      }
      buf[p] = 0;

      u32 len = p;
      u32 pad = (MAX_LEN - len);
      u32 left = pad / 2;
      u32 right = pad - left;

      for (u32 k = 0; k < left; ++k)
        putchar(' ');
      fputs(buf, stdout);
      for (u32 k = 0; k < right; ++k)
        putchar(' ');
    }
    putchar('\n');
  }
}

int main() {
  FILE *file = fopen("day7.txt", "r");
  if (!file)
    return 1;

  char *lines = malloc(SIZE * SIZE);

  for (u32 i = 0; i < SIZE; ++i) {
    const u32 idx = i * SIZE;
    fgets(lines + idx, SIZE, file);
  }
  fclose(file);

  /****** PART 1 ******/
  u32 count = 0;

  // First line
  for (u32 j = 0; j < SIZE; ++j) {
    if (lines[j] == 'S') {
      lines[j + SIZE] = '|';
    }
  }

  // The rest
  for (u32 i = 1; i < SIZE - 1; ++i) {
    char *const line = lines + i * SIZE;
    for (u32 j = 0; j < SIZE; ++j) {
      if (line[j] == '|') {
        char *const nextLine = lines + (i + 1) * SIZE;
        if (nextLine[j] == '.') {
          nextLine[j] = '|';
        } else if (nextLine[j] == '^') {
          ++count;
          nextLine[j + 1] = '|';
          nextLine[j - 1] = '|';
        }
      }
    }
  }

  /****** PART 2 ******/
  u64 *numbers = calloc(SIZE * SIZE, sizeof(u64));

  // First line
  for (u32 j = 0; j < SIZE; ++j) {
    if (lines[j] == 'S') {
      numbers[j] = 1;
      ++numbers[j + SIZE];
    }
  }

  // The rest
  for (u32 i = 1; i < SIZE - 1; ++i) {
    char *const line = lines + i * SIZE;
    u64 *const lineNumbers = numbers + i * SIZE;

    for (u32 j = 0; j < SIZE; ++j) {
      if (lineNumbers[j] > 0) {
        char *const nextLine = lines + (i + 1) * SIZE;
        u64 *const nextLineNumbers = numbers + (i + 1) * SIZE;
        if (nextLine[j] == '^') {
          nextLineNumbers[j + 1] += lineNumbers[j];
          nextLineNumbers[j - 1] += lineNumbers[j];
        } else {
          nextLineNumbers[j] += lineNumbers[j];
        }
      }
    }
  }

  u64 possibleWorlds = 0;
  u64 *const lastLineNumbers = numbers + (SIZE - 1) * SIZE;

  for (u32 j = 0; j < SIZE; ++j) {
    possibleWorlds += lastLineNumbers[j];
  }

  // prettyLines(lines);
  // prettyNumbers64(numbers);

  free(numbers);
  free(lines);

  printf("%u\n", count);
  printf("%lu\n", possibleWorlds);
  return 0;
}