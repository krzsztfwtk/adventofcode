#include "../types.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_POINTS_NUM 512

typedef struct {
  i64 x;
  i64 y;
} Point;

Point points[MAX_POINTS_NUM];

static inline u64 calculateArea(const Point p1, const Point p2) {
  const u64 width = llabs(p1.x - p2.x) + 1;
  const u64 height = llabs(p1.y - p2.y) + 1;

  return width * height;
}

int main() {
  FILE *file = fopen("day9.txt", "r");
  if (!file)
    return 1;

  char buff[16];

  u32 i = 0;

  while (fgets(buff, 16, file)) {
    char *end;
    points[i] = (Point){strtoll(buff, &end, 10), strtoll(end + 1, NULL, 10)};
    ++i;
  }
  fclose(file);
  const u32 pointsNum = i;

  u64 maxArea = 0;

  for (i = 0; i < pointsNum; ++i) {
    for (u32 j = i + 1; j < pointsNum; ++j) {
      u64 area = calculateArea(points[i], points[j]);
      if (area > maxArea) {
        maxArea = area;
      }
    }
  }

  printf("%llu\n", maxArea);

  return 0;
}