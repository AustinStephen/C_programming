/*
 Author: Austin Stephen
 Date: April 18, 2022

 COSC 3750, Homework 11

 My implementation of matrix multiplication.

 Notes on implementation:
- Threads approach allows only locking the mutex over the thread index counter because 
  I did the matrix math to edit the result array all simultaneously (dont even have to lock a row).

- Time looks a little weird but this was only way I found to not meaure CPU time
  but wall time so you can see how my threads make it faster.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

void *thread_function(void* structPtr);

typedef struct MatrixStore{
  double *result;
  double *A;
  double *B;
  int m1;
  int n1;
  int n2;
  int numThreads;

  // stores the work each thread does
  int work;
  // stores the work the last thread does 
  // could be different from uneven division
  int workLast;
} MatStore;

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int finished;
int counter = 0;

int main(int argc, char* argv[]){

  // buffers for reading in the bytes of data
  char dblBuff[8];
  // dimensions of the matricies
  int m1, n1, m2, n2; // m is rows n is cols
  int numThreads;
  //timing 
  struct timespec start, end;
  double time;

// check enough args
  if(argc < 4){
    printf("Need at least 3 files \n");
    return 1;
  } 
  else if (argc == 5){
    if(argv[4] > 0){
      numThreads = atoi(argv[4]);
    }
  }
  else{
    numThreads =0;
  }

// Read first file

  // the "b" has no effect but makes the code more portable
  FILE *fp = fopen(argv[1], "rb");
  if( fp == NULL){         
    perror(argv[1]);
    exit(EXIT_FAILURE);
  }
  // reading the dimensions
  fread(&m1,sizeof(int),1, fp);
  fread(&n1,sizeof(int),1, fp);

  // check positive dimensions
  if(m1 < 1 || n1 < 1){
  printf("Cannot have negative dimensions %d %d\n", m1, n1);
  return(1);
  }

  // matrix
  double *A = calloc(m1*n1, sizeof(double)); // A[m1*n1];
  int location = 0;
  // reading the values of the matrix
  while (fread(dblBuff, sizeof(double), 1, fp) == 1){
    double d = *((double*)dblBuff);
    A[location] = d;
    location++;
  }
fclose(fp);

// Read second file

  // the "b" has no effect but makes the code more portable
  FILE *fp2 = fopen(argv[2], "rb");
  if( fp2 == NULL){         
    perror(argv[2]);
    exit(EXIT_FAILURE);
  }
  // reading the dimensions
  fread(&m2,sizeof(int),1, fp2);
  fread(&n2,sizeof(int),1, fp2);

  // check positive dimensions
  if(m2 < 1 || n2 < 1){
  printf("Cannot have negative dimensions %d %d\n", m1, n1);
  return(1);
  }

  //matrix
  double *B = calloc(m2*n2, sizeof(double)); //double B[m2*n2];
  location = 0;
  // reading the values of the matrix
  while (fread(dblBuff, sizeof(double), 1, fp2) == 1){
    double d = *((double*)dblBuff);
    B[location] = d;
    location++;
  }
fclose(fp2);

// Check if the matricies are valid dimensions
  if(n1 != m2){
    printf("Dimension mismatch! %d and %d are not equal \n", n1, m2);
    free(A);
    free(B);
    return(1);
  }

// perfrom matrix multiplication with threads
// result matrix
  double *result= calloc(m1*n2, sizeof(double));

  // If there are threads to be used 
  if(numThreads > 0){
    // array of threads
    pthread_t thread_id[numThreads];
    
    // Struct with the result and indicies in the the loops
    struct MatrixStore Result;

    // initalize the struct with values
    Result.result = result;
    Result.A= A;
    Result.B= B;
    Result.m1 = m1;
    Result.n1 = n1;
    Result.n2 = n2;
    Result.numThreads = numThreads;

    // Number of computations per thread:
    // Each matrix does its share of the work by integer division. 
    // The last thread does the remaining work from potentially 
    // uneven division. If there is 1 thread it does all of the
    // work.
    if(numThreads == 1){
    Result.workLast = (m1 * n1 * n2);
    }
    else{ 
    Result.work = ((m1 * n1 * n2)/(numThreads));
    Result.workLast = ((m1 * n1 * n2) - (Result.work *(numThreads-1)));
    }

    // Approach to using the threads:
    // Above I find the number of computations it will reqire 
    // to build the result matrix. This is divided by the #
    // of threads because each thread should be responsible 
    // for that porportion of computations. Then each thread 
    // is created and computes its portion of the result. The
    // "main" thread waits on the rest of the threads before
    // writting the result to a file as specified.


    // start the time
    clock_gettime(CLOCK_MONOTONIC, &start);

    // coutners for making sure the threads all join
    int i, j;

    for(i=0; i < numThreads; i++)
    {
        pthread_create( &thread_id[i], NULL, thread_function, &Result );
    }

    for(j=0; j < numThreads; j++)
    {
        pthread_join( thread_id[j], NULL); 
    }                                              
    // end time
    clock_gettime(CLOCK_MONOTONIC, &end);
    time = (end.tv_sec - start.tv_sec);

    // Write result to a file in binary
    FILE *wrPtr = fopen(argv[3],"wb");

    // if the file could be opened 
    if(wrPtr != NULL){

      // write the dimensions
      fwrite(&m1, sizeof(int),1,wrPtr);
      fwrite(&n2, sizeof(int),1,wrPtr);

      for(int d =0; d < (m1 *n2); d++){
        fwrite(&result[d], sizeof(double),1,wrPtr);
      }
      fclose(wrPtr);
    }
    else{
      perror(argv[3]);
    }

  }
  // Perfrom the matrix multiplication without threads
  else{
    // start time
    clock_gettime(CLOCK_MONOTONIC, &start);

    // algorithm to conduct the multiplication
    for(int i=0; i < m1; i++){
      for(int j=0; j < n2; j++){
        for(int k=0; k < n1; k++){
          result[i*n2 + j] += A[i*n1 +k] * B[k*n2 + j];
        }
      }
    }
    // end time
    clock_gettime(CLOCK_MONOTONIC, &end);
    time = (end.tv_sec - start.tv_sec);

    // Write result to a file in binary
    FILE *wrPtr = fopen(argv[3],"wb");

    // if the file could be opened
    if(wrPtr != NULL){

      fwrite(&m1, sizeof(int),1,wrPtr);
      fwrite(&n2, sizeof(int),1,wrPtr);

      for(int d =0; d < (m1 *n2); d++){
        fwrite(&result[d], sizeof(double),1,wrPtr);
      }
      fclose(wrPtr);
    }
    else{
      perror(argv[3]);
    }
  }

  // Printing run diagnostics 
  
  printf("Matrix Sizes:\n");
  printf("    M: %d\n", m1);
  printf("    N: %d\n", n1);
  printf("    P: %d\n",n2);

  printf("\nWorker Threads: %d\n",numThreads);

  printf("\nTotal time: %0.0f second(s).\n",time);

  free(A);
  free(B);
  free(result);
  return 0;
}

// Threads function:
// Uses the struct to access all of the fields required to 
// update the matrix. The index of the for-loop if manually
// updated so the current spot to be updatedS can be shared 
// between all of the other threads.
void *thread_function(void *structPtr)
{
  MatStore *data = (MatStore*)structPtr;
  int thread;
  // locking the shared memory
  pthread_mutex_lock( &mutex1 );
  // figure out which thread you are
  thread = counter;
  // don't let anyone else be the same thread
  counter++;
  // done modifying shared memory
  pthread_mutex_unlock( &mutex1 );

  // getting copies of the dimensions for the 
  // problem
  int n1 = data->n1;
  int n2 = data->n2;
  int m1 = data->m1;
  int numThreads = data->numThreads;
  int work;
  int workNormal;
  int i;
  int j;
  int k;

  // set the work depending if youre the last thread
  // or just another thread
  if(thread == (numThreads -1)){
    work = data->workLast;
    workNormal = data->work; 
    
    // need to find place in the array
    // compute your starting i,j,k based on which thread you
    // are. This is how they modify different places in
    // the array all at the same time
    k = (thread * workNormal) % n1;
    j = ((thread * workNormal) / n1) % n2;
    i = (thread * workNormal) / (n1 *n2) % m1;
  }
  else{
    work = data->work;

    // need to find place in the array
    // compute your starting i,j,k based on which thread you
    // are. This is how they modify different places in
    // the array all at the same time
    k = (thread * work) % n1;
    j = ((thread * work) / n1) % n2;
    i = (thread * work) / (n1 *n2) % m1;
  }

  // Loop until the work for that thread is finished by decementing
  // work
  while( work >0){
    // innermost loop increment logic
    if ( k == n1){
      k = 0;
      j = j +1;
    }
    // second loop
    if ( j == n2){
      j = 0;
      i = i +1;
    }
    // third loop
    if ( i == m1){
      finished = 1; 
    }

    // update a weight
    if(finished != 1){
      data->result[i*n2 + j] += data->A[i*n1 +k] * data->B[k*n2 + j];
    }
    // increment k
    k = k +1;
    // one unit of work complete
    work--;

  }

    return(structPtr); 
}