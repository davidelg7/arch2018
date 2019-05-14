#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <xmmintrin.h>


float qs[2*8] = {  5.4, 5.3, 8, 6.2, 12, 8.7, 23, 33.4,
                  10, 11, 8.3, 5.5, 6.9, 12, 5, 3.5
                };


void writeDataset(char* filename) {
float ds[8*8] = {     1, 1, 2, 3, 2, 3, 3, 2,
                      4.6, 8.3, 3.5, 7.0, 1, 5.6, 5.1, 0,
                      32.2, 44, 10.5, 8.1, 2.4, 5.2, 8.4, 9.1,
                      3.7, 19.2, 11, 9.1, 71, 8.4, 8.3, 6.6,
                      4.1, 6.6, 4, 4.34, 6.6, 14, 8.5, 6.8,
                      7.3, 5.4, 3.1, 45.6, 4.6, 7.8, 3.6, 9.6,
                      1, 5.2, 5, 7.5, 27.4, 8.2, 4.5, 5.3,
                      5.3, 5.4, 5.4, 34.2, 7.3, 5.7, 9.6, 9.6
                     };
	FILE* fp;
	int i, j;
	char fpath[256];
  int dim=8*8;
	sprintf(fpath,"../ds/%s.ds", filename);
	fp = fopen(fpath, "w");
  int r=8;
  int c =8;
  fwrite( &c, 1, sizeof(int), fp ) ;
  fwrite( &r, 1, sizeof(int), fp ) ;
	for (i = 0; i < dim; i++) {
    fwrite( &ds[i], 1, sizeof(float), fp) ;
	}
	fclose(fp);
}
void writeQuery(char* filename) {
	FILE* fp;
	int i, j;
	char fpath[256];
  int dim=2*8;
	sprintf(fpath, "../ds/%s.qs", filename);
  fp = fopen(fpath, "w");
  int r=2;
  int c =8;
  fwrite( &c, 1, sizeof(int), fp ) ;
  fwrite( &r, 1, sizeof(int), fp ) ;
	for (i = 0; i < dim; i++) {
    fwrite( &qs[i], 1, sizeof(float), fp) ;
	}
	fclose(fp);
}
