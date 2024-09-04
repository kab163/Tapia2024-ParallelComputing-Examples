/* Copyright (c) 2015, Texas State University. All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted for academic, research, experimental, or personal use provided
that the following conditions are met:

   * Redistributions of source code must retain the above copyright notice,
   * this list of conditions and the following disclaimer.
   
   * Redistributions in binary form must reproduce the above copyright notice,
   * this list of conditions and the following disclaimer in the documentation
   * and/or other materials provided with the distribution.
   
   * Neither the name of Texas State University nor the names of its
   * contributors may be used to endorse or promote products derived from this
   * software without specific prior written permission.
   
   * For all other uses, please contact the Office for Commercialization 
   * and Industry
   * Relations at Texas State University <http://www.txstate.edu/ocir/>.

   * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
   * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
   * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
   * FOR A PARTICULAR PURPOSE ARE DISCLAIMED IN NO EVENT SHALL THE COPYRIGHT 
   * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
   * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   *(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
   * AND ON
   * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   *(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
   * OF THIS
   * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   *  Author: Martin Burtscher <burtscher@txstate.edu>                         
*/

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <sys/time.h>
#include <mpi.h>

#include "writeBMP.hpp"

#define xMin 0.74395
#define xMax 0.74973
#define yMin 0.11321
#define yMax 0.11899

int main(int argc, char *argv[])
{
  int row, col, depth, width, maxdepth, local_width;
  double cx, cy, dx, dy, x, y, x2, y2;
  unsigned char *cnt;
  unsigned char *local_cnt;
  struct timeval start, end;
  int comm_sz, my_rank;
  double local_start, local_finish, local_elapsed, elapsed;
  int my_start, my_end;
  
  /* Initialize MPI Library and get # of processes/ assigned ranks */
  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  if (my_rank ==0){
    printf("Fractal v1.2\n");
    printf("There are %d processes available\n", comm_sz);}

  /* check command line */
  if (argc != 3) 
    {fprintf(stderr, "usage: %s edge_length max_depth\n", argv[0]); exit(-1);}
  width = atoi(argv[1]);
  if (width < 10) 
    {fprintf(stderr, "edge_length must be at least 10\n"); exit(-1);}
  maxdepth = atoi(argv[2]);
  if (maxdepth < 10) 
    {fprintf(stderr, "max_depth must be at least 10\n"); exit(-1);}

  /* width is an integer multiple of the # of processes */
  assert(width % comm_sz ==0);

  /* array for local computations*/
  local_cnt = (unsigned char *)malloc((width / comm_sz) * width * 
    sizeof(unsigned char));
  
  if (NULL == local_cnt) {
    fprintf(stderr, "could not allocate memory\n");
    exit(-1);
  }

  if(my_rank ==0){
    printf("computing %d by %d fractal with a maximum depth of %d\n", width, 
      width, maxdepth);

    /* allocate array for entire fractal */
    cnt = (unsigned char *)malloc(width * width * sizeof(unsigned char));
    if (cnt == NULL) 
      {fprintf(stderr, "could not allocate memory\n"); exit(-1);}
  }
  
  /* start time */
  MPI_Barrier(MPI_COMM_WORLD);
  if (my_rank ==0)
    gettimeofday(&start, NULL);

  local_width = width / comm_sz;

  /* compute fractal */
  dx = (xMax - xMin) / width;
  dy = (yMax - yMin) / width;
  my_start = my_rank * width / comm_sz;
  for (row = 0; row  < local_width; row++) {
    cy = yMin + (my_start + row) * dy;
    cx = xMin;
    local_width = width / comm_sz;
    for (col = 0; col < width; col++) { 
      cx = xMin + col *dy;
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
      local_cnt[row * width + col] = depth & 255; 
    }
  }

  MPI_Gather(local_cnt, (local_width * width), MPI_UNSIGNED_CHAR, cnt, 
    (local_width * width), MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD); 

  if(my_rank ==0){
    gettimeofday(&end, NULL); /* end time */

    /* print time */
    printf("compute time: %.4f s\n", end.tv_sec + end.tv_usec / 
      1000000.0 - start.tv_sec - start.tv_usec / 1000000.0);

    /* verify result by writing it to a file */
    if (width <= 1024)
      WriteBMP(width, width, cnt, "fractal.bmp");

    if (cnt != NULL){
      free(cnt);
      cnt = NULL;
    }
  }
  
  if (local_cnt != NULL){
    free(local_cnt);
    local_cnt = NULL;
  }

  MPI_Finalize();
  return 0;
}

