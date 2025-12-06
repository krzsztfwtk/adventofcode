#include "../types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N_ROWS 4
#define MAX_LINE_LENGHT 4096

const u8 N_LINES = N_ROWS + 1;

int main() {
  FILE *file = fopen("day6.txt", "r");
  if (file == NULL)
    return 1;

  char *numbersLines = malloc(N_ROWS * MAX_LINE_LENGHT);
  char *numbers[N_ROWS];

  for (u32 i = 0; i < N_ROWS; ++i) {
    const u32 idx = i * MAX_LINE_LENGHT;
    fgets(numbersLines + idx, MAX_LINE_LENGHT, file);
    numbers[i] = numbersLines + idx;
  }

  char *operatorsLine = malloc(MAX_LINE_LENGHT);
  fgets(operatorsLine, MAX_LINE_LENGHT, file);
  char *operator = operatorsLine;

  fclose(file);

  u64 sum1 = 0;

  while (*operator) {
    switch (*operator) {
    case '+':
      for (u8 i = 0; i < N_ROWS; ++i) {
        char *end;
        sum1 += strtoul(numbers[i], &end, 10);
        numbers[i] = end;
      }
      break;

    case '*':
      char *end;
      u64 result = strtoul(numbers[0], &end, 10);
      numbers[0] = end;
      for (u8 i = 1; i < N_ROWS; ++i) {
        result *= strtoul(numbers[i], &end, 10);
        numbers[i] = end;
      }
      sum1 += result;
      break;
    }
    ++operator;
  }

  // reset pointers for part 2
  for (u32 i = 0; i < N_ROWS; ++i) {
    const u32 idx = i * MAX_LINE_LENGHT;
    numbers[i] = numbersLines + idx;
  }

  // use pointers arithmetic to not iterate again with strlen
  // writing this comment took probably more time than 10^6 calls of strlen
  const u32 lineLenght = operator - operatorsLine;
  u64 sum2 = 0;
  u64 result = 0;

  enum Operation : u8 { OP_NONE, ADD, MULTIPLY };
  enum Operation operation = OP_NONE;

  // -1 to skip '\n' char, in may depend on specific input format
  for (u32 i = 0; i < lineLenght - 1; ++i) {
    if (!operation) {
      if (operatorsLine[i] == '+') {
        operation = ADD;
        result = 0;
      } else {
        operation = MULTIPLY;
        result = 1;
      }
    }

    u32 number = 0;
    u32 magnitude = 1;

    for (u8 j = N_ROWS - 1; j < N_ROWS; --j) {
      const char digit = *(numbers[j] + i);

      if (digit != ' ') {
        number += (digit - '0') * magnitude;
        magnitude *= 10;
      }
    }

    if (number == 0) {
      sum2 += result;
      operation = OP_NONE;
      continue;
    }

    if (operation == ADD)
      result += number;
    else
      result *= number;
  }

  sum2 += result;

  printf("%lu\n", sum1);
  printf("%lu\n", sum2);
  free(operatorsLine);
  free(numbersLines);
}