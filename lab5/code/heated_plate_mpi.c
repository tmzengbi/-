# include <stdlib.h>
# include <stdio.h>
# include <math.h>
# include <mpi/mpi.h>
# include <string.h>

#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))
int main ( int argc, char *argv[] );


int getLocal(int tot, int myrank, int size) {
  int localsize = tot / size;
  int x = size - tot % size;
  if(myrank >= x)
    ++localsize;
  return localsize;
}

/******************************************************************************/

int main ( int argc, char *argv[] )

/******************************************************************************/
/*
  Purpose:

    MAIN is the main program for HEATED_PLATE_OPENMP.

  Discussion:

    This code solves the steady state heat equation on a rectangular region.

    The sequential version of this program needs approximately
    18/epsilon iterations to complete. 


    The physical region, and the boundary conditions, are suggested
    by this diagram;

                   W = 0
             +------------------+
             |                  |
    W = 100  |                  | W = 100
             |                  |
             +------------------+
                   W = 100

    The region is covered with a grid of M by N nodes, and an N by N
    array W is used to record the temperature.  The correspondence between
    array indices and locations in the region is suggested by giving the
    indices of the four corners:

                  I = 0
          [0][0]-------------[0][N-1]
             |                  |
      J = 0  |                  |  J = N-1
             |                  |
        [M-1][0]-----------[M-1][N-1]
                  I = M-1

    The steady state solution to the discrete heat equation satisfies the
    following condition at an interior grid point:

      W[Central] = (1/4) * ( W[North] + W[South] + W[East] + W[West] )

    where "Central" is the index of the grid point, "North" is the index
    of its immediate neighbor to the "north", and so on.
   
    Given an approximate solution of the steady state heat equation, a
    "better" solution is given by replacing each interior point by the
    average of its 4 neighbors - in other words, by using the condition
    as an ASSIGNMENT statement:

      W[Central]  <=  (1/4) * ( W[North] + W[South] + W[East] + W[West] )

    If this process is repeated often enough, the difference between successive 
    estimates of the solution will go to zero.

    This program carries out such an iteration, using a tolerance specified by
    the user, and writes the final estimate of the solution to a file that can
    be used for graphic processing.

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    18 October 2011

  Author:

    Original C version by Michael Quinn.
    This C version by John Burkardt.

  Reference:

    Michael Quinn,
    Parallel Programming in C with MPI and OpenMP,
    McGraw-Hill, 2004,
    ISBN13: 978-0071232654,
    LC: QA76.73.C15.Q55.

  Local parameters:

    Local, double DIFF, the norm of the change in the solution from one iteration
    to the next.

    Local, double MEAN, the average of the boundary values, used to initialize
    the values of the solution in the interior.

    Local, double U[M][N], the solution at the previous iteration.

    Local, double W[M][N], the solution computed at the latest iteration.
*/
{
  MPI_Init(NULL, NULL);
# define M 500
# define N 500

  double diff;
  double epsilon = 0.001;
  int i;
  int iterations;
  int iterations_print;
  int j;
  double mean;
  // double my_diff;
  double u[M][N];
  double w[M][N];
  double wtime;


  int myrank;
  int size;
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  if(myrank == 0) {
    printf ( "\n" );
    printf ( "HEATED_PLATE_OPENMP\n" );
    printf ( "  C/MPI version\n" );
    printf ( "  A program to solve for the steady state temperature distribution\n" );
    printf ( "  over a rectangular plate.\n" );
    printf ( "\n" );
    printf ( "  Spatial grid of %d by %d points.\n", M, N );
    printf ( "  The iteration will be repeated until the change is <= %e\n", epsilon ); 
    printf ( "  Number of processors available = %d\n", size);
  }
/*
  Set the boundary values, which don't change. 
*/
  mean = 0.0;
  memset(w, 0 ,sizeof w);
  for ( i = myrank + 1; i < M - 1; i+= size )
  {
    w[i][0] = 100.0;
  }
  for ( i = myrank + 1; i < M - 1; i+=size )
  {
    w[i][N-1] = 100.0;
  }
  for ( j = myrank; j < N; j+=size )
  {
    w[M-1][j] = 100.0;
  }
  for ( j = myrank; j < N; j+=size )
  {
    w[0][j] = 0.0;
  }
  MPI_Allreduce(MPI_IN_PLACE, w, N * M, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

/*
  Average the boundary values, to come up with a reasonable
  initial value for the interior.
*/
  for ( i = myrank + 1; i < M - 1; i += size)
  {
    mean = mean + w[i][0] + w[i][N-1];
  }
  for ( j = myrank; j < N; j += size )
  {
    mean = mean + w[M-1][j] + w[0][j];
  }
  MPI_Allreduce(MPI_IN_PLACE, &mean, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
/*
  OpenMP note:
  You cannot normalize MEAN inside the parallel region.  It
  only gets its correct value once you leave the parallel region.
  So we interrupt the parallel region, set MEAN, and go back in.
*/

  mean = mean / ( double ) ( 2 * M + 2 * N - 4 );
  if(myrank == 0) {
    printf ( "\n" );
    printf ( "  MEAN = %f\n", mean );
  }
/* 
  Initialize the interior solution to the mean value.
*/
  for(i = 1; i < M - 1; ++ i)
    for ( j = 1; j < N - 1; j++ )
      w[i][j] = mean;
/*
  iterate until the  new solution W differs from the old solution U
  by no more than EPSILON.
*/
  iterations = 0;
  iterations_print = 1;
  if(myrank == 0) {
    printf ( "\n" );
    printf ( " Iteration  Change\n" );
    printf ( "\n" );
  }
  
  diff = epsilon;
  int startPos[size], local[size];
  for(i = 0; i < size; ++ i)
    local[i] = getLocal(M, i, size);
  startPos[0] = 0;
  for(i = 1; i < size; ++ i)
    startPos[i] = startPos[i - 1] + local[i - 1];
  int recv[size], disp[size];
  for(int i = 0; i < size; ++ i) {
    recv[i] = local[i] * N;
    disp[i] = startPos[i] * N;
  }
  wtime = MPI_Wtime( );
  while ( epsilon <= diff )
  {
/*
  Save the old solution in U.
*/
    for(i = max(0, startPos[myrank] - 1); i < min(M , startPos[myrank] + local[myrank] + 1); ++ i) {
      for(j = 0; j < N; ++ j) {
        u[i][j] = w[i][j];
      }
    }
/*
  Determine the new estimate of the solution at the interior points.
  The new solution W is the average of north, south, east and west neighbors.
*/
    for(i = startPos[myrank]; i < startPos[myrank] + local[myrank]; ++ i) {
      if(i == 0 || i == M - 1) continue;
      for(j = 1; j < N - 1; ++ j) {
        w[i][j] = ( u[i-1][j] + u[i+1][j] + u[i][j-1] + u[i][j+1] ) / 4.0;
      }
    }
    MPI_Allgatherv(MPI_IN_PLACE, N * M, MPI_DOUBLE, w, recv, disp , MPI_DOUBLE, MPI_COMM_WORLD);

/*
  C and C++ cannot compute a maximum as a reduction operation.

  Therefore, we define a private variable MY_DIFF for each thread.
  Once they have all computed their values, we use a CRITICAL section
  to update DIFF.
*/
    diff = 0.0;
    for(i = startPos[myrank]; i < startPos[myrank] + local[myrank]; ++ i) {
      if(i == 0 || i == M - 1) continue;
      for ( j = 1; j < N - 1; j++ )
      {
        if ( diff < fabs ( w[i][j] - u[i][j] ) )
        {
          diff = fabs ( w[i][j] - u[i][j] );
        }
      }
    }

    MPI_Allreduce(MPI_IN_PLACE, &diff, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    iterations++;

    if (iterations == iterations_print )
    {
      if(myrank == 0)
        printf ( "  %8d  %.10lf\n", iterations, diff );
      iterations_print = 2 * iterations_print;
    }
  }
  wtime = MPI_Wtime ( ) - wtime;
  if(myrank == 0) {
    printf ( "\n" );
    printf ( "  %8d  %f\n", iterations, diff );
    printf ( "\n" );
    printf ( "  Error tolerance achieved.\n" );
    printf ( "  Wallclock time = %f\n", wtime );
  /*
    Terminate.
  */
    printf ( "\n" );
    printf ( "HEATED_PLATE_MPI:\n" );
    printf ( "  Normal end of execution.\n" );
  }
  MPI_Finalize();
  return 0;

# undef M
# undef N
}

/*
HEATED_PLATE_OPENMP
  C/OpenMP version
  A program to solve for the steady state temperature distribution
  over a rectangular plate.

  Spatial grid of 500 by 500 points.
  The iteration will be repeated until the change is <= 1.000000e-03
  Number of processors available = 16
  Number of threads =              16

  MEAN = 74.949900

 Iteration  Change

         1  18.7374749499
         2  9.3687374749
         4  4.0988226453
         8  2.2895767120
        16  1.1366042532
        32  0.5682013654
        64  0.2828045279
       128  0.1417770314
       256  0.0708080045
       512  0.0354270147
      1024  0.0177073867
      2048  0.0088556735
      4096  0.0040200462
      8192  0.0016084011

     11449  0.001000

  Error tolerance achieved.
  Wallclock time = 3.743082

HEATED_PLATE_OPENMP:
  Normal end of execution.

*/