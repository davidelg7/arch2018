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
	//
	// Inserire qui i campi necessari a memorizzare i Quantizzatori
	//
	// ...
	// ...
	// ...
	//
} params;
void popolaANN_ES(MATRIX qs,MATRIX centroids,MAP ann,MAP map,MATRIX dis,int n, int nq,int d, int m, int k,int knn);
void popolaANN_EA(MATRIX qs,MATRIX centroids,MAP ann,MAP map,int n, int nq,int d, int m, int k,int knn);
float calcDistMatrix(MATRIX centroids, MATRIX dis,int d, int m,int k);
extern float dista(MATRIX m1, MATRIX m2, int x1, int x2, int k);
float dist(MATRIX m1, MATRIX m2, int x1, int x2, int k);


void popolaANN(MATRIX qs,MATRIX centroids,MAP ann,MAP map,MATRIX dis,int n, int nq,int d, int m, int k,int knn, int exaustive, int symmetric){
	if(exaustive==1&&symmetric==1){
    calcDistMatrix(centroids,dis,d,m,k);
		popolaANN_ES(qs,centroids,ann,map,dis,n, nq,d, m,  k,knn);
	}
	else
	if(exaustive==1&&symmetric==0){
 	popolaANN_EA(qs,centroids,ann,map,n,nq,d,m,k,knn);
	}

}

int* quantize(MATRIX qs,MATRIX centroids,int d,int m,int k,int q){
	float* distanze=malloc(k*sizeof(float));
	int* risultato= malloc(m*sizeof(int));
	int dm= d/m;
	for(int j=0;j<m;j++){
		int indiceInizioQuery= q*d+j*dm;
		for(int i=0;i<k;i++){
			int indiceInizioCentroide=i*d+j*dm;
			distanze[i]=dist(centroids,qs,indiceInizioCentroide,indiceInizioQuery,dm);
		}
		int min=0;
		for(int z=0;z<k;z++)
			if(distanze[min]>distanze[z])
				min=z;
		risultato[j]=min;
	}
	free(distanze);
	return risultato;
}
//Popola la struttura ANN inserendoci dentro le posizioni di knn approximated nn del dataset per ogni punto query in Queryset
void popolaANN_EA(MATRIX qs,MATRIX centroids,MAP ann,MAP map,int n, int nq,int d, int m, int k,int knn){
	int dm= d/m;
	//Per ogni punto del queryset
	for(int i=0;i<nq;i++){
		float*vicini= malloc(knn*2*sizeof(float)); //(id,dist)
		int p=0;
		for(int x=0;x<n;x++){
			float dx=0;
			for(int j=0;j<m;j++){
				int centroideX=map[x*m+j]*d+m*dm;
				dx=dx+dist(centroids,qs,centroideX,i*d+j*dm,dm);
		}
			//Se ancora non ne ho trovati knn,metto i primi che trovo
			if(p<knn*2){
				vicini[p]=x;
				vicini[p+1]=dx;
				p+=2;
			}
			else{
				int max=0;
				for(int l=0;l<knn*2;l+=2)
					if(vicini[l+1]>vicini[max+1])
						max=l;
				//A questo punto ho la posizione del massimo
				//posso sostituire il massimo con il corrente dx se la distanza dx è più piccola
				if(vicini[max+1]>dx){
					vicini[max]=x;
					vicini[max+1]=dx;}
			}
		}
		// Arrivato qui ho controllato tutti i punti e ho in vicini la lista (id,dist) dei knn più vicini a y
		// e li salvo in ANN
		for(int s=0;s<knn;s++){
			ann[i*knn+s]=(int)vicini[s*2];
		}
		free(vicini);
	}
}
//Popola la struttura ANN inserendoci dentro le posizioni di knn approximated nn del dataset per ogni punto query in Queryset
void popolaANN_ES(MATRIX qs,MATRIX centroids,MAP ann,MAP map,MATRIX dis,int n, int nq,int d, int m, int k,int knn){
	int dm= d/m;
	//Per ogni punto del queryset
	for(int i=0;i<nq;i++){
		int*q=quantize(qs,centroids,d,m,k,i);
		float*vicini= malloc(knn*2*sizeof(float)); //(id,dist)
		int p=0;
		for(int x=0;x<n;x++){
			float dx=0;
			for(int j=0;j<m;j++){
				//avanzo del gruppo
				int indice=j*k*k;
				// printf("%d\n",indice);
				//avanzo del Centroide x
				indice+=map[x*m+j]*k;

				//avanzo del centroide mappato da i
				indice+=q[j];
				dx=dx+dis[indice]*dis[indice];

		}
			//Se ancora non ne ho trovati knn,metto i primi che trovo
			if(p<knn*2){
				vicini[p]=x;
				vicini[p+1]=dx;
				p+=2;
			}
			else{
				int max=0;
				for(int l=0;l<knn*2;l+=2)
					if(vicini[l+1]>vicini[max+1])
						max=l;
				//A questo punto ho la posizione del massimo
				//posso sostituire il massimo con il corrente dx se la distanza dx è più piccola
				if(vicini[max+1]>dx){
					vicini[max]=x;
					vicini[max+1]=dx;}
			}

		}
		// Arrivato qui ho controllato tutti i punti e ho in vicini la lista (id,dist) dei knn più vicini a y
		// e li salvo in ANN
		for(int s=0;s<knn;s++){
			ann[i*knn+s]=(int)vicini[s*2];
		}
		free(q);
		free(vicini);
	}

}
void stampaVettore(MATRIX f,int start,int stop);
float calcDistMatrix(MATRIX centroids, MATRIX dis,int d, int m,int k){
	 //per ogni gruppo
	 for(int j=0;j<m;j++){
		 //per ogni quantizzatore
		 for(int i=0;i<k;i++){
			 //Calcola la distanza i centroidi[i,q] del gruppo j e salvala in m
			 int i1=i*d+j*d/m;
			 for(int q=0;q<k;q++){
				 int i2=q*d+j*d/m;
				// printf("%d %d %d\n",(i1%16+i1)%16,i1%16,i1);
				// float q1[9] = {3,2, 0, 1,7,4, 3,2,6};
				// float q2[9] = {1,1,3,0,5,6,6,6,4};
				// stampaVettore(q1,1,9);
				// stampaVettore(q2,1,9);
				//  printf("SBAGLIATA %f\n",dista(q1,q2,1,1,9));
				//  printf("GUSTA %f\n\n",dist(q1,q2,1,1,9));
			 		dis[j*k*k+i*k+q]=dista(centroids,centroids,i1,i2,d/m);
			 }
		 }
	 }
}
