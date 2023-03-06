// assignment 2 question 4 (a)
// g++ -std=c++11 -O3 -march=native pipelining.cpp -ftree-vectorize -fopt-info-vec-optimized -o pipelining.out && ./pipelining.out -n 100

#include <stdio.h>
#include "utils.h"

// #define N 50000

void dot_vanilla(double* a, double* b, double &result, long N){
    double s = 0;
    for (long i=0; i<N; i++){
        // printf("calculating\n");
        s += a[i] * b[i];
    }
    result = s;
}

void dot_unroll2(double* a, double* b, double &result, long N){
    double s=0, sum1=0, sum2=0;
    for (long i = 0; i < N; i=i+2)
    {
        sum1 += *(a+0) * *(b+0);
        sum2 += *(a+1) * *(b+1);
        a += 2;
        b += 2;
    }
    result = sum1 + sum2;
}

void dot_unroll4(double* a, double* b, double &result, long N){
    double s=0, sum1=0, sum2=0, sum3=0, sum4=0;
    for (long i = 0; i < N; i=i+4)
    {
        sum1 += *(a+0) * *(b+0);
        sum2 += *(a+1) * *(b+1);
        sum3 += *(a+2) * *(b+2);
        sum4 += *(a+3) * *(b+3);
        a += 4;
        b += 4;
    }
    result = sum1 + sum2 + sum3 + sum4;
}

void dot_disentangle2(double* a, double* b, double &result, long N){
    double s=0, sum1=0, sum2=0;
    for (long i = 0; i < N; i=i+2)
    {
        double temp1 = *(a+0) * *(b+0);
        double temp2 = *(a+1) * *(b+1);
        sum1 += temp1;
        sum2 += temp2;
        a += 2;
        b += 2;
    }
    result = sum1 + sum2;
}

void dot_rearrange2(double* a, double* b, double &result, long N){
    double s=0, sum1=0, sum2=0;
    double temp1=0, temp2=0;
    for (long i = 0; i < N; i=i+2)
    {   
        sum1 += temp1;
        temp1 = *(a+0) * *(b+0);

        sum2 += temp2;
        temp2 = *(a+1) * *(b+1);
        
        a += 2;
        b += 2;
    }
    sum1 += temp1; // the last time
    sum2 += temp2;
    result = sum1 + sum2;
}

void dot_rearrange4(double* a, double* b, double &result, long N){
    double s=0, sum1=0, sum2=0, sum3=0, sum4=0;
    double temp1=0, temp2=0, temp3=0, temp4=0;
    for (long i = 0; i < N; i=i+4)
    {   
        sum1 += temp1;
        temp1 = *(a+0) * *(b+0);

        sum2 += temp2;
        temp2 = *(a+1) * *(b+1);

        sum3 += temp3;
        temp3 = *(a+3) * *(b+3);

        sum4 += temp4;
        temp4 = *(a+4) * *(b+4);
        
        a += 4;
        b += 4;
    }
    sum1 += temp1; // the last time
    sum2 += temp2;
    sum3 += temp3;
    sum4 += temp4;
    result = sum1 + sum2 + sum3 + sum4;
}


int main(int argc, char** argv) {
  Timer t;
  long N = read_option<long>("-n", argc, argv);
  long repeat = 1e9 / N + 1;
  double* A = (double*) aligned_malloc(N * sizeof(double));
  double* B = (double*) aligned_malloc(N * sizeof(double)); 
  double result;

  for (long i = 0; i < N; i++) A[i] = drand48();
  for (long i = 0; i < N; i++) B[i] = drand48();

  printf("vanilla dot product\n");
  t.tic();
  for (long i = 0; i < repeat; i++) dot_vanilla(A, B, result, N);
  printf("time = %f\n", t.toc());
  printf("flop-rate = %f Gflop/s\n\n", N*2*repeat/1e9/t.toc());

  printf("unrolling by 2\n");
  t.tic();
  for (long i = 0; i < repeat; i++) dot_unroll2(A, B, result, N);
  printf("time = %f\n", t.toc());
  printf("flop-rate = %f Gflop/s\n\n", N*2*repeat/1e9/t.toc());

  printf("disentangled 2\n");
  t.tic();
  for (long i = 0; i < repeat; i++) dot_disentangle2(A, B, result, N);
  printf("time = %f\n", t.toc());
  printf("flop-rate = %f Gflop/s\n\n", N*2*repeat/1e9/t.toc());

  printf("rearranged by 2\n");
  t.tic();
  for (long i = 0; i < repeat; i++) dot_rearrange2(A, B, result, N);
  printf("time = %f\n", t.toc());
  printf("flop-rate = %f Gflop/s\n\n", N*2*repeat/1e9/t.toc());

  printf("unrolling by 4\n");
  t.tic();
  for (long i = 0; i < repeat; i++) dot_unroll4(A, B, result, N);
  printf("time = %f\n", t.toc());
  printf("flop-rate = %f Gflop/s\n\n", N*2*repeat/1e9/t.toc());

  printf("rearranged by 4\n");
  t.tic();
  for (long i = 0; i < repeat; i++) dot_rearrange4(A, B, result, N);
  printf("time = %f\n", t.toc());
  printf("flop-rate = %f Gflop/s\n\n", N*2*repeat/1e9/t.toc());


//   printf("result: %.6f\n", result);
  aligned_free(A);
  aligned_free(B);

  return 0*result;
}