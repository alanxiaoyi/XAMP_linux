#include <stdio.h>
#include <math.h>
#define SCALING_FACTOR 4

char *cseq_outfile;
char *profile_set_string;
int profile_level;
int profile_max, profile_min;
int warmup;
char *trace_outfile;
FILE *TF;

typedef struct
{
	int tag;
	int count;
} matrix;

typedef struct 
{
char cseq_outfile_set[256];

FILE *Fp;
int hitOrMiss, num_hits, num_misses, hitPoint, nsets, nassoc, cseqrows, nbsize;
int exthitOrMiss, extnum_hits, extnum_misses, exthitPoint, dmax;
matrix **cache; 
matrix **extcache;

int **cseqtable;
double *weightedAvg;
int *stackDist;

}profile_cache;

profile_cache* pcache1[10];
profile_cache* pcache2[10];
