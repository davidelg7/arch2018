#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "time.h"
#define	MATRIX		float*
#define	VECTOR		int*
#define MAP 			int*

typedef struct {
	char* filename; //
	MATRIX ds; // data set
	MATRIX qs; // query set
	int n; // numero di punti del data set
	int d; // numero di dimensioni del data/query set
	int nq; // numero di punti del query set
	int knn; // numero di ANN approssimati da restituire per ogni query
	int m; // numero di gruppi del quantizzatore prodotto
	int k; // numero di centroidi di ogni sotto-quantizzatore
	int kc; // numero di centroidi del quantizzatore coarse
	int w; // numero di centroidi del quantizzatore coarse da selezionare per la ricerca non esaustiva
	int nr; // dimensione del campione dei residui nel caso di ricerca non esaustiva
	float eps; //
	int tmin; //
	int tmax; //
	int exaustive; // tipo di ricerca: (0=)non esaustiva o (1=)esaustiva
	int symmetric; // tipo di distanza: (0=)asimmetrica ADC o (1=)simmetrica SDC
	int silent;
	int display;
	// nns: matrice row major order di interi a 32 bit utilizzata per memorizzare gli ANN
	// sulla riga i-esima si trovano gli ID (a partire da 0) degli ANN della query i-esima
	//
	MAP ANN;
	MATRIX quant;
	MAP map;
	MATRIX dis;
	MATRIX res;

  MATRIX map2;
  MATRIX quant2;

	MATRIX ds2;

	MATRIX quant3;//contiene i quantizzatori prodotto della ricerca non esaustiva
	MAP map3;//contiene la mappa [residuo->centroide in quant3]
	//
	// Inserire qui i campi necessari a memorizzare i Quantizzatori
	//
	// ...
	// ...
	// ...
	//

} params;



void k_means(MATRIX ds, MATRIX centroids, MAP map, int n, int d, int m, int k, int tmin,int tmax, float eps);

void coarse(MATRIX ds, MATRIX centroids, MAP map, int n, int d, int k, int tmin,int tmax, float eps){
  k_means(ds, centroids, map, n, d, 1, k, tmin, tmax, eps);
}
extern void diff2(MATRIX res, MATRIX ds, MATRIX quant2, int i1, int i2, int d);
void diff(MATRIX res, MATRIX ds, MATRIX quant2, int i1, int i2, int d){
  for(int j = 0; j < d; j++)
    res[i1+j]=ds[i1+j]-quant2[i2+j];
}

void popolaRes(MATRIX ds,MAP map2, MATRIX res, MATRIX quant2, int n , int d, int kc){
  for(int i = 0; i < n; i++){
    int i1 = i*d;
    int quant = map2[i];
    int i2 = quant*d;
    diff(res, ds, quant2, i1, i2, d);
  }
}
void popolaDs2(MATRIX res,MATRIX ds2,MAP map,int nr,int d){
	for(int i=0;i<nr;i++)
		for(int j=0;j<d;j++)
			ds2[i*d+j]=res[i*d+j];
}
