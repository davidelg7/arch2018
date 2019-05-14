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
	int* ANN;
	MATRIX quant;
	MAP map;
	//
	// Inserire qui i campi necessari a memorizzare i Quantizzatori
	//
	// ...
	// ...
	// ...
	//
} params;
//group GRUPPO
//c Centroide
//v componente del centroide
int getCentroidIndex(params* input,int group,int c,int v){
	return c*input->d+input->d/input->m*group+v;
}
//gruppo group
//i riga
//v componente della riga
int getDatasetIndex(params* input,int group,int i,int v){
	return input->d*i+group*input->d/input->m+v;
}
int getMapIndex(params*input,int group,int i){
	return i*input->m+group;
}
//calola la distanza geometrica tra il punto x1 e x2
//x1 e x2 devono avere la stessa dimensione k
//x1 appartiene a m1 e x2 appartiene a m2
float dist(MATRIX m1, MATRIX m2, int x1, int x2, int k){
	float d=0;
		for(int i=0;i<k;i++)
			d+=(m1[x1+i]-m2[x2+i])*(m1[x1+i]-m2[x2+i]);
	d=sqrtf(d);
	return d;
}
void stampaMappa(params* input){
	MAP map=input->map;
	for(int i=0;i<input->n;i++){
		for(int j=0;j<input->m;j++){
			// printf("[");
			// for(int k=0;k<input->d/input->m;k++)
			// 	printf("%1.1f ,",input->ds[getDatasetIndex(input,j,i,k)]);
			// 	printf("]");
			printf("[%d,%d]->%d  ",i,j,map[getMapIndex(input,j,i)]);
	}
		printf("\n");
	}
}
void sub_k_means(params* input,int group);
void select_random_centroid(params* input);
void printCentroids(params*input, int group){
	for(int i=0;i<input->k;i++){
		printf("%d-> ",i);
		for(int j=0;j<input->d/input->m;j++)
			printf("%1.1f  ",input->quant[getCentroidIndex(input,group,i,j)]);
		printf("\n" );
	}
}
void updateNN(params* input,int group){
		MATRIX ds= input->ds;
		MATRIX centroidi= input->quant;
		MAP map= input->map;

		float* dis= malloc(input->k*sizeof(float));
		for(int i=0;i<input->k;i++)
			dis[i]=0;
		//per ogni punto del dataset
		for(int i =0;i<input->n;i++){

			//per ogni cetroide del gruppo group
			//calcola le distanze e memorizzale in dist

			int min=0;

			for(int j=0;j<input->k;j++){
				dis[j]=dist(ds,centroidi,i*input->d+group*input->d/input->m,group*input->d/input->m+j*input->d,input->d/input->m);
				if(dis[j]<=dis[min])
					min=j;
			}
			map[getMapIndex(input,group,i)]=min;
		}
		free(dis);
}
float calcolaDifferenza(params* input,int group,float* newCentroid){
		float diff=0;
		float tot=0;
		MATRIX q=input->quant;
		int d=input->d;
		int m=input->m;
		//per ogni centroide
		for(int i=0;i<input->k;i++){
			diff=0;
			//per ogni componente del centroide i
			for(int j=0;j<input->d/input->m;j++){
				//eleva al quadrato la differenza delle componenti
				diff+=(q[i*d+group*d/m+j]-newCentroid[i*d/m+j])*(q[i*d+group*d/m+j]-newCentroid[i*d/m+j]);
			}
			tot+=sqrtf(diff);
		}
		return tot;
}
void stampaVettore(MATRIX f ,int step,int dim){
	printf("VETTORE DI DIMENSIONE %d CON CARDINALITA %d\n",dim,step);
	for(int i=0;i<dim;){
		printf("%d   ",(int)(i/step));
		for(int j=0;j<step;i++,j++)
			printf("%1.1f  ",f[i]);
		printf("\n");
		}
}

void updateCentroids(params* input,int group,float* newCentroids){
		// printf("CENTROIDI DEL GRUPPO %d PRIMA DEL CAMBIAMENTO\n",group );
		// printCentroids(input,group);
		// printf("\nCAMBIAMENTO DA APPORTARE \n");
		// for(int i=0;i<input->k;i++){
		// 	for(int j=0;j<input->d/input->m;j++)
		// 		printf("%1.1f ",newCentroids[i*input->d/input->m+j]);
		// 	printf("\n" );
		// }
		for(int i=0;i<input->k;i++){
			for(int j=0;j<input->d/input->m;j++)
					input->quant[getCentroidIndex(input,group,i,j)]=newCentroids[i*input->d/input->m+j];
		}
		// printf("\nCENTROIDI DEL GRUPPO %d DOPO LA MODIFICA\n",group);
		// printCentroids(input,group);
		// printf("\n");

}


//calcola la media geometrica dei punti mappati dai centroidi di un gruppo
float* mediaGeometrica(params* input,int group){
	//contiene blocchi k blocchi di d/m elementi. ogni blocco rappresenta la media mediaGeometrica
	//dei punti mappati dal centroide k
 	float* media= malloc((input->k*(input->d/input->m))*sizeof(float));

	int*occ=malloc(input->k*sizeof(int));

	for(int i =0;i<input->k;i++)
		occ[i]=0;
	for(int i=0;i<input->k;i++)
	for(int j=0;j<input->d/input->m;j++)
		media[i*input->d/input->m+j]=input->quant[getCentroidIndex(input,group,i,j)];
		//per ogni riga del dataset
	for(int i=0;i<input->n;i++){
		//aumenta il numero di righe che il centroide i del gruppo group mappa
		occ[input->map[getMapIndex(input,group,i)]]++;
		for(int j=0;j<input->d/input->m;j++){
			media[input->map[getMapIndex(input,group,i)]*input->d/input->m+j]+=input->ds[getDatasetIndex(input,group,i,j)];
 }
}
 for(int i=0;i<input->k*input->d/input->m;i++){
	
 media[i]/=input->n;}
 free(occ);
return media;
}

void stampaCentroidi(params* input){
	for(int i=0;i<input->m;i++){
		printf("------------------------		GRUPPO		<%1d>		------------------------\n",i);
		for(int j=0;j<input->k;j++){
			printf("%1d--  ",j);
			for(int m=0;m<(input->d/input->m);m++){
				printf("%1.1f ",input->quant[getCentroidIndex(input,i,j,m)]);
			}
			printf("\n");
		}
		printf("\n");
	}


}
void stampaQuantiMappatiPerOgniCentroide(params* input){
	MAP map= input->map;
	// stampaMappa(input);
	int* occ= malloc(input->k*input->m*sizeof(int));
	for(int i=0;i<input->k*input->m;i++)
		occ[i]=0;
	for(int i =0;i< input->n;i++)
		for(int j=0;j<input->m;j++)
				occ[j*input->k+map[i*input->m+j]]++;
		for(int i =0;i< input->k;i++){
			for(int j=0;j<input->m;j++)
				printf("[c:%d, g:%d, m:%d] \t",i,j,occ[j*input->m+i]);
				printf("\n" );
			}
		free(occ);
	}
//calcola i k centroidi attraverso  k_means
//una chiamata a sub_k_means li calcola per ogni gruppo 0<=i<=m
void k_means(params* input){
	// dimensione matrice quant:
	// k:numero di centroidi per gruppo
	//m:numero di gruppi
	//d:numero di dimensioni di un punto del dataset
	// punti necessari: k*m*d/m-> k*d
	input->quant=malloc(input->k*input->d*sizeof(float));
	input->map=malloc(input->n*input->m*sizeof(int));
	select_random_centroid(input);
	//stampaCentroidi(input);
	// stampaMappa(input);

//	stampaCentroidi(input);
	for(int i=0;i<input->m;i++)
			sub_k_means(input,i);
	 stampaCentroidi(input);
	 stampaMappa(input);
	 stampaQuantiMappatiPerOgniCentroide(input);
	}
//calcola i k centroidi casuali relativi al gruppo group
void select_random_centroid(params* input){
	//selezione dei numeri casuali
	srand(time(NULL));
	for(int i=0;i<input->k;i++)
	for(int j=0;j<input->m;j++){
			int rnd=rand() % input->n;
			for(int k=0;k<input->d/input->m;k++){
				input->quant[i*input->d+j*input->d/input->m+k]=input->ds[rnd*input->d+j*input->d/input->m+k];
		}
	}
}
float absoluteValue(float r){
	if(r>0)
		return r;
	return -r;
}

void sub_k_means(params* input,int group){
	 printf("GRUPPO %d\n\n", group);
	for(int i=0;i<input->tmin;i++){
		updateNN(input,group);
	 	float* newCentroids=mediaGeometrica(input,group);
		float increment=calcolaDifferenza(input,group,newCentroids);
		 printf("Al passo %d i centroidi sono stati spostati di un totale di %1.3f\n",i, increment);
		updateCentroids(input,group,newCentroids);
		free(newCentroids);
	}
	updateNN(input,group);
	int max= input->tmax;
	float lastIncrement=input->eps+1;
	float oldLastIncrement=0;
	// devo avere tentativi e devo incrementare almeno di eps
	while(max>0 ){
			updateNN(input,group);
	 		float* newCentroids=mediaGeometrica(input,group);
			lastIncrement=calcolaDifferenza(input,group,newCentroids);
			if(absoluteValue(lastIncrement-oldLastIncrement)<input->eps) break;
			updateCentroids(input,group,newCentroids);
			printf("Passo %d newIncrement %f\n",max, lastIncrement);
			//stampaMappa(input);
		//	printf("\n");
		max--;
		oldLastIncrement=lastIncrement;

	}

}
