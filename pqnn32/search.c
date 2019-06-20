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
void popolaANN_NES(MATRIX qs,MATRIX coarse,MAP mapCoarse,MATRIX centroidiPq,MAP mapPq,MATRIX residui,MAP ann,int n,int nq,int d,int m,int kc,int w,int nr,int k,int knn);
float calcDistMatrix(MATRIX centroids, MATRIX dis,int d, int m,int k);
extern float distanza(MATRIX m1, MATRIX m2, int x1, int x2, int k);
float dist(MATRIX m1, MATRIX m2, int x1, int x2, int k);
void* get_block(int size, int elements);
void free_block(void* p);
void stampaVettore(MATRIX f,int start,int stop);
void printMatrix(MATRIX m, int r, int c);
extern float dista2(MATRIX m1, MATRIX m2, int x1, int x2, int k);

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
			distanze[i]=distanza(centroids,qs,indiceInizioCentroide,indiceInizioQuery,dm);
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
void printCI(float* ci, int m,int w){
	for(int i=0;i<w;i++){
		for(int j=0;j<m;j++){
			printf("%d->%1.5f\t",(int)ci[(i*m+j)*2],ci[(i*m+j)*2+1]);
		}
		printf("\n");
	}
	printf("\n");

}
int* quantize2(MATRIX qs,MATRIX centroids,int d,int m,int k,int q,int w){
	//Per il punto query q trova i w centroidi più vicini di ogni gruppo
	//ogni riga di risultato memorizza un centroide per gruppo. Ci sono w righe e m colonne
	float* distanze=malloc(k*sizeof(float));
	int* risultato= malloc(m*w*sizeof(int));
	int dm= d/m;

	for(int j=0;j<m;j++){
		int indiceInizioQuery= q*d+j*dm;
		for(int i=0;i<k;i++){
			int indiceInizioCentroide=i*d+j*dm;
			distanze[i]=distanza(centroids,qs,indiceInizioCentroide,indiceInizioQuery,dm);
		}
		//ora in distanze ho tutte le distanze della query dal gruppo j
		//e aggiungo w elementi a risultato
		for(int z=0;z<w;z++){
			//mi trovo quello con la distanza più piccola
			int min=0;
			for(int s=0;s<k;s++)
				if(distanze[s]>0&&distanze[min]>distanze[s])
						min=s;
			risultato[j+z*m]=min;
			distanze[min]=-1;
			}
	}
	free(distanze);
	return risultato;
}
void stampaMappa(MAP map,int n, int m);

//Popola la struttura ANN inserendoci dentro le posizioni di knn approximated nn del dataset per ogni punto query in Queryset
void popolaANN_EA(MATRIX qs,MATRIX centroids,MAP ann,MAP map,int n, int nq, int d, int m, int k, int knn){
	int dm=d/m;
	// per ogni punto query
	for(int i=0;i<nq;i++){
		int* quantizzazione=quantize2(qs,centroids,d,m,k,i,1);

		int* id= get_block(sizeof(int),knn);
		float* distanze=get_block(sizeof(float),knn);
		for(int j=0;j<n;j++){

				float distanza=0;
				for(int g=0;g<m;g++){
					distanza=distanza+dista2(qs,centroids,(i*d)+(g*dm),(map[j*m+g]*d)+(g*dm),dm);
				}
				distanza=sqrtf(distanza);
		if(j<knn){
			id[j]=j;
			distanze[j]=distanza;
		}
		else{
			int posMax=0;
			for(int posizione=0;posizione<knn;posizione++)
				if(distanze[posizione]>distanze[posMax])
					posMax=posizione;
			//a questo punto ho la posizione del massimo
			if(distanze[posMax]>distanza){
				id[posMax]=j;
				distanze[posMax]=distanza;
			}
		}
		}
		for(int a =0;a<knn;a++)
				ann[i*knn+a]=id[a];

		free_block(quantizzazione);
		free_block(id);
		free_block(distanze);
	}
}
//Popola la struttura ANN inserendoci dentro le posizioni di knn approximated nn del dataset per ogni punto query in Queryset
void popolaANN_ES(MATRIX qs,MATRIX centroids,MAP ann,MAP map,MATRIX dis,int n, int nq,int d, int m, int k,int knn){
	int dm= d/m;
	//Per ogni punto del queryset
	for(int i=0;i<nq;i++){
		//quantizzo il punto i
		int*q=quantize2(qs,centroids,d,m,k,i,1);
		int* id= get_block(sizeof(int),knn);
		float* distanza= get_block(sizeof(float),knn);
		int p=0;
		for(int x=0;x<n;x++){
			float dx=0;
			for(int j=0;j<m;j++){
				//avanzo del gruppo
				int indice=j*k*k;
				//avanzo del centroide mappato da i
				indice+=q[j]*k;

				//avanzo del Centroide x
				indice+=map[x*m+j];

				//TODO togliere il per
				dx=dx+dis[indice];//*dis[indice];
		}
		dx=sqrtf(dx);

			//Se ancora non ne ho trovati knn,metto i primi che trovo
			if(p<knn){
				id[p]=x;
				distanza[p]=dx;
				p++;
			}
			else{
				int max=0;
				for(int l=0;l<knn;l++)
					if(distanza[l]>distanza[max])
						max=l;
				//A questo punto ho la posizione del massimo
				//posso sostituire il massimo con il corrente dx se la distanza dx è più piccola
				if(distanza[max]>dx){
					id[max]=x;
					distanza[max]=dx;}
			}

		}
		// Arrivato qui ho controllato tutti i punti e ho in id-distanza la lista (id) dei knn più vicini a y
		// e li salvo in ANN
		for(int s=0;s<knn;s++){
			  int min=0;
			 	for(int l=0;l<knn;l++)
			  	if(id[l]!=-1){
			  		min=l;
					break;}
				for(int l=0;l<knn;l++)
					  if(distanza[l]>0&&distanza[min]>distanza[l])
							min=l;
			ann[i*knn+s]=id[min];
		  id[min]=-1;
			distanza[min]=-1;
		}
		free(q);
		free_block(id);
		free_block(distanza);
	}
}

void popolaANN_NES(MATRIX qs,MATRIX coarse,MAP mapCoarse,MATRIX centroidiPq,MAP mapPq,MATRIX residui,MAP ann,int n,int nq,int d,int m,int kc,int w,int nr,int k,int knn){
	MATRIX dis=get_block(sizeof(float),m*k*k);
	calcDistMatrix(centroidiPq,dis,d,m,k);
	for(int i=0;i<nq;i++){
		int* id=get_block(sizeof(int),knn);
		for(int p=0;p<knn;p++)
			id[p]=-1;
		float* distanz=get_block(sizeof(float),knn);
		// 1 numero di gruppi del quantizatore coarse
		int*q= quantize2(qs,coarse,d,1,kc,i,w);
		int trovati=0;
		for(int j=0;j<w;j++){
			float* df2= get_block(sizeof(float),d);
			for(int c=0;c<d;c++)
				df2[c]=coarse[q[j]+c]-qs[i*d+c];
			int* ql=quantize2(df2,centroidiPq,d,m,k,0,1);
			 for(int p=0;p<nr;p++){
				 if(mapCoarse[p]==q[j]){
					 float dist=0;
					 for(int g=0;g<m;g++)
					 		 dist+=dis[g*k*k+ql[g]*k+mapPq[p*m+g]];
						dist=sqrtf(dist);
	 					 if(trovati<knn){
	 						 id[trovati]=p;
	 						 distanz[trovati]=dist;
	 						 trovati++;
	 					 }
	 					 else{
	 						 int posMax=0;
	 						 for(int max=0;max<knn;max++)
	 							 if(distanz[posMax]>distanz[max])
	 								 posMax=max;
	 						 if(distanz[posMax]>dist){
	 						 id[posMax]=p;
	 						 distanz[posMax]=dist;
	 					 }
	 				 }
				 }
			 }
			 for(int p=0;p<knn;p++)
			 ann[i*knn+p]=id[p];
			 free(ql);
			 free_block(df2);
		 }

		free(q);
		free_block(id);
		free_block(distanz);

	}
	free_block(dis);
}

void popolaANN_NEA(MATRIX qs,MATRIX coarse,MAP mapCoarse,MATRIX centroidiPq,MAP mapPq,MATRIX residui,MAP ann,int n,int nq,int d,int m,int kc,int w,int nr,int k,int knn){
	for(int i=0;i<nq;i++){
		int* id=get_block(sizeof(int),knn);
		for(int p=0;p<knn;p++)
			id[p]=-1;
		float* distanz=get_block(sizeof(float),knn);
		// 1 numero di gruppi del quantizatore coarse
		int*q= quantize2(qs,coarse,d,1,kc,i,w);
		int trovati=0;
		for(int j=0;j<w;j++){
			float* df2= get_block(sizeof(float),d);
			for(int c=0;c<d;c++)
				df2[c]=coarse[q[j]+c]-qs[i*d+c];

				MATRIX dis2=get_block(sizeof(float),k*m);
				for(int j=0;j<m;j++){
		 			 int i1=j*d/m;
		 			 for(int q=0;q<k;q++){
		 				 int i2=q*d+j*d/m;
		 			 		dis2[j*k+q]=dista2(df2,centroidiPq,i1,i2,d/m);
		 		 }
		 	 }
			 for(int p=0;p<nr;p++){
				 if(mapCoarse[p]==q[j]){
					 float dist=0;
					 for(int g=0;g<m;g++)
					 		 dist+=dis2[g*k+mapPq[p*m+g]];
						dist=sqrtf(dist);
	 					 if(trovati<knn){
	 						 id[trovati]=p;
	 						 distanz[trovati]=dist;
	 						 trovati++;
	 					 }
	 					 else{
	 						 int posMax=0;
	 						 for(int max=0;max<knn;max++)
	 							 if(distanz[posMax]>distanz[max])
	 								 posMax=max;
	 						 if(distanz[posMax]>dist){
	 						 id[posMax]=p;
	 						 distanz[posMax]=dist;
	 					 }
	 				 }
				 }
			 }
			 for(int p=0;p<knn;p++)
			 ann[i*knn+p]=id[p];
			 free_block(dis2);
			 free_block(df2);
		 }

		free(q);
		free_block(id);
		free_block(distanz);
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
			 		dis[j*k*k+i*k+q]=dista2(centroids,centroids,i1,i2,d/m);
			 }
		 }
	 }
}
