/**
  * Please refer to License.txt for License information.
 */

#include "acappProfiler.h"

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "cache.h"

void initcache(profile_cache* pc, int numsets, int numassoc, int bsize);
//void printset(int set);
void extprintset(profile_cache* pc, int set);
//void hitStackShift(int setNum, int hitPoint);
void exthitStackShift(profile_cache* pc, int setNum, int exthitPoint);
//void missStackShift(int setNum, int tag);
void extmissStackShift(profile_cache* pc, int setNum, int tag);
//int hitOrMissFunc(int setNum, int tag);
int exthitOrMissFunc(profile_cache* pc, md_addr_t addr);
void printCacheStats(profile_cache* pc );
void calcWeightedAvg(profile_cache* pc );
/*
#define SCALING_FACTOR 4
extern char *cseq_outfile;
FILE *Fp;
typedef struct
{
	int tag;
	int count;
} matrix;

int hitOrMiss, num_hits, num_misses, hitPoint, nsets, nassoc, cseqrows, nbsize;
int exthitOrMiss, extnum_hits, extnum_misses, exthitPoint, dmax;
matrix **cache; 
matrix **extcache;

int **cseqtable;
double *weightedAvg;
int *stackDist;
*/
/**
  * This function initialize the cache and sets the size according 
  * to parameters numsets and nummasoc.
  */
void initcache(profile_cache* pc, int numsets, int numassoc, int bsize)
{
  int i,j;
  pc->dmax = SCALING_FACTOR * numassoc; //size of columns in cseqtable & assoc of pseudocache
  printf("Initializing Cache: sets %d\n", numsets);
  char integer_string[32];
  sprintf(integer_string, "%d", numsets);
  strcpy(pc->cseq_outfile_set,cseq_outfile);
  strcat(pc->cseq_outfile_set,integer_string);
  pc->hitOrMiss = 0; 
  pc->num_hits = 0;
  pc->num_misses = 0;
  pc->hitPoint = 0;

  pc->exthitOrMiss = 0; 
  pc->extnum_hits = 0;
  pc->extnum_misses = 0;
  pc->exthitPoint = 0;
  	  
  pc->nsets = numsets;	
  pc->nassoc = numassoc;	
  pc->nbsize = bsize;
  pc->cseqrows = 200;
  

  pc->extcache = malloc((pc->nsets) * sizeof(matrix));
  pc->cseqtable = malloc((pc->cseqrows) * sizeof(int*));			//yw: modify to int*
  pc->cache = malloc((pc->nsets) * sizeof(matrix));
  pc->weightedAvg = malloc((pc->dmax) * sizeof(double));
  pc->stackDist = malloc((pc->dmax+1) * sizeof(int));
  for ( i=0; i < pc->nsets; i++ ) {
    pc->cache[i] = malloc((pc->nassoc) * sizeof(matrix));
  }
    
    for ( i=0; i < (pc->nsets); i++ ) {
    for ( j=0; j < (pc->nassoc); j++ ) {
    	pc->cache[i][j].tag = 0;
	pc->cache[i][j].count = 0;

  
   	}
   }
  
  for ( i=0; i < pc->cseqrows; i++ ) {
    pc->cseqtable[i] = malloc((pc->dmax) * sizeof(int));   // yw: modify to dmax
  }
	
	for (j = 0; j < pc->cseqrows; j++) {		//yw: add init
		for (i = 0; i < pc->dmax; i++) {
			pc->cseqtable[j][i]=0;
		}
	}
	
	
  for ( i=0; i < pc->nsets; i++ ) {
    pc->extcache[i] = malloc((pc->dmax) * sizeof(matrix));
  }  
  

   
    for ( i=0; i < pc->nsets; i++ ) {
    for ( j=0; j < pc->dmax; j++ ) {
    	pc->extcache[i][j].tag = 0;
	pc->extcache[i][j].count = 0;

  
   	}
   }


if(trace_outfile!=NULL){
	printf("trace file output to %s \n", trace_outfile);
	TF = fopen(trace_outfile,"w");
}
  

  printf("Cache Initialization done: sets %d \n", numsets);
  
}

/** Test function that prints the contents of a particular set in the cache
 */
 /*
void printset(int set)
{

	int i;
	
	printf("\n");
	for (i = 0; i < nassoc; i++) {
		printf("%d ",cache[set][i].tag);
		
		
	}
	printf("\n");
	for (i = 0; i < nassoc; i++) {
	printf("%d ",cache[set][i].count);
	}	
	printf("\n");
		

}*/

void extprintset(profile_cache* pc, int set)
{

	int i;
	
	printf("\n");
	for (i = 0; i < pc->dmax; i++) {
		printf("%d ",pc->extcache[set][i].tag);
		
		
	}
	printf("\n");
	for (i = 0; i < pc->dmax; i++) {
	printf("%d ",pc->extcache[set][i].count);
	}	
	printf("\n");
		

}

void calcWeightedAvg(profile_cache* pc ){
	int i,n,rSum,weightedSum;
	
	
	for (i = 0; i < pc->dmax; i++){
		
		rSum = 0;
		weightedSum = 0;
		for (n = i+1; n < pc->cseqrows; n++) {		//yw: modify n=0 to n=i+1 since n>=d+1
		weightedSum+= (n+1)*(pc->cseqtable[n][i]);
		rSum += (pc->cseqtable[n][i]);
		
		}
		pc->stackDist[i] = rSum; //sum of all values in each d
		if (rSum != 0)		
		pc->weightedAvg[i] = (double)((double)weightedSum/(double)rSum);
		
		else pc->weightedAvg[i] = 0;
		
		
		
	}

pc->stackDist[pc->dmax] = pc->extnum_misses;
}

/*
 * This function shifs the stack accordingly when a hit occurs - LRU
 */

/*
 * This function shifs the stack accordingly when a hit occurs - LRU
 */
 /*
void hitStackShift(int setNum, int hitPoint)
{

	matrix temp;
	int i;
	
	cache[setNum][hitPoint].count++;
	temp = cache[setNum][hitPoint];
	
	if (hitPoint != 0) 
	{
	
		if (hitPoint == nassoc - 1) //if hit in last assoc # o0f cache
		{
			for (i = nassoc - 1; i > 0; i--)
			{
			
	cache[setNum][i] = cache[setNum][i-1];
				
			}
			cache[setNum][0] = temp;
		}
		else 
		{
			for (i = hitPoint; i > 0; i--)
			{
				cache[setNum][i] = cache[setNum][i-1];
				
			}
			cache[setNum][0] = temp;
			
		}
	}
	cache[setNum][0].count = 1;
	
	for (i = 1; i < nassoc; i++)
		if (cache[setNum][i].tag != 0)
		cache[setNum][i].count++;	
	/** NOT FOR THIS CACHE
	int n = temp.count;
	cseqtable[n-1][hitPoint]++;
	*/
/*	
}
*/

void exthitStackShift(profile_cache* pc, int setNum, int exthitPoint)
{

	matrix temp;
	int i;

	pc->extcache[setNum][exthitPoint].count++;

	temp = pc->extcache[setNum][exthitPoint];
	
	if (exthitPoint != 0) 
	{
	
		if (exthitPoint == pc->dmax - 1) //if hit in last assoc # o0f extcache
		{
			for (i = pc->dmax - 1; i > 0; i--)
			{
			
				pc->extcache[setNum][i] = pc->extcache[setNum][i-1];
				
			}
			pc->extcache[setNum][0] = temp;
		}
		else 
		{
			for (i = exthitPoint; i > 0; i--)
			{
				pc->extcache[setNum][i] = pc->extcache[setNum][i-1];
				
			}
			pc->extcache[setNum][0] = temp;
			
		}
	}
	pc->extcache[setNum][0].count = 1;
	
	for (i = 1; i < pc->dmax; i++)
		if (pc->extcache[setNum][i].tag != 0)
		pc->extcache[setNum][i].count++;	
	
	int n = temp.count;
	if(n>(pc->cseqrows)) n=pc->cseqrows;				//yw: add a check
	if(warmup<=0)
		pc->cseqtable[n-1][exthitPoint]++;
//  printf("%d",n);
//	printf("%d, %d\n", n, exthitPoint);
	
}

/*
 * This function shifts the stack accordingly when a miss occurs - LRU
 */
 /*
void missStackShift(int setNum, int tag)
{
	int i;
	
	for (i = nassoc - 1; i >= 0; i--)
	{
		cache[setNum][i] = cache[setNum][i-1];
		
	
	}
	cache[setNum][0].tag = tag;
	cache[setNum][0].count = 1;
	
	
	

	
	for (i = 1; i < nassoc; i++){
	if (cache[setNum][i].tag != 0)
	cache[setNum][i].count++;
	}
	
}
*/


void extmissStackShift(profile_cache* pc, int setNum, int tag)
{
	int i;
	
	for (i = pc->dmax- 1; i >= 0; i--)
	{
		pc->extcache[setNum][i] = pc->extcache[setNum][i-1];
		
	
	}
	pc->extcache[setNum][0].tag = tag;
	pc->extcache[setNum][0].count = 1;
	
	
	

	
	for (i = 1; i < pc->dmax; i++){
	if (pc->extcache[setNum][i].tag != 0)
	pc->extcache[setNum][i].count++;
	}
	
}

/*
 * This function determines if a hit or miss has occurred
 */
 /*
int hitOrMissFunc(int setNum, int tag)
{
	int i;
	
	
	for (i = 0; i < nassoc; i++)
	{
		
		//hit
		if (cache[setNum][i].tag == tag)
		{
			
			hitPoint = i;
			hitOrMiss = 1;
			break;
		}
		else {  //miss
		hitOrMiss = 0;

		
		}
	
	}
	
	if (hitOrMiss == 1) {
		num_hits++;
		hitStackShift(setNum, hitPoint);
		
		
	}
	else {   
		num_misses++;
		missStackShift(setNum, tag);
		
	}
	
	return hitOrMiss;
}
*/
//this method is the entrance from cache.c
int exthitOrMissFunc(profile_cache* pc, md_addr_t addr)
{

	int i;
	int setNum=(addr>>log_base2(pc->nbsize))&(pc->nsets-1);
	int tag=addr>>(log_base2(pc->nbsize)+log_base2(pc->nsets));	
	for (i = 0; i < pc->dmax; i++)
	{
		//hit
		if (pc->extcache[setNum][i].tag == tag)
		{ //	printf("%d  ", i);
			pc->exthitPoint = i;
			pc->exthitOrMiss = 1;
			break;
		}
		else {  //miss
		pc->exthitOrMiss = 0;
		}
	
	}
	
	if (pc->exthitOrMiss == 1) {
		if(warmup<=0)
			pc->extnum_hits++;
		exthitStackShift(pc, setNum, pc->exthitPoint);
		
	}
	else {   
		if(warmup<=0)
			pc->extnum_misses++;
		extmissStackShift(pc, setNum, tag);
		
	}
	
	return pc->exthitOrMiss;
}




/**
  * This function prints the L2 cache stats
  */
void printCacheStats(profile_cache* pc)
{
	double miss_rate, extmiss_rate;
	int i,j;
	
	miss_rate = (double)(pc->num_misses)/(double)(pc->num_hits + pc->num_misses);
	extmiss_rate = (double)(pc->extnum_misses)/(double)(pc->extnum_hits + pc->extnum_misses);
	calcWeightedAvg(pc);
	
	char cseq_outfile_whole[256];
	strcpy(cseq_outfile_whole,pc->cseq_outfile_set);
	strcat(cseq_outfile_whole,"_whole.csq");
	pc->Fp = fopen(cseq_outfile_whole,"w");
	
	for (j = 0; j < pc->cseqrows; j++) {
		for (i = 0; i < pc->dmax; i++) {
		fprintf(pc->Fp, "%d ",pc->cseqtable[j][i]);
		}
		fprintf(pc->Fp, "\n");
	} 
	fclose(pc->Fp);
	
	//Print to screen
        /*
	printf("\n********** EXT NEW CACHE STATS (L2) ******** \n");
	printf("\nNumber of sets in cache: %d", nsets);
	printf("\nAssociativity of cache: %d", nassoc);
	printf("\nScaling factor: %d\n",SCALING_FACTOR);
	printf("\bBlock size: %d",nbsize);
	printf("\nNumber of misses: %d", extnum_misses);
	printf("\nNumber of hits: %d", extnum_hits);
	printf("\nNumber of cache accesses: %d", extnum_hits + extnum_misses);
	printf("\nCache miss rate: %f \n"  , extmiss_rate);

	printf("Cache contents \n");
	for (j = 0; j < 10; j++) {
		for (i = 0; i < nassoc; i++) {
		printf("%d:%d ",cache[j][i].tag,cache[j][i].count);
		}
		printf("\n");
	}
	
	printf("extCache contents \n");
	for (j = 0; j < 10; j++) {
		for (i = 0; i < dmax; i++) {
		printf("%d:%d ",extcache[j][i].tag,extcache[j][i].count);
		}
		printf("\n");
		}
	*/	
	//print cseq to file
	strcat(pc->cseq_outfile_set,".csq");
	pc->Fp = fopen(pc->cseq_outfile_set, "w");
	
	fprintf(pc->Fp,"#sets %d\n", pc->nsets);
	fprintf(pc->Fp,"#assoc %d\n", pc->nassoc);
	fprintf(pc->Fp,"#scaling_factor %d\n",SCALING_FACTOR);
	fprintf(pc->Fp,"#block_size %d\n",pc->nbsize);
	
	fprintf(pc->Fp,"\n#cseq\n");
	for (i = 0; i < pc->dmax; i++){
		fprintf(pc->Fp,"%.1f ",pc->weightedAvg[i]);
		
	}
	fprintf(pc->Fp,"\n\n#stackDist\n");
	for (i = 0; i < pc->dmax+1; i++){
		fprintf(pc->Fp,"%d ",pc->stackDist[i]);
		
	}
	fclose(pc->Fp);
if(trace_outfile!=NULL)
	fclose(TF);
	
}         
