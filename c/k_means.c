#include <stdio.h>
#include <stdlib.h>
#define	MATRIX		float*
#define	VECTOR		double*

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
	int* ANN;
	MATRIX quant;
	//
	// Inserire qui i campi necessari a memorizzare i Quantizzatori
	//
	// ...
	// ...
	// ...
	//
} params;
float* mediaG(params* input,int group);
float dist(params* input, int group,int x1,int x2);
void sub_k_means(params* input,int group);
//calcola i k centroidi attraverso  k_means
//una chiamata a sub_k_means li calcola per ogni gruppo 0<=i<=m
void k_means(params* input){
	// dimensione matrice quant:
	// k:numero di centroidi per gruppo
	//m:numero di gruppi
	//d:numero di dimensioni di un punto del dataset
	// punti necessari: k*m*d/m-> k*d
	input->quant=malloc(input->k*input->d*sizeof(float));
	for(int i=0;i<input->m;i++){
			sub_k_means(input,i);
	}
	for(int i=0;i<1;i++){
		for(int j=0;j<input->k;j++){
			for(int m=0;m<(input->d/input->m);m++){
			printf("%1.1f ",input->quant[i*input->k+j+m]);
		}
			printf("\n");

		}
		printf("\n ");

}
}
//calcola i k centroidi relativi all'i-esimo gruppo
void sub_k_means(params* input,int group){
	//selezione dei numeri casuali
	for(int i=0;i<input->k;i++){
		int rnd=rand() % input->n;
		input->quant[group*input->k+i]=input->ds[rnd];
	}




}
