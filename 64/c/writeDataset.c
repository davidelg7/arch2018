#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <xmmintrin.h>

#define	MATRIX		float*
#define	VECTOR		int*
#define MAP 			int*

float qs[2*4] = {6.5,5.5,1.5,9.5,9,2.5,1,2};
float ds[2*20]={
	1,1,
	1,3,
	2,4,
	4,3,
	4,5,
	3,6,
	1,7,
	7,5,
	8,6,
	9,5,
	7,8,
	4,9,
	2,9,
	1,10,
	6,6
	,7,3,
	9,3
	,9,2,
	7,1,
	6,4};

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
  int r=20;
  int c =2;
  int dim=r*c;
  fwrite( &c, 1, sizeof(int), fp ) ;
  fwrite( &r, 1, sizeof(int), fp ) ;
	for (i = 0; i < dim; i++) {
    fwrite( &ds[i], 1, sizeof(float), fp) ;
	}
	fclose(fp);
}
void writeCentroid(MATRIX qs,MATRIX quant,MAP map,int n,int m,int d,int k,int group, int passo) {

	for(int j=0;j<k;j++){
		FILE* fp;
		char buf[256];
		snprintf(buf, sizeof buf, "../csv2/p %d c %d g %d.csv",passo,j,group);
		fp = fopen(buf, "w+");
		fprintf(fp,"%s,","x");
		fprintf(fp,"%s,\n","y");

			for(int i=0;i<d/m;i++)
				fprintf(fp,"%f,",quant[j*d+group*d/m+i]);

			fprintf(fp,"\n");
			for(int p=0;p<n;p++){
				if(map[p*m+group]==j){
					for(int i=0;i<(d/m);i++){
						fprintf(fp,"%f,",qs[p*d+group*d/m+i]);
					}
					fprintf(fp,"\n");

				}
			}
			fclose(fp);

		}
}
void writeANN(MATRIX qs,MAP ann,MATRIX ds,int nq,int d) {
  FILE* fp;
  char buf[256];
  snprintf(buf, sizeof buf, "../csv2/ANN.csv");
  fp = fopen(buf, "w+");
  fprintf(fp,"%s,","xq");
  fprintf(fp,"%s,","yq");
  fprintf(fp,"%s,","xd");
  fprintf(fp,"%s\n","yd");
	for(int j=0;j<nq;j++){
      fprintf(fp,"%f,%f,%f,%f\n",qs[j*d],qs[j*d+1],ds[ann[j]*d],ds[ann[j]*d+1]);
		}
    fclose(fp);
		snprintf(buf, sizeof buf, "../csv2/DS.csv");
		fp = fopen(buf, "w+");
		fprintf(fp,"%s,","x");
		fprintf(fp,"%s,","y");

		for(int j=0;j<20;j++){
				fprintf(fp,"%f,%f\n",ds[j],ds[j+1]);
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
  int r=4;
  int c =2;
  int dim=r*c;

  fwrite( &c, 1, sizeof(int), fp ) ;
  fwrite( &r, 1, sizeof(int), fp ) ;
	for (i = 0; i < dim; i++) {
    fwrite( &qs[i], 1, sizeof(float), fp) ;
	}
	fclose(fp);
}
