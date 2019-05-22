#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <xmmintrin.h>


float qs[2*1] = {  4,4
                };
float ds[2*6]={ 1,1,
                2,1,
                1,3,
                4,1,
                5,3,
                7,4};

void writeRandomDataset(char* filename) {
printf("Scrivo il dataset\n");
	FILE* fp;
	int i, j;
	char fpath[256];

	sprintf(fpath,"/media/starquake/D/arch2018/ds/%s.ds", filename);
	fp = fopen(fpath, "w+");
  int r=40;
  int c =2;
  int dim=r*c;
  float* ds = malloc(dim*sizeof(float));
  srand(time(NULL));
  for (i = 0; i <10; i++)
    ds[i]=rand() % 100;

  for (i = 10; i < dim; i++)
    if(i<dim/2)
    ds[i]=rand() % 40+50;
    else
    ds[i]=rand()%40;

  fwrite( &c, 1, sizeof(int), fp ) ;
  fwrite( &r, 1, sizeof(int), fp ) ;
	for (i = 0; i < dim; i++) {
    fwrite( &ds[i], 1, sizeof(float), fp) ;
	}
	fclose(fp);
}
void writeDataset(char* filename) {
printf("Scrivo il dataset\n");
	FILE* fp;
	int i, j;
	char fpath[256];

	sprintf(fpath,"../ds/%s.ds", filename);
	fp = fopen(fpath, "w+");
  int r=6;
  int c =2;
  int dim=r*c;
  fwrite( &c, 1, sizeof(int), fp ) ;
  fwrite( &r, 1, sizeof(int), fp ) ;
	for (i = 0; i < dim; i++) {
    fwrite( &ds[i], 1, sizeof(float), fp) ;
	}
	fclose(fp);
}
void writeQuery(char* filename) {
  printf("Scrivo il queryset\n");

	FILE* fp;
	int i, j;
	char fpath[256];
	sprintf(fpath, "../ds/%s.qs", filename);
  fp = fopen(fpath, "w");
  int r=1;
  int c =2;
  int dim=r*c;

  fwrite( &c, 1, sizeof(int), fp ) ;
  fwrite( &r, 1, sizeof(int), fp ) ;
	for (i = 0; i < dim; i++) {
    fwrite( &qs[i], 1, sizeof(float), fp) ;
	}
	fclose(fp);
}
