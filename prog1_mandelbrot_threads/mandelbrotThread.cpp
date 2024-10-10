#include <stdio.h>
#include <thread>

#include "CycleTimer.h"

typedef struct {
    float x0, x1;
    float y0, y1;
    unsigned int width;
    unsigned int height;
    int maxIterations;
    int* output;
    int threadId;
    int numThreads;
} WorkerArgs;

extern void mandelbrotSerial(float x0, float y0, float x1, float y1, int width,
                             int height, int startRow, int numRows,
                             int maxIterations, int output[]);

extern void mandelbrotSerial1(float x0, float y0, float x1, float y1, int width,
                              int height, int startRow, int stepsize,
                              int maxIterations, int output[]);

//
// workerThreadStart --
//
// Thread entrypoint.
void workerThreadStart(WorkerArgs * const args) {

    // TODO FOR CS149 STUDENTS: Implement the body of the worker
    // thread here. Each thread should make a call to mandelbrotSerial()
    // to compute a part of the output image.  For example, in a
    // program that uses two threads, thread 0 could compute the top
    // half of the image and thread 1 could compute the bottom half.

    // printf("Hello world from thread %d\n", args->threadId);

    // 1.3 time the thing
    double startTime = CycleTimer::currentSeconds();
    // use threadId to distinguish what to do -> call mandelbrotSerial()
    // -> done

    // depending on threadId:
    //  - start row
    //  - total row
    // the rest remains
    // 1.1: uses 2 processors
    /*int startRow = args->threadId ? 0 : (args->height / 2);
    mandelbrotSerial(args->x0, args->y0, args->x1, args->y1, args->width,
                     args->height, startRow, // start row,
                     args->height / 2,       // total rows
                     args->maxIterations,
                     args->output // can i just do this?
    );*/
    // 1.2: uses 2~8 threads (./mandelbrot -t 8)
    // let's just make it very much like CUDA
    /*int height_d = (args->height) / (args->numThreads);
    int startRow = (args->threadId) * height_d;
    if (args->threadId == args->numThreads - 1)
      height_d = (args->height) - height_d * (args->numThreads - 1);
    mandelbrotSerial(args->x0, args->y0, args->x1, args->y1, args->width,
                     args->height,
                     startRow, // start row,
                     height_d, // total rows
                     args->maxIterations,
                     args->output // can i just do this?
    );*/
    // 1.4: distribute the workload evenly to gain performance
    /*
    mandelbrotSerial1(args->x0, args->y0, args->x1, args->y1, args->width,
                      args->height,
                      args->threadId,   // start row,
                      args->numThreads, // total rows
                      args->maxIterations,
                      args->output // can i just do this?
    );*/
    // 1.4
    // distribute the workload evenly without creating a new function
    int startRow;
    for (startRow = args->threadId; startRow < *(int *)(&args->height);
         startRow += args->numThreads) {
      mandelbrotSerial(args->x0, args->y0, args->x1, args->y1, args->width,
                       args->height, startRow, 1, args->maxIterations,
                       args->output);
    }

    // requires sync because of args->output?: no because we're writing to
    //  different address ?
    double endTime = CycleTimer::currentSeconds();
    double t_expense = endTime - startTime;
    printf("[time for thread %d creation]:\t\t[%.3f] ms\n", args->threadId,
           t_expense * 1000);
}

//
// MandelbrotThread --
//
// Multi-threaded implementation of mandelbrot set image generation.
// Threads of execution are created by spawning std::threads.
void mandelbrotThread(
    int numThreads,
    float x0, float y0, float x1, float y1,
    int width, int height,
    int maxIterations, int output[])
{
    static constexpr int MAX_THREADS = 32;

    if (numThreads > MAX_THREADS)
    {
        fprintf(stderr, "Error: Max allowed threads is %d\n", MAX_THREADS);
        exit(1);
    }

    // Creates thread objects that do not yet represent a thread.
    std::thread workers[MAX_THREADS];
    WorkerArgs args[MAX_THREADS];

    for (int i=0; i<numThreads; i++) {

        // TODO FOR CS149 STUDENTS: You may or may not wish to modify
        // the per-thread arguments here.  The code below copies the
        // same arguments for each thread
        args[i].x0 = x0;
        args[i].y0 = y0;
        args[i].x1 = x1;
        args[i].y1 = y1;
        args[i].width = width;
        args[i].height = height;
        args[i].maxIterations = maxIterations;
        args[i].numThreads = numThreads;
        args[i].output = output;

        args[i].threadId = i;
    }

    // Spawn the worker threads.  Note that only numThreads-1 std::threads
    // are created and the main application thread is used as a worker
    // as well.
    for (int i=1; i<numThreads; i++) {
        workers[i] = std::thread(workerThreadStart, &args[i]);
    }

    workerThreadStart(&args[0]);

    // join worker threads
    for (int i=1; i<numThreads; i++) {
        workers[i].join();
    }
}

