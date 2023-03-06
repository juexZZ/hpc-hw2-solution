# Report for homework2
Juexiao Zhang

experiment machine: AMD Ryzen 7 5700G with Radeon Graphics, which has 8 cpu cores, and a cache size of 512 KB. System: windows sub Linux system Ubuntu 22.

## Question 1
---

please see the comments in the corresponding code files

## Question 2
---
The timings of different settings can be found in the log directory `mmult-logs`. Specify as follows:

 - `O3Bx.log`: timings for different matrix sizes under optimization flag -O3, with block size x
 - `O3ref.log`: timings of running function `MMult0` (the original version) under -O3 as a reference.
 - `O3omp1.log`: openMP version with -O3 and `OMP_NUM_THREADS=1`

Among the different block sizes I found **32** is the best.
 
 Also note that the `MMult0` under -O3 have a rather good performance and this is because the compiler does a great deal of optimization. We can see a more obvious speedup under -O0:

`MMult0` under -O0:
```
Dimension       Time    Gflop/s       GB/s        Error
        32   3.386950   0.590510   0.608963 0.000000e+00
        64   3.431868   0.582819   0.601032 0.000000e+00
        96   3.393147   0.589798   0.608230 0.000000e+00
       128   3.402752   0.587960   0.606334 0.000000e+00
       160   3.415354   0.587652   0.606016 0.000000e+00
       192   3.419784   0.587792   0.606160 0.000000e+00
```

Block `MMult1` under -O0:
```
Dimension       Time    Gflop/s       GB/s        Error
        32   1.407440   1.421039   1.465447 0.000000e+00
        64   1.418946   1.409609   1.453659 0.000000e+00
        96   1.422450   1.406920   1.450886 0.000000e+00
       128   1.451882   1.377993   1.421055 0.000000e+00
       160   1.427472   1.406010   1.449948 0.000000e+00
       192   1.437757   1.398094   1.441785 0.000000e+00
```

## Question 3
---
AVX version for `sin4_intrin()` and `sin_vec()` is implemented in the source file.


## Question 4
---

### (a)

Experiment logs are in the directory `pipeline-logs`:
- `allfuncs.log` shows timings and flop rates of the different functions discussed in the book. We can see that unrolling by 4 is faster than by 2.
- `unroll2sizes.log` and `unroll4sizes.log` shows the timings of unrolling 2 and unrolling 4 for different vector lengths respectively. We can see that length 4194304 is where the time has a sudden increase. This indicates the vectors can no longer be fit into the cache.


### (b)

- **compute.cpp**
  
when running the command suggested in the comment, the outputs of **compute.cpp** under different optimization flags:
**multiply-add**:
| O3                   |          O2          |          O1          |          O0           |
| :------------------- | :------------------: | :------------------: | :-------------------: |
| 0.878982 seconds     |   0.878103 seconds   |   2.862484 seconds   |   3.334673 seconds    |
| 2.900860 cycles/eval | 2.897913 cycles/eval | 9.446394 cycles/eval | 11.004615 cycles/eval |
| 2.275184 Gflop/s     |   2.277497 Gflop/s   |   0.698679 Gflop/s   |   0.599748 Gflop/s    |

**division**:
| O3                   |          O2          |          O1           |          O0           |
| :------------------- | :------------------: | :-------------------: | :-------------------: |
| 2.959592 seconds     |   2.975849 seconds   |   4.509251 seconds    |   4.930273 seconds    |
| 9.766868 cycles/eval | 9.820526 cycles/eval | 14.880718 cycles/eval | 16.270086 cycles/eval |
| 0.675754 Gflop/s     |   0.672061 Gflop/s   |   0.443527 Gflop/s    |   0.405652 Gflop/s    |

**sqrt**:
| O3                    |          O2           |          O1           |          O0           |
| :-------------------- | :-------------------: | :-------------------: | :-------------------: |
| 4.499548 seconds      |   4.513189 seconds    |   6.493499 seconds    |   6.168601 seconds    |
| 14.848636 cycles/eval | 14.893717 cycles/eval | 21.428766 cycles/eval | 20.356540 cycles/eval |
| 0.444485 Gflop/s      |   0.443140 Gflop/s    |   0.307997 Gflop/s    |   0.324220 Gflop/s    |

**sin**:
| O3                    |          O2           |          O1           |          O0           |
| :-------------------- | :-------------------: | :-------------------: | :-------------------: |
| 7.132642 seconds      |   7.154899 seconds    |   8.166835 seconds    |   8.259182 seconds    |
| 23.537874 cycles/eval | 23.611318 cycles/eval | 26.950735 cycles/eval | 27.255557 cycles/eval |
| 0.280399 Gflop/s      |   0.279527 Gflop/s    |   0.244891 Gflop/s    |   0.242152 Gflop/s    |

- **compute-vec.cpp**
   

   when using optimization flag -O3 with implicit vectorisation, the output is:
   ```
    compute-vec.cpp:16:21: optimized: loop vectorized using 32 byte vectors
    compute-vec.cpp:16:21: optimized:  loop versioned for vectorization because of possible aliasing
        compute-vec.cpp:52:21: optimized: loop vectorized using 16 byte vectors
        compute-vec.cpp:46:5: optimized: basic block part vectorized using 32 byte vectors
    time = 0.879205
    flop-rate = 9.098600 Gflop/s

    time = 0.878442
    flop-rate = 9.106789 Gflop/s

    time = 0.881654
    flop-rate = 9.073573 Gflop/s
    ```

    Note that the computation is 4 times more than the computation of `compute.cpp` since the vector length is 4 and is repeated for the same amount of time. So the auto-vectorization gives 4 times speedup compared with the `compute.cpp`.
    Experiments show that the pragmas `unroll` and `GCC ivdep` does not change the performance. This suggest that the compiler has already done the works suggest by the pragmas when we compile with the implicit vectorization flags.

    Using openMP, for optimization flag -O3, the output is:

    ```
    time = 0.880678
    flop-rate = 9.083412 Gflop/s

    time = 0.881578
    flop-rate = 9.074323 Gflop/s

    time = 0.879792
    flop-rate = 9.092741 Gflop/s
    ```

    Implicit and explicit vectorization give roughly the same performance. And such observation is consistent across different optimization flags. This makes sense since they are doing the same vectorization and can obtain the same level of parallelism. 

    When using flag -O0 and -O1, there is a clear speed difference: fn0 > fn1 > fn2 while they are basically the same when using flags -O2 and -O3. This shows that vector intrinsics gain the most benefits from compiler optimization.

- **compute-vec-pipe.cpp**

    **Implicit Vectorization**: flop rate (Gflop/s) for all the three functions with different M:

    | M    |    1     |     2     |     4     |     8     |    16     |    32     |
    | :--- | :------: | :-------: | :-------: | :-------: | :-------: | :-------: |
    | fn0  | 9.058724 | 18.194633 | 36.360510 | 56.787868 | 57.599702 | 31.552505 |
    | fn1  | 9.091181 | 18.163392 | 36.279317 | 55.057205 | 32.607620 | 32.051048 |
    | fn2  | 9.104850 | 18.153487 | 36.271088 | 55.028363 | 32.438479 | 30.974262 |

    The performances show that M = 8 gives the peak performance on this 8-core machine.

    **openMP**: flop rate (Gflop/s) for all the three functions with different M:

    | M    |    1     |     2     |     4     |     8     |    16     |    32     |
    | :--- | :------: | :-------: | :-------: | :-------: | :-------: | :-------: |
    | fn0  | 9.093459 | 18.220608 | 36.264081 | 32.138932 | 57.980313 | 32.242794 |
    | fn1  | 9.112367 | 18.167002 | 35.990228 | 35.780188 | 31.934670 | 34.335481 |
    | fn2  | 9.107717 | 18.215529 | 36.141719 | 35.716261 | 31.918655 | 34.058868 |

    OpenMP gives a slightly different results especially when M=8, and this may be because the implicit vectorization is more compatible with the specific hardware while openMP induces some extra cost for being more general.