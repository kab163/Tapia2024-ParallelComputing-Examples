# Tapia2024-ParallelComputing-Examples
Using a Fractal code to demonstrate using various Parallel Programming techniques

## Serial Fractal

First, take a look at the serial implementation of the Fractal code.
It is important to understand how the serial code works first before trying to
exploit any parallelism.

You can run the code with:

```
gcc fractal-serial.cpp -o fractal
./fractal 1024
```

The executable will need one argument, the width of the fractal to be generated.
A good sample size is 1024. This will create a 1024 x 1024 fractal image. It will
be output as a .bmp file. You can open the file and check the image. Checking that
the image looks correct is a great way to check the correctness of the program.

If you try to run the executable with the incorrect arguments, you will see a "usage" error.

Take note of the runtime after running this serial fractal program. In my case, this is
what I saw:

```


```

## OpenMP Fractal

Next, take a look at the OpenMP implementation of the Fractal code.
This implementation is using a Shared Memory parallelization paradigm with OpenMP.

You can run the code with:

```
gcc -fopenmp fractal-omp.cpp -o fractal-omp
./fractal-omp 1024
``` 

Similar as before, the executable just needs one argument, the width. Again, take
note of the runtime from this OpenMP parallelization of the fractal code. How does it
differ from the serial runtime?

In my case, this is what I saw after running the OpenMP version:

```


```

## MPI Fractal

Finally, take a look at the MPI implementation of the Fractal code. MPI is a bit
more difficult to run with since you need a distributed memory system to fully
take advantage of MPI. Getting the MPI dependencies may be a bit tricky.

Once you have MPI set up, you can build and run with:

```
mpicc fractal-mpi.cpp -o fractal-mpi
mpiexec -n 1 fractal-mpi 1024 256
```

Note that I am starting out by running with just one process. By doing this, I can
then check the resulting fractal image and ensure correctness before running the fractal
with more than one process.

Next, I can run with:

```
mpiexec -n 2 fractal-mpi 1024 256
```

In my case, after changing to 2 processes, this is the resulting output:

```


```

## DIY Fractal Implementations

Now that you have seen a bit more of how to run the fractal with OpenMP and MPI, try
editing the code yourself and playing around with the number of threads/processes, the
scheduling of threads, and other parameters. Here are a few suggestions:


