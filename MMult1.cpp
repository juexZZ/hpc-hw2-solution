// g++ -std=c++11 -O3 -march=native MMult1.cpp && ./a.out

#include <stdio.h>
#include <math.h>
// #include <omp.h> 
#include "utils.h"

#define BLOCK_SIZE 32

// Note: matrices are stored in column major order; i.e. the array elements in
// the (m x n) matrix C are stored in the sequence: {C_00, C_10, ..., C_m0,
// C_01, C_11, ..., C_m1, C_02, ..., C_0n, C_1n, ..., C_mn}
void MMult0(long m, long n, long k, double *a, double *b, double *c) {
  for (long j = 0; j < n; j++) {
    for (long p = 0; p < k; p++) {
      for (long i = 0; i < m; i++) {
        double A_ip = a[i+p*m];
        double B_pj = b[p+j*k];
        double C_ij = c[i+j*m];
        C_ij = C_ij + A_ip * B_pj;
        c[i+j*m] = C_ij;
      }
    }
  }
}

void MMult1(long m, long n, long k, double *a, double *b, double *c) {
  // TODO: See instructions below
  long ib = BLOCK_SIZE;
  long kb = BLOCK_SIZE;
  // #pragma omp parallel for
  for (long ii = 0; ii < m; ii += ib)
  {
    for (long kk = 0; kk < k; kk += kb)
    {
        for (long j=0; j < n; j += 2)
        {
            for (long i = ii; i < ii + ib; i += 2)
            {
                // double acc00, acc01, acc10, acc11;
                // if (kk == 0)
                //     acc00 = acc01 = acc10 = acc11 = 0;
                // else
                // {
                //     acc00 = c[i+j*m]; //C[i + 0][j + 0];
                //     acc01 = c[i+(j+1)*m]; //C[i + 0][j + 1];
                //     acc10 = c[i+1+j*m]; //C[i + 1][j + 0];
                //     acc11 = c[i+1+(j+1)*m]; //C[i + 1][j + 1];
                // }
                double acc00 = c[i+j*m]; //C[i + 0][j + 0];
                double acc01 = c[i+(j+1)*m]; //C[i + 0][j + 1];
                double acc10 = c[i+1+j*m]; //C[i + 1][j + 0];
                double acc11 = c[i+1+(j+1)*m]; //C[i + 1][j + 1];
                for (long kp = kk; kp < kk + kb; kp++)
                {
                  acc00 += b[kp+j*k] * a[i+kp*m]; //B[kp][j + 0] * A[i + 0][kp];
	                acc01 += b[kp+(j+1)*k] * a[i+kp*m]; //B[kp][j + 1] * A[i + 0][kp];
	                acc10 += b[kp+j*k] * a[i+1+kp*m]; //B[kp][j + 0] * A[i + 1][kp];
	                acc11 += b[kp+(j+1)*k] * a[i+1+kp*m]; //B[kp][j + 1] * A[i + 1][kp];
                }
                c[i+(j+0)*m] = acc00;
                c[i+(j+1)*m] = acc01;
                c[i+1+(j+0)*m] = acc10;
                c[i+1+(j+1)*m] = acc11;
            }
        }
    }
  }
}

int main(int argc, char** argv) {
  const long PFIRST = BLOCK_SIZE;
  const long PLAST = 2000;
  const long PINC = std::max(50/BLOCK_SIZE,1) * BLOCK_SIZE; // multiple of BLOCK_SIZE

  printf(" Dimension       Time    Gflop/s       GB/s        Error\n");
  for (long p = PFIRST; p < PLAST; p += PINC) {
    long m = p, n = p, k = p;
    long NREPEATS = 1e9/(m*n*k)+1;
    double* a = (double*) aligned_malloc(m * k * sizeof(double)); // m x k
    double* b = (double*) aligned_malloc(k * n * sizeof(double)); // k x n
    double* c = (double*) aligned_malloc(m * n * sizeof(double)); // m x n
    double* c_ref = (double*) aligned_malloc(m * n * sizeof(double)); // m x n

    // Initialize matrices
    for (long i = 0; i < m*k; i++) a[i] = drand48();
    for (long i = 0; i < k*n; i++) b[i] = drand48();
    for (long i = 0; i < m*n; i++) c_ref[i] = 0;
    for (long i = 0; i < m*n; i++) c[i] = 0;

    for (long rep = 0; rep < NREPEATS; rep++) { // Compute reference solution
      MMult0(m, n, k, a, b, c_ref);
    }

    Timer t;
    t.tic();
    for (long rep = 0; rep < NREPEATS; rep++) {
      MMult1(m, n, k, a, b, c);
    }
    double time = t.toc();
    // double flops = 0; // TODO: calculate from m, n, k, NREPEATS, time
    double flops = NREPEATS * (2*k*m*n) / time / (1.0*1e9);
    // double bandwidth = 0; // TODO: calculate from m, n, k, NREPEATS, time
    double bandwidth = NREPEATS * 2*m*k*n*(BLOCK_SIZE+1) / BLOCK_SIZE / time / (1.0*1e9);
    printf("%10ld %10f %10f %10f", p, time, flops, bandwidth);

    double max_err = 0;
    for (long i = 0; i < m*n; i++) max_err = std::max(max_err, fabs(c[i] - c_ref[i]));
    printf(" %10e\n", max_err);

    aligned_free(a);
    aligned_free(b);
    aligned_free(c);
  }

  return 0;
}

// * Using MMult0 as a reference, implement MMult1 and try to rearrange loops to
// maximize performance. Measure performance for different loop arrangements and
// try to reason why you get the best performance for a particular order?
//
//
// * You will notice that the performance degrades for larger matrix sizes that
// do not fit in the cache. To improve the performance for larger matrices,
// implement a one level blocking scheme by using BLOCK_SIZE macro as the block
// size. By partitioning big matrices into smaller blocks that fit in the cache
// and multiplying these blocks together at a time, we can reduce the number of
// accesses to main memory. This resolves the main memory bandwidth bottleneck
// for large matrices and improves performance.
//
// NOTE: You can assume that the matrix dimensions are multiples of BLOCK_SIZE.
//
//
// * Experiment with different values for BLOCK_SIZE (use multiples of 4) and
// measure performance.  What is the optimal value for BLOCK_SIZE?
//
//
// * What percentage of the peak FLOP-rate do you achieve with your code?
//
//
// NOTE: Compile your code using the flag -march=native. This tells the compiler
// to generate the best output using the instruction set supported by your CPU
// architecture. Also, try using either of -O2 or -O3 optimization level flags.
// Be aware that -O2 can sometimes generate better output than using -O3 for
// programmer optimized code.
