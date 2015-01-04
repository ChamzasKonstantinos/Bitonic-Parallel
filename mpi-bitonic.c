

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mpi.h"

struct timeval startwtime, endwtime;
double seq_time;

int N;          // data array size
int *a,*b;         // data array to be sorted

const int ASCENDING  = 1;
const int DESCENDING = 0;


void init(void);
void print(void);
void sort(void);
void test(void);
inline void exchange(int i, int j);
void compare(int i, int j, int dir);
void bitonicMerge(int lo, int cnt, int dir);
void recBitonicSort(int lo, int cnt, int dir);
void impBitonicSort(void);

/** the main program **/ 
	

    /** the main program **/
int main(int argc, char **argv) {                        
														 
														 
  int taskid,numtasks;                                   
  	
  if (argc != 2) {
  printf("Usage: %s q\n  where n=2^q is problem size (power of two)\n",
	   argv[0]);
  exit(1);
  }													 
  MPI_Init(&argc, &argv);                                
  MPI_Comm_rank(MPI_COMM_WORLD, &taskid);                
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);              
  MPI_Status status;                                     
  a = (int *) malloc(N * sizeof(int));
  srand(taskid);
  printf("Hi I am thread %d and this is my array before sorting ",taskid );
  init();
  print();
  printf("Hi I am thread %d and this is my array after sorting ",taskid );
  qsort(a, N, sizeof(int), cmpfunc);
  print();												 
  int numworkers = numtasks-1;                           
  int source, dest, nbytes, mtype,                       
	  //~ intsize = sizeof(int), dbsize = sizeof(double),
	  //~ rows, averow, extra, offset;                   
  double start, finish;                                  
  double maxr = (double)RAND_MAX;                        
  N = 1<<atoi(argv[1]);
 
  
 
 
 //EDW MPAINEIS!!!!!!!!!!!!!!!!!!!
 if (taskid == MASTER) {
	start = MPI_Wtime();
	printf(" I am Master thread and i do nothing for now:) \n ");
	for (dest=1; dest<=numworkers; dest++) {                   
	  printf("   sending %d rows to task %d\n",rows,dest);
	  MPI_Send(&offset      ,         1,    MPI_INT, dest, FROM_MASTER, MPI_COMM_WORLD);
	  MPI_Send(&rows        ,         1,    MPI_INT, dest, FROM_MASTER, MPI_COMM_WORLD);
	  MPI_Send(&A[offset][0], rows*dim2, MPI_DOUBLE, dest, FROM_MASTER, MPI_COMM_WORLD);
	  MPI_Send(&B           , dim2*dim3, MPI_DOUBLE, dest, FROM_MASTER, MPI_COMM_WORLD);
	  offset = offset + rows;
	}
	sleep(4);
	finish = MPI_Wtime();
	printf("time for bitonic sorting is %d",finish - start);
  }  // end of master task
  // MAKRIA APO EDW!!!!!!!!!!!!!!!!
  else{
	  
	  
	  
  }
  MPI_Finalize();
 
  return 0;
}



/** -------------- SUB-PROCEDURES  ----------------- **/ 

/** procedure compare(): qsort use it  **/
int compare(const void* a, const void* b)
{
	return (*(int*)a - *(int*)b);
	
}



/** procedure test() : verify sort results **/
void test() {
  int pass = 1;
  int i;
  for (i = 1; i < N; i++) {
    pass &= (a[i-1] <= a[i]);
  }

  printf(" TEST %s\n",(pass) ? "PASSed" : "FAILed");
}


/** procedure init() : initialize array "a" with data **/
void init() {
  int i;
  for (i = 0; i < N; i++) {
    a[i] = rand() % N; // (N - i);
  }
}

/** procedure  print() : print array elements **/
void print() {
  int i;
  for (i = 0; i < N; i++) {
    printf("%d\n", a[i]);
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
   if dir = ASCENDING, and in descending order otherwise. 
   The sequence to be sorted starts at index position lo,
   the parameter cbt is the number of elements to be sorted. 
 **/make[1]: Leaving directory `/home/petros/Desktop/Parallila/mpi'

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



/** function recBitonicSort() 
    first produces a bitonic sequence by recursively sorting 
    its two halves in opposite sorting orders, and then
    calls bitonicMerge to make them in the same order 
 **/
void recBitonicSort(int lo, int cnt, int dir) {
  if (cnt>1) {
    int k=cnt/2;
    recBitonicSort(lo, k, ASCENDING);
    recBitonicSort(lo+k, k, DESCENDING);
    bitonicMerge(lo, cnt, dir);
  }
}


/** function sort() 
   Caller of recBitonicSort for sorting the entire array of length N 
   in ASCENDING order
 **/
void sort() {
  recBitonicSort(0, N, ASCENDING);
}

