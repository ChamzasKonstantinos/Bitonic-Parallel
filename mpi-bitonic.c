#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include "mpi.h"

#define MASTER 0
#define FROM_WORKER 2

struct timeval startwtime, endwtime;
double seq_time;

int N;          // data array size
int *a,*b;         // data array to be sorted( b for testing purposes)
int taskid,numtasks;
int CHUNK;        //chunk  will break the data MPI_Isends
//Initializes with marix a with N random numbers
void init(void);
//Prints matrix a
void print(void);
//Tests if the sorting was correct
void test(void);
//used in ascending qsort 
int cmpfuncA(const void* aa, const void* bb);
//used in descending qsort 
int cmpfuncB(const void* aa, const void* bb);
// exchanges elements with indexes aa and bb in matrix a 
inline void exchange(int i, int j);
//compares and  exchanges the specified elements
void compare(int i, int j, int dir);
//bitonic merging of a bitonic sequence
void bitonicMerge(int lo, int cnt, int dir);


/** the main program **/
int main(int argc, char **argv) {

  if (argc != 2) {
    printf("Usage: %s q\n  where n=2^q is problem size (power of two)\n",
    argv[0]);
    exit(1);
    }

  int q;
  q = atoi(argv[1]);
  // N is the number of elements each matrix holds
  N = 1<<q;
  
  //initilize MPI
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  MPI_Status status;
  MPI_Request request;

  //Initialize the matrices for each task
  a = (int *) malloc(N* sizeof(int));
  b = (int *) malloc((N*numtasks) * sizeof(int));
  srand(taskid);
  init();
  if ((taskid+1)%2) qsort(a, N, sizeof(int), cmpfuncA);
  else qsort(a, N, sizeof(int), cmpfuncB);

  CHUNK =1;
  if (q>14)(CHUNK = CHUNK<<(q-14));

  MPI_Barrier(MPI_COMM_WORLD);
  //One thread i used as master to count time
  if(taskid==MASTER) gettimeofday (&startwtime, NULL);

  int offset,k;
  //k is the number of proccesors tha hold 1 bitonic sequence
  for (k = 2; k <= numtasks; k = 2*k) {
    //offset is the number distance between elements that must be compared
    for (offset = k >> 1; offset > 0 ; offset = offset >> 1) {
      
      int partner_id = taskid^offset;
      // First CHUNK of the message
      int j=0;
      int hchunk=2*CHUNK;
      for(j=0;j<CHUNK;j++){
      if(taskid<partner_id){
          MPI_Isend (&a[(N*j)/hchunk+(N/2)],N/hchunk,MPI_INT,partner_id,FROM_WORKER,MPI_COMM_WORLD,&request);
          MPI_Recv(&a[(N*j)/hchunk+(N/2)],N/hchunk, MPI_INT,partner_id, FROM_WORKER,MPI_COMM_WORLD, &status);
          }
      else{
        MPI_Isend (&a[(N*j)/hchunk],N/hchunk,MPI_INT,partner_id,FROM_WORKER,MPI_COMM_WORLD,&request);
        MPI_Recv(&a[(N*j)/hchunk], N/hchunk, MPI_INT,partner_id, FROM_WORKER,MPI_COMM_WORLD, &status);
      }
      int i;
      for (i=(N*j)/hchunk; i<((N*j)/hchunk+N/hchunk); i++){
        compare(i,i+N/2, !(bool)(k&taskid));
      }
      if(taskid<partner_id){
          MPI_Isend (&a[(N*j)/hchunk+(N/2)],N/hchunk,MPI_INT,partner_id,FROM_WORKER,MPI_COMM_WORLD,&request);
          MPI_Recv(&a[(N*j)/hchunk+(N/2)],N/hchunk, MPI_INT,partner_id, FROM_WORKER,MPI_COMM_WORLD, &status);
          }
      else{
        MPI_Isend (&a[(N*j)/hchunk],N/hchunk,MPI_INT,partner_id,FROM_WORKER,MPI_COMM_WORLD,&request);
        MPI_Recv(&a[(N*j)/hchunk], N/hchunk, MPI_INT,partner_id, FROM_WORKER,MPI_COMM_WORLD, &status);
      }
    }
       MPI_Barrier(MPI_COMM_WORLD);
    }
    bitonicMerge(0, N, !(bool)(k&taskid));
  }
  
  if(taskid==MASTER){
    gettimeofday (&endwtime, NULL);

  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
         + endwtime.tv_sec - startwtime.tv_sec);

  printf("Imperative wall clock time = %f\n", seq_time);
    int i;
   for(i=1;i<numtasks;i++){
      MPI_Recv(&b[N*i], N, MPI_INT,i, FROM_WORKER,MPI_COMM_WORLD, &status);
   }
   for(i=0;i<N;i++){
      b[i]=a[i];
   }
   test();
  }
  else{
    MPI_Send (a,N,MPI_INT,0,FROM_WORKER,MPI_COMM_WORLD);
  }
  MPI_Finalize();

  return 0;
}



/** -------------- SUB-PROCEDURES  ----------------- **/

/** procedure compare(): qsort use it  **/
int cmpfuncA(const void* aa, const void* bb)
{
  return (*(int*)aa - *(int*)bb);

}

/** procedure compare(): qsort use it  **/
int cmpfuncB(const void* aa, const void* bb)
{
  return (*(int*)bb - *(int*)aa);

}


/** procedure test() : verify sort results **/
void test() {
  int pass = 1;
  int i;
  for (i = 1; i < N*numtasks; i++) {
    pass &= (b[i-1] <= b[i]);
  }

  printf(" TEST %s\n",(pass) ? "PASSed" : "FAILed");
}


/** procedure init() : initialize array "a" with data **/
void init() {
  int i;
  for (i = 0; i <(N); i++) {
    a[i] = rand() % N; // (N - i);
    }
  }


/** procedure  print() : print array elements **/
void print() {
  int i;
  
  for (i = 0; i < N; i++) {
    printf(" |%d| ", a[i]);
  }
  printf("\n");
}


/** INLINE procedure exchange() : pair swap **/
inline void exchange(int i, int j) {
  int t;
  t = a[i];
  a[i] = a[j];
  a[j] = t;
}



/** procedure compare()
   The parameter dir indicates the sorting direction, ASCENDING
   or DESCENDING; if (a[i] > a[j]) agrees with the direction,
   then a[i] and a[j] are interchanged.
**/
inline void compare(int i, int j, int dir) {
  if (dir==(a[i]>a[j]))
    exchange(i,j);
}

/** Procedure bitonicMerge()
   It recursively sorts a bitonic sequence in ascending order,
   if dir = 1, and in descending order otherwise.
   The sequence to be sorted starts at index position lo,
   the parameter cbt is the number of elements to be sorted.
 **/

void bitonicMerge(int lo, int cnt, int dir) {
  if (cnt>1) {
    int k=cnt/2;
    int i;
    for (i=lo; i<lo+k; i++)
      compare(i, i+k, dir);
    bitonicMerge(lo, k, dir);
    bitonicMerge(lo+k, k, dir);
  }
}



