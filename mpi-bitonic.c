#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include "mpi.h"

#define MASTER 0
#define FROM_WORKER 2
#define ITERATION_NUM 10


struct timeval startwtime, endwtime;
double seq_time,sumseq_time,average_seq_time;
int j;

int N;          // data array size
int *a,*b,*c;         // data array to be sorted( b ,c for testing purposes)
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
  b = (int *) malloc(( 3* sizeof(int));
  c=(int *) malloc(( 3* sizeof(int));
  MPI_Barrier(MPI_COMM_WORLD);
  int t;
  for(t=0;t<ITERATION_NUM;t++){
    srand(taskid);
    init();
    
    if(taskid==MASTER) gettimeofday (&startwtime, NULL);
    if ((taskid+1)%2) qsort(a, N, sizeof(int), cmpfuncA);
    else qsort(a, N, sizeof(int), cmpfuncB);

    CHUNK =1;
    if (q>14)(CHUNK = CHUNK<<(q-14));

    MPI_Barrier(MPI_COMM_WORLD);
    //One thread i used as master to count time
   

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

    //printf("Imperative wall clock time = %f\n", seq_time);
    sumseq_time+=seq_time;
    if(t==ITERATION_NUM-1){
      average_seq_time = sumseq_time/ITERATION_NUM;
      printf(" %d\n %d\n %f\n",numtasks,q,average_seq_time);
    }
    
     int i;
     int sum=0,pass=1;
     int prev, next;
     prev = a[N-1];
     sum=test();
     for(i=1;i<numtasks;i++){
        
        MPI_Recv(c, 3, MPI_INT,i, FROM_WORKER,MPI_COMM_WORLD, &status);
        sum=sum+c[0];
        next = c[1];
        if (prev>next) pass=0;
        prev=c[2];
     }
         if(sum==numtasks && pass) printf("TEST PASSed \n");
         else printf("TEST FAILed \n");
    }
    else{
      b[0]=test();
      b[1]=a[0];
      b[2]=a[N-1];
      MPI_Send (b,3,MPI_INT,0,FROM_WORKER,MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
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
int test() {
  int pass = 1;
  int i;
  for (i = 1; i < N*numtasks; i++) {
    pass &= (a[i-1] <= a[i]);
  }

  //printf(" TEST %s\n",(pass) ? "PASSed" : "FAILed");
  return pass;
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
/** INLINE procedure exchange() : pair swap **/
}

}
