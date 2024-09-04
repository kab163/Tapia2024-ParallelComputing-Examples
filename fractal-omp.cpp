#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <sys/time.h>
#include <omp.h>

#include "writeBMP.hpp"

#define xMin 0.74395
#define xMax 0.74973
#define yMin 0.11321
#define yMax 0.11899

int main(int argc, char *argv[])
{
  int row, col, depth, width, maxdepth;
  double cx, cy, dx, dy, x, y, x2, y2;
  unsigned char *cnt;
  struct timeval start, end;
  int thread_count;

  printf("Fractal v1.2\n");

  /* check command line */
  if (argc != 3) {fprintf(stderr, "usage: %s edge_length max_depth\n", argv[0]); exit(-1);}
  width = atoi(argv[1]);
  if (width < 10) {fprintf(stderr, "edge_length must be at least 10\n"); exit(-1);}
  maxdepth = atoi(argv[2]);
  if (maxdepth < 10) {fprintf(stderr, "max_depth must be at least 10\n"); exit(-1);}

  printf("computing %d by %d fractal with a maximum depth of %d\n", width, width, maxdepth);
  
  thread_count = 4;
  printf("there are %d threads with %d chunk size\n", thread_count, 1);
  
  /* allocate arrays */
  cnt = (unsigned char *)malloc(width * width * sizeof(unsigned char));
  if (cnt == NULL) {fprintf(stderr, "could not allocate memory\n"); exit(-1);}

  /* start time */
  gettimeofday(&start, NULL);

  /* compute fractal */
  dx = (xMax - xMin) / width;
  dy = (yMax - yMin) / width;
# pragma omp parallel for num_threads(thread_count) \
    schedule (static, 1) default(none) private(row, col, depth, x2, y2, x, y, cx, cy) \
    shared(width, dx, dy, maxdepth, cnt)
  for (row = 0; row < width; row++) {
    cy = yMin + row * dy;
    for (col = 0; col < width; col++) {
      cx = xMin + col * dx;
      x = -cx;
      y = -cy;
      depth = maxdepth;
      do {
        x2 = x * x;
        y2 = y * y;
        y = 2 * x * y - cy;
        x = x2 - y2 - cx;
        depth--;
      } while ((depth > 0) && ((x2 + y2) <= 5.0));
      cnt[row * width + col] = depth & 255;
    }
  }

  /* end time */
  gettimeofday(&end, NULL);
  printf("compute time: %.4f s\n", end.tv_sec + end.tv_usec / 1000000.0 - start.tv_sec - start.tv_usec / 1000000.0);

  /* verify result by writing it to a file */
  if (width <= 1024) {
    WriteBMP(width, width, cnt, "fractal.bmp");
  }

  free(cnt);
  return 0;
}
