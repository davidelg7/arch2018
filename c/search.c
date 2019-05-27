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
void popolaANN_ES(params* input);
void popolaANN_EA(params* input);
float calcDistMatrix(params*input);
extern float dista(MATRIX m1, MATRIX m2, int x1, int x2, int k);
float dist(MATRIX m1, MATRIX m2, int x1, int x2, int k);
void popolaANN(params* input){
	printf("Fatto\n");
	float res=0;
	//
	// printf("%f\n",dista(input->quant,input->quant,16,16,(int)(input->d/input->m)));
	// printf("%d\n",input->quant);
	if(input->exaustive==1&&input->symmetric==1){
    calcDistMatrix(input);
		popolaANN_ES(input);
	}
	else
	if(input->exaustive==1&&input->symmetric==0){
		popolaANN_EA(input);
	}

}

int* quantize(params* input,int q){
	float* distanze=malloc(input->k*sizeof(float));
	int* risultato= malloc(input->m*sizeof(int));
	int dm= input->d/input->m;
	for(int m=0;m<input->m;m++){
		int indiceInizioQuery= q*input->d+m*dm;
		for(int i=0;i<input->k;i++){
			int indiceInizioCentroide=i*input->d+m*dm;
			distanze[i]=dista(input->quant,input->qs,indiceInizioCentroide,indiceInizioQuery,dm);
		}
		int min=0;
		for(int j=0;j<input->k;j++)
			if(distanze[min]>distanze[j])
				min=j;
		risultato[m]=min;
	}
	free(distanze);
	return risultato;
}
//Popola la struttura ANN inserendoci dentro le posizioni di knn approximated nn del dataset per ogni punto query in Queryset
void popolaANN_EA(params* input){
	int k= input->k;
	int dm= input->d/input->m;
	int d=input->d;
	//Per ogni punto del queryset
	for(int i=0;i<input->nq;i++){
		float*vicini= malloc(input->knn*2*sizeof(float)); //(id,dist)
		int p=0;
		for(int x=0;x<input->n;x++){
			float dx=0;
			for(int m=0;m<input->m;m++){
				int centroideX=input->map[x*input->m+m]*d+m*dm;
				dx=dx+dista(input->quant,input->qs,centroideX,i*d+m*dm,dm);
		}
			dx=sqrtf(dx);
			//Se ancora non ne ho trovati knn,metto i primi che trovo
			if(p<input->knn*2){
				vicini[p]=x;
				vicini[p+1]=dx;
				p+=2;
			}
			else{
				int max=0;
				for(int l=0;l<input->knn*2;l+=2)
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
		for(int s=0;s<input->knn;s++){
			input->ANN[i*input->knn+s]=(int)vicini[s*2];
		}
		free(vicini);
	}
}
//Popola la struttura ANN inserendoci dentro le posizioni di knn approximated nn del dataset per ogni punto query in Queryset
void popolaANN_ES(params* input){
	MATRIX distanze=input->dis;
	int k= input->k;
	int dm= input->d/input->m;
	//Per ogni punto del queryset
	for(int i=0;i<input->nq;i++){
		int*q=quantize(input,i);
		float*vicini= malloc(input->knn*2*sizeof(float)); //(id,dist)
		int p=0;
		for(int x=0;x<input->n;x++){
			float dx=0;
			for(int m=0;m<input->m;m++){
				//avanzo del gruppo
				int indice=m*k*k;
				// printf("%d\n",indice);
				//avanzo del Centroide x
				indice+=input->map[x*input->m+m]*k;
				//avanzo del centroide mappato da i
				indice+=q[m];
				dx=dx+input->dis[indice]*input->dis[indice];

		}
			dx=sqrtf(dx);
			//Se ancora non ne ho trovati knn,metto i primi che trovo
			if(p<input->knn*2){
				vicini[p]=x;
				vicini[p+1]=dx;
				p+=2;
			}
			else{
				int max=0;
				for(int l=0;l<input->knn*2;l+=2)
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
		for(int s=0;s<input->knn;s++){
			input->ANN[i*input->knn+s]=(int)vicini[s*2];
		}
		free(q);
		free(vicini);
	}

}
void stampaVettore(MATRIX f,int start,int stop);
float calcDistMatrix(params*input){
	int k=input->k;
	int m= input->m;
	int d=input->d;
	MATRIX M= input->dis;
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
			 		M[j*k*k+i*k+q]=dist(input->quant,input->quant,i1,i2,d/m);
			 }
		 }
	 }
}
