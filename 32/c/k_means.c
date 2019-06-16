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
void* get_block(int size, int elements);
void free_block(void* p);
//group GRUPPO
//c Centroide
//v componente del centroide
extern float dista(MATRIX m1, MATRIX m2, int x1, int x2, int k);
extern float dista2(MATRIX m1, MATRIX m2, int x1, int x2, int k);
//extern float* vettoreDistanze(MATRIX m1, MATRIX m2, int x1, int x2, int dim,int d,int k);

// calola la distanza geometrica tra il punto x1 e x2
// x1 e x2 devono avere la stessa dimensione k
// x1 appartiene a m1 e x2 appartiene a m2
float dist(MATRIX m1, MATRIX m2, int x1, int x2, int k){
	float d=0;
		for(int i=0;i<k;i++)
			d=d+(m1[x1+i]-m2[x2+i])*(m1[x1+i]-m2[x2+i]);
	return sqrtf(d);
}
float distanza(MATRIX m1, MATRIX m2, int x1, int x2, int k){
	return 	dista( m1,  m2,  x1,  x2,  k);
}
// extern float dist(MATRIX m1, MATRIX m2, int x1, int x2, int k);

void writeCentroid(MATRIX qs,MATRIX quant,MAP map,int n,int m,int d,int k,int group, int passo);


void stampaMappa(MAP map,int n, int m){

	for(int i=0;i<n;i++){
		for(int j=0;j<m;j++){
			printf("[%d,%d]->%d  ",i,j,map[i*m+j]);
}
		printf("\n");
}
}

//Esegue k-means sul gruppo group del dataset ds, aggiornando la mappa e i centroidi
//ds ->dataset
// centroids _> centroidi
void sub_k_means(MATRIX ds, MATRIX centroids, MAP map, int n, int d, int m, int k, int group, int tmin,int tmax, float eps);
//Seleziona n centroidi casuali dal dataset ds e li memorizza in centroids
void select_random_centroid(MATRIX ds,MATRIX centroids, int n, int d, int m, int k);

 //Aggiorna gli ann in map relativi al gruppo group servendosi dei centroidi
void updateNN(MATRIX ds,MATRIX centroids, MAP map,int n, int d, int m, int k, int group){
		//vettore che contiene distanze
	//	float* dis= get_block(sizeof(float), k);
		int dm= (int)d/m;
		//per ogni elemento del dataset vado a cercare quale centroide gli è più vicino
		for(int i=0;i<n;i++){
			int i1=i*d+group*dm;
			//posizione ipotetica del minimo
			int min=0;
			float minV= 1000000000000000000;
			//TODO si può eliminare il vettore in realtà
			//calcola la distanza e aggiorna la posizione del minimo
			int j=0;

			for(j=0;j<k;j+=4) {
				int j1=j*d+group*dm;
				float d1=distanza(ds,centroids,i1,j1,dm);
				float d2=distanza(ds,centroids,i1,j1+d,dm);
				float d3=distanza(ds,centroids,i1,j1+2*d,dm);
				float d4=distanza(ds,centroids,i1,j1+3*d,dm);

				if(minV>d1){
					minV=d1;
					min=j;
				}
				if(minV>d2){
					minV=d2;
					min=j+1;
				}
				if(minV>d3){
					minV=d3;
					min=j+2;
				}

				if(minV>d4){
					minV=d4;
					min=j+3;
				}

				}
				for(;j<k;j++) {
					int j1=j*d+group*dm;
					float d1=distanza(ds,centroids,i1,j1,dm);
					if(minV>d1){
						minV=d1;
						min=j;
					}
				}
				//vai nella mappa relativa al gruppo group del punto i e aggiorna il centroide da cui è mappato
				map[i*m+group]=min;
		}
}
extern float calcolaDifferenzaVect(MATRIX m1, MATRIX m2, int group, int k, int dm);
//calcola la somma di quanto i vettori in centroids(solo relativi al gruppo group) e
//in newCentroid si sono spostati
float calcolaDifferenza(MATRIX centroids,int d,int m, int k, int group,float* newCentroid){
		float diff=0;
		int dm=d/m;
		int gdm=group*dm;
		//per ogni centroide
		int i=0;
		for(i=0;i<k;i+=8){
			int i1=i*d+gdm;
			int i2=i*dm;
			//per ogni componente del centroide i
			diff=diff+distanza(centroids,newCentroid,i1,i2,dm);
			diff=diff+distanza(centroids,newCentroid,i1+d,i2+dm,dm);
			diff=diff+distanza(centroids,newCentroid,i1+2*d,i2+2*dm,dm);
			diff=diff+distanza(centroids,newCentroid,i1+3*d,i2+3*dm,dm);
			diff=diff+distanza(centroids,newCentroid,i1+4*d,i2+4*dm,dm);
			diff=diff+distanza(centroids,newCentroid,i1+5*d,i2+5*dm,dm);
			diff=diff+distanza(centroids,newCentroid,i1+6*d,i2+6*dm,dm);
			diff=diff+distanza(centroids,newCentroid,i1+7*d,i2+7*dm,dm);
		}

		for(;i<k;i++){
			int i1=i*d+gdm;
			int i2=i*dm;
			diff=diff+distanza(centroids,newCentroid,i1,i2,dm);
			}

		return diff;
}
void stampaVettore(MATRIX f,int start,int stop){
	printf("VETTORE DI DIMENSIONE %d\n",stop-start);
	for(int i=start;i<stop;i++)
			printf("%1.2f  ,",f[i]);
		printf("\n");

}
float calcolaDifferenza2(MATRIX centroids,int d,int m, int k, int group,float* newCentroid){
		// float diff=0;
		// float tot=0;
		// MATRIX q=centroids;
		// int dm=d/m;
		// //pdistaer ogni centroide
		// float* dis=vettoreDistanze(q,newCentroid,d+group*dm,dm,dm,d,k);
		// for(int i=0;i<k;i++){
		// 	dis[i]=sqrtf(dis[i]);
		// 	}
		// 	int totale=0;
		//
		// for(int i=0;i<k;i++){
		// 	diff=0;
		// 	int i1=i*d+group*dm;
		// 	int i2=i*dm;
		// 	//per ogni componente del centroide i
		// 	float d= dist(q,newCentroid,i1,i2,dm);
		// 	// stampaVettore(q,i1,i1+dm);
		// 	// stampaVettore(newCentroid,i2,i2+dm);
		// 	// printf("GIUSTA %f \n",dist(q,newCentroid,i1,i2,dm));
		// 	// printf("SBAGLIATA %f \n",d );
		// 	// printf("\n");
		// 	printf("GIUSTA %f\n",d);
		// 	printf("SBAGLIATA %f\n",dis[i]);
		//
		// 	totale=totale+dis[i];
		// 	diff=diff+d;
		// 	tot+=diff;
		// }
		// printf("GIUSTA %f\n",tot);
		//
		// 	stampaVettore(dis,0,k);
		//
		// free_block(dis);
		// printf("SBAGLIATA %f\n",totale);
		// return tot;
}

//aggiorna il valore dei centroidi del gruppo group con i nuovi valori contenuti in newCentroids
void updateCentroids(MATRIX centroids,int d, int m, int k,int group,float* newCentroids){
	int dm= d/m;
		for(int i=0;i<k;i++){
			int i1=i*d+group*dm;
			int i2=i*dm;
			for(int j=0;j<dm;j++)
					centroids[i1+j]=newCentroids[i2+j];
		}
}


//calcola la media geometrica dei punti mappati dai centroidi di un gruppo
float* mediaGeometrica(MATRIX ds,MATRIX centroids,MAP map, int n, int d, int m, int k, int group){

	int dm=(int)d/m;
	//contiene blocchi k blocchi di d/m elementi. ogni blocco rappresenta la media mediaGeometrica
	//dei punti mappati dal centroide k
 	float* media= get_block(sizeof(float), k*dm);
	//vettore delle occorrenze che dice quanti punti il centroide i mappa
	int* occ=get_block(sizeof(int),k);
	for(int i=0;i<k;i++)
		occ[i]=1;
		for(int i=0; i<k;i++) {
			int i1=i*m+group;
			for(int m=0;m<dm;m++)
				media[i*dm+m]=
				centroids[i*d+group*dm+m];
}

	for(int i=0; i<n;i++){
		int i1=i*m+group;
		for(int m=0;m<dm;m++){
			media[map[i1]*dm+m]+=ds[i1*dm+m];
		}
		occ[map[i1]]++;
	}
	for(int i=0;i<k;i++){
		for(int j=0;j<dm;j++)
			media[i*dm+j]/=occ[i];
 }
 free_block(occ);
return media;
}

void stampaQuantiMappatiPerOgniCentroide(params* input){
	MAP map= input->map;
	// stampaMappa(input);

	int** occ= malloc(input->k*sizeof(int));
	for(int i=0;i<input->k;i++)
		occ[i]=malloc(input->m*sizeof(int));

	for(int i=0;i<input->k;i++)
	for(int j=0;j<input->m;j++)

		occ[i][j]=0;
	//per ogni riga
	for(int i =0;i< input->n;i++)
	// per ogni gruppo
		for(int j=0;j<input->m;j++)
				occ[map[i*input->m+j]][j]++;
			for(int j=0;j<input->m;j++){
				for(int i =0;i< input->k;i++){

				printf("[c:%d, g:%d, m:%d] \t",i,j,occ[i][j]);
				printf("\n" );
				if(occ[i][j]==0){
					printf("Centroide %d del gruppo %d non ne mappa nessuno\n	Sarà vero?\n",i,j);
					//Per ogni riga
					MATRIX ds= input->ds;
					MATRIX centroidi= input->quant;
					MAP map= input->map;
					int mappati=0;
					int group=j;
					float* dis= malloc(input->k*sizeof(float));
					for(int i=0;i<input->k;i++)
						dis[i]=0;
					//per ogni punto del dataset
					for(int i =0;i<input->n;i++){

						//per ogni cetroide del gruppo group
						//calcola le distanze e memorizzale in dist
						int min=0;
						for(int j=0;j<input->k;j++){
							dis[j]=distanza(ds,centroidi,i*input->d+group*input->d/input->m+j,group*input->d/input->m+j*input->d,input->d/input->m);
							if(dis[j]<=dis[min])
								min=j;
						}
						if(min==i)
							mappati++;
					}
					free(dis);
					printf("Invece ne mappa %d\n",mappati );
			}
		}
		printf("\n" );

	}
			for(int i=0;i<input->k;i++)
				free(occ[i]);

		free(occ);
	}
//calcola i k centroidi attraverso  k_means
//una chiamata a sub_k_means li calcola per ogni gruppo 0<=i<=m
void k_means(MATRIX ds, MATRIX centroids, MAP map, int n, int d, int m, int k, int tmin,int tmax, float eps){
	// int dim=17;
	// float q1[17] = {3,2, 0, 1,7,4, 3,2,7,3,56,3,45,7,35,6,1};
	// float q2[17] = {1,1,3,0,5,6,6,6,8,34,6,4,6,76,13,4,4};
	// stampaVettore(q1,0,dim);
	// stampaVettore(q2,0,dim);
	//  printf("SBAGLIATA %f\n",dista(q1,q2,0,0,16));
	//  printf("GUSTA %f\n\n",dist(q1,q2,0,0,16));
	//  exit(-1);
	// dimensione matrice quant:
	// k:numero di centroidi per gruppo
	//m:numero di gruppi
	//d:numero di dimensioni di un punto del dataset
	select_random_centroid(ds,centroids, n, d, m, k);

	for(int i=0;i<m;i++)
			sub_k_means(ds,centroids, map, n, d, m, k,i, tmin,tmax,eps);
	}
//calcola i k centroidi casuali relativi al gruppo group
void select_random_centroid(MATRIX ds,MATRIX centroids, int n, int d, int m, int k){
	//selezione dei numeri casuali
	//inizializzazione del seme
	srand(time(NULL));
	for(int i=0;i<k;i++)
	for(int j=0;j<m;j++){
			int rnd=i;//rand()%n;
			for(int p=0;p<d/m;p++){
				centroids[i*d+j*d/m+p]=ds[rnd*d+j*d/m+p];
		}
	}
}
float absoluteValue(float r){
	if(r>0)
		return r;
	return -r;
}

void sub_k_means(MATRIX ds, MATRIX centroids, MAP map, int n, int d, int m, int k, int group, int tmin,int tmax, float eps){
	// printf("GRUPPO %d\n\n", group);
	for(int i=0;i<tmin;i++){
		// printf("PASSO-%d\n",i );
		// if(group==0)
		// writeCentroid(ds,centroids,map,n,m,d,k,group,i);
		updateNN(ds,centroids,map,n,d,m,k,group);
	 	float* newCentroids=mediaGeometrica(ds,centroids,map,n,d, m, k,group);
		// float increment=calcolaDifferenza(centroids,d,m,k,group,newCentroids);
		// float increment2=calcolaDifferenza2(centroids,d,m,k,group,newCentroids);
		// printf("Al passo %d i centroidi sono stati spostati di un totale di %1.3f\n",i, increment);
		// printf("Al passo %d i centroidi sono stati spostati di un totale di %1.3f\n\n",i, increment2);
		 // writeCentroid(input,group,i);
		updateCentroids(centroids,d,m,k,group,newCentroids);
	//	stampaCentroidi(input);
		free_block(newCentroids);
	}
	int max= tmax;
	float lastIncrement=eps+1;
	float oldLastIncrement=0;
	// devo avere tentativi e devo incrementare almeno di eps
	while(max>0){
		// printf("PASSO-%d\n",tmax-max );
		updateNN(ds,centroids,map,n,d,m,k,group);
		float* newCentroids=mediaGeometrica(ds,centroids,map,n,d, m, k,group);
		lastIncrement=calcolaDifferenza(centroids,d,m,k,group,newCentroids);
			if(absoluteValue(lastIncrement-oldLastIncrement)<eps) break;
			updateCentroids(centroids,d,m,k,group,newCentroids);
			// float increment2=calcolaDifferenza2(input,group,newCentroids);
			// printf("Passo %d newIncrement %f\n",max, lastIncrement);
			// printf("Passo %d newIncrement SBAGLIATO %f\n\n",max, increment2);
			//stampaMappa(input);
		//	printf("\n");
		max--;
		oldLastIncrement=lastIncrement;
		free_block(newCentroids);

	}
	// updateNN(ds,centroids,map,n,d,m,k,group);

}
