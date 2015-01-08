#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>

#define ITERATION_NUM 10


struct timeval startwtime, endwtime;
double seq_time,sumseq_time,average_seq_time;

int q;          
int p;
int N;          // data array size
int *a;         // data array to be sorted

const int ASCENDING  = 1;
const int DESCENDING = 0;

int cmpfuncA(const void* aa, const void* bb);
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
int main(int argc, char **argv) {
  int n_proc,j;
  if (argc != 3) {
    printf("Usage: %s q\n  where n=2^q is problem size (power of two)\n", 
	   argv[0]);
    exit(1);
  }
  q = atoi(argv[1]);
  n_proc = atoi(argv[2]);
  N = (1<<q)*n_proc;
  a = (int *) malloc(N * sizeof(int));
  char nt1,nt2,nt3,q1,q2;
  if (n_proc<10) {
    nt1 = (char)((int)'0');
    nt2 = (char)((int)'0');
    nt3 = (char)(((int)'0')+n_proc);
  }
  else if(n_proc<100) {
    nt1 = (char)(((int)'0'));
    nt2 = (char)(((int)'0')+n_proc/10);
    nt3 = (char)(((int)'0')+n_proc%10);
  }
  else {
    nt1 = '1';
    nt2 = '2';
    nt3 = '8';
  }
  if(q==20)  q1 = '2';
  else q1 = '1';
  q2 = (char)(((int)'0')+q%10);
  char filename[17];
  filename[0]='r';
  filename[1]='e';
  filename[2]='s';
  filename[3]='u';
  filename[4]='l';
  filename[5]='t';
  filename[6]='s';
  filename[7]=nt1;
  filename[8]=nt2;
  filename[9]=nt3;
  filename[10]=q1;
  filename[11]=q2;
  filename[12]='.';
  filename[13]='t';
  filename[14]='x';
  filename[15]='t';
  filename[16]='\0';
  FILE *f = fopen(filename, "w");
  if (f == NULL)
  {
      printf("Error opening file!\n");
      exit(1);
  }
  
  for(j=0;j<ITERATION_NUM;j++){
  init();

  gettimeofday (&startwtime, NULL);
  qsort(a, N, sizeof(int), cmpfuncA);
  gettimeofday (&endwtime, NULL);

  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);

  printf("Imperative wall clock time = %f\n", seq_time);
  sumseq_time+=seq_time;
  if(j==ITERATION_NUM-1){
    average_seq_time = sumseq_time/ITERATION_NUM;
    printf("Average imperative wall clock time = %f\n", average_seq_time);
    fprintf(f, " %d\n %d\n %f\n",n_proc,q,average_seq_time);
  }
  test();
}
  fclose(f);
  
}


/** -------------- SUB-PROCEDURES  ----------------- **/ 

int cmpfuncA(const void* aa, const void* bb)
{
  return (*(int*)aa - *(int*)bb);
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

