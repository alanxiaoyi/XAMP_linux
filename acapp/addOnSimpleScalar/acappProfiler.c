/**
  * Please refer to License.txt for License information.
 */

#include "acappProfiler.h"
#include <stdio.h>

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

/**
  * This function initialize the cache and sets the size according 
  * to parameters numsets and nummasoc.
  */
void initcache(int numsets, int numassoc, int bsize)
{
  int i,j;
  dmax = SCALING_FACTOR * numassoc; //size of columns in cseqtable & assoc of pseudocache
  printf("Initializing Cache\n");
  
  hitOrMiss = 0; 
  num_hits = 0;
  num_misses = 0;
  hitPoint = 0;

  exthitOrMiss = 0; 
  extnum_hits = 0;
  extnum_misses = 0;
  exthitPoint = 0;
  	  
  nsets = numsets;	
  nassoc = numassoc;	
  nbsize = bsize;
  cseqrows = 200;
  

  extcache = malloc(nsets * sizeof(matrix));
  cseqtable = malloc(cseqrows * sizeof(int*));			//yw: modify to int*
  cache = malloc(nsets * sizeof(matrix));
  weightedAvg = malloc(dmax * sizeof(double));
  stackDist = malloc((dmax+1) * sizeof(int));
  for ( i=0; i < nsets; i++ ) {
    cache[i] = malloc(nassoc * sizeof(matrix));
  }
    
    for ( i=0; i < nsets; i++ ) {
    for ( j=0; j < nassoc; j++ ) {
    	cache[i][j].tag = 0;
	cache[i][j].count = 0;

  
   	}
   }
  
  for ( i=0; i < cseqrows; i++ ) {
    cseqtable[i] = malloc(dmax * sizeof(int));   // yw: modify to dmax
  }
	
	for (j = 0; j < cseqrows; j++) {		//yw: add init
		for (i = 0; i < dmax; i++) {
			cseqtable[j][i]=0;
		}
	}
	
	
  for ( i=0; i < nsets; i++ ) {
    extcache[i] = malloc(dmax * sizeof(matrix));
  }  
  

   
    for ( i=0; i < nsets; i++ ) {
    for ( j=0; j < dmax; j++ ) {
    	extcache[i][j].tag = 0;
	extcache[i][j].count = 0;

  
   	}
   }
  printf("Cache Initialization done \n");
  
}

/** Test function that prints the contents of a particular set in the cache
 */
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
		

}

void extprintset(int set)
{

	int i;
	
	printf("\n");
	for (i = 0; i < dmax; i++) {
		printf("%d ",extcache[set][i].tag);
		
		
	}
	printf("\n");
	for (i = 0; i < dmax; i++) {
	printf("%d ",extcache[set][i].count);
	}	
	printf("\n");
		

}

void calcWeightedAvg(){
	int i,n,rSum,weightedSum;
	
	
	for (i = 0; i < dmax; i++){
		
		rSum = 0;
		weightedSum = 0;
		for (n = i+1; n < cseqrows; n++) {		//yw: modify n=0 to n=i+1 since n>=d+1
		weightedSum+= (n+1)*cseqtable[n][i];
		rSum += cseqtable[n][i];
		
		}
		stackDist[i] = rSum; //sum of all values in each d
		if (rSum != 0)		
		weightedAvg[i] = (double)((double)weightedSum/(double)rSum);
		
		else weightedAvg[i] = 0;
		
		
		
	}

stackDist[dmax] = extnum_misses;
}

/*
 * This function shifs the stack accordingly when a hit occurs - LRU
 */

/*
 * This function shifs the stack accordingly when a hit occurs - LRU
 */
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
	
}

void exthitStackShift(int setNum, int exthitPoint)
{

	matrix temp;
	int i;
	
	extcache[setNum][exthitPoint].count++;
	temp = extcache[setNum][exthitPoint];
	
	if (exthitPoint != 0) 
	{
	
		if (exthitPoint == dmax - 1) //if hit in last assoc # o0f extcache
		{
			for (i = dmax - 1; i > 0; i--)
			{
			
				extcache[setNum][i] = extcache[setNum][i-1];
				
			}
			extcache[setNum][0] = temp;
		}
		else 
		{
			for (i = exthitPoint; i > 0; i--)
			{
				extcache[setNum][i] = extcache[setNum][i-1];
				
			}
			extcache[setNum][0] = temp;
			
		}
	}
	extcache[setNum][0].count = 1;
	
	for (i = 1; i < dmax; i++)
		if (extcache[setNum][i].tag != 0)
		extcache[setNum][i].count++;	
	
	int n = temp.count;
	if(n>cseqrows) n=cseqrows;				//yw: add a check
	
	cseqtable[n-1][exthitPoint]++;
//  printf("%d",n);
//	printf("%d, %d\n", n, exthitPoint);
	
}

/*
 * This function shifts the stack accordingly when a miss occurs - LRU
 */
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



void extmissStackShift(int setNum, int tag)
{
	int i;
	
	for (i = dmax- 1; i >= 0; i--)
	{
		extcache[setNum][i] = extcache[setNum][i-1];
		
	
	}
	extcache[setNum][0].tag = tag;
	extcache[setNum][0].count = 1;
	
	
	

	
	for (i = 1; i < dmax; i++){
	if (extcache[setNum][i].tag != 0)
	extcache[setNum][i].count++;
	}
	
}

/*
 * This function determines if a hit or miss has occurred
 */
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

//this method is the entrance from cache.c
int exthitOrMissFunc(int setNum, int tag)
{
	int i;
	
	
	for (i = 0; i < dmax; i++)
	{
		//hit
		if (extcache[setNum][i].tag == tag)
		{ //	printf("%d  ", i);
			exthitPoint = i;
			exthitOrMiss = 1;
			break;
		}
		else {  //miss
		exthitOrMiss = 0;
		}
	
	}
	
	if (exthitOrMiss == 1) {
		extnum_hits++;
		exthitStackShift(setNum, exthitPoint);
		
	}
	else {   
		extnum_misses++;
		extmissStackShift(setNum, tag);
		
	}
	
	return exthitOrMiss;
}




/**
  * This function prints the L2 cache stats
  */
void printCacheStats()
{
	double miss_rate, extmiss_rate;
	int i,j;
	
	miss_rate = (double)(num_misses)/(double)(num_hits + num_misses);
	extmiss_rate = (double)(extnum_misses)/(double)(extnum_hits + extnum_misses);
	calcWeightedAvg();
	
	char cseq_outfile_whole[256];
	strcpy(cseq_outfile_whole,cseq_outfile);
	strcat(cseq_outfile_whole,"_whole.csq");
	Fp = fopen(cseq_outfile_whole,"w");
	
	for (j = 0; j < cseqrows; j++) {
		for (i = 0; i < dmax; i++) {
		fprintf(Fp, "%d ",cseqtable[j][i]);
		}
		fprintf(Fp, "\n");
	} 
	fclose(Fp);
	
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
	strcat(cseq_outfile,".csq");
	Fp = fopen(cseq_outfile, "w");
	
	fprintf(Fp,"#sets %d\n", nsets);
	fprintf(Fp,"#assoc %d\n", nassoc);
	fprintf(Fp,"#scaling_factor %d\n",SCALING_FACTOR);
	fprintf(Fp,"#block_size %d\n",nbsize);
	
	fprintf(Fp,"\n#cseq\n");
	for (i = 0; i < dmax; i++){
		fprintf(Fp,"%.1f ",weightedAvg[i]);
		
	}
	fprintf(Fp,"\n\n#stackDist\n");
	for (i = 0; i < dmax+1; i++){
		fprintf(Fp,"%d ",stackDist[i]);
		
	}
	fclose(Fp);
	
}         
