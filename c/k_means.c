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

//group GRUPPO
//c Centroide
//v componente del centroide
int getCentroidIndex(params* input,int group,int c,int v){
	return c*input->d+input->d/input->m*group+v;
}
void popolaANN_ES(params* input);
void popolaANN_EA(params* input);

void popolaANN(params* input){
	if(input->exaustive==1&&input->symmetric==1){
		printf("simmetrica\n");
		popolaANN_ES(input);
	}
	else
	if(input->exaustive==1&&input->symmetric==0){
		printf("Asimmetrica\n" );
		popolaANN_EA(input);
	}
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

int* quantize(params* input,int q){
	float* distanze=malloc(input->k*sizeof(float));
	int* risultato= malloc(input->m*sizeof(int));
	int dm= input->d/input->m;
	for(int m=0;m<input->m;m++){
		int indiceInizioQuery= q*input->d+m*dm;
		for(int i=0;i<input->k;i++){
			int indiceInizioCentroide=i*input->d+m*dm;
			distanze[i]=dist(input->quant,input->qs,indiceInizioCentroide,indiceInizioQuery,dm);
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
				dx=dx+dist(input->quant,input->qs,centroideX,i*d+m*dm,dm);
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

void writeCentroid(params* input,int group, int passo) {


	for(int j=0;j<input->k;j++){
		FILE* fp;
		char buf[256];
		snprintf(buf, sizeof buf, "../csv/p %d c %d g %d.csv",passo,j,group);
		fp = fopen(buf, "w+");
		fprintf(fp,"%s,","x");
		fprintf(fp,"%s,\n","y");

			for(int m=0;m<input->d/input->m;m++){
				fprintf(fp,"%f,",input->quant[j*input->d+group*input->d/input->m+m]);
			}
			fprintf(fp,"\n");
			for(int p=0;p<input->n;p++){
				if(input->map[p*input->m+group]==j){
					for(int m=0;m<(input->d/input->m);m++){
						fprintf(fp,"%f,",input->ds[p*input->d+group*input->d/input->m+m]);
					}
					fprintf(fp,"\n");

				}
			}
			fclose(fp);

		}
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
			 		M[j*k*k+i*k+q]=dist(input->quant,input->quant,i1,i2,d/m);
			 }
		 }
	 }
}
void stampaMappa(params* input){
	MAP map=input->map;
	for(int i=0;i<input->n;i++){
		for(int j=0;j<1;j++){
			printf("[%d,%d]->%d  ",i,j,map[i*input->m+j]);
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
		int dm= (int)input->d/input->m;
		for(int i=0;i<input->n;i++){
			int i1=i*input->d+group*dm;
			int min=0;
			for(int j=0;j<input->k;j++) {
				int j1=j*input->d+group*dm;
				dis[j]=0;
				for(int k=0;k<input->d/input->m;k++)
					dis[j]+=(ds[i1+k]-centroidi[j1+k])*
						(ds[i1+k]-centroidi[j1+k]);
				dis[j]=sqrtf(dis[j]);
				if(dis[min]>dis[j])
					min=j;
				map[i*input->m+group]=min;

			}

		}
		free(dis);
}
float calcolaDifferenza(params* input,int group,float* newCentroid){
		float diff=0;
		float tot=0;
		MATRIX q=input->quant;
		int dm=input->d/input->m;
		//per ogni centroide
		for(int i=0;i<input->k;i++){
			diff=0;
			int i1=i*input->d+group*dm;
			int i2=i*dm;
			//per ogni componente del centroide i
			for(int j=0;j<dm;j++){
				//eleva al quadrato la differenza delle componenti
				diff+=(q[i1+j]-newCentroid[i2+j])*(q[i1+j]-newCentroid[i2+j]);
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
	int dm= input->d/input->m;
		for(int i=0;i<input->k;i++){
			int i1=i*input->d+group*dm;
			int i2=i*dm;
			for(int j=0;j<input->d/input->m;j++)
					input->quant[i1+j]=newCentroids[i2+j];
		}
}


//calcola la media geometrica dei punti mappati dai centroidi di un gruppo
float* mediaGeometrica(params* input,int group){
	//contiene blocchi k blocchi di d/m elementi. ogni blocco rappresenta la media mediaGeometrica
	//dei punti mappati dal centroide k
 	float* media= malloc((input->k*(input->d/input->m))*sizeof(float));
	MAP map= input->map;
	int*occ=malloc(input->k*sizeof(int));
	int dm=(int)input->d/input->m;

	for(int i=0;i<input->k;i++)
		occ[i]=1;

		for(int i=0; i<input->k;i++) {
			int i1=i*input->m+group;

			for(int m=0;m<input->d/input->m;m++)
				media[i*dm+m]=
				input->quant[i*input->d+group*dm+m];
}

	for(int i=0; i<input->n;i++){
		int i1=i*input->m+group;
		for(int m=0;m<input->d/input->m;m++){
			media[map[i1]*dm+m]+=
			input->ds[i1*dm+m];
		}
		occ[map[i1]]++;
		// printf("Centroide %d mappa %d nel gruppo %d, dopo aggiornamento vale %d \n",map[i*input->m+group],i,group,occ[map[i*input->m+group]]);
	}
	for(int i=0;i<input->k;i++){
	// 	if(occ[i]==0)
	// 		printf("Centroide %d zero occorrenze\n",i );
		for(int j=0;j<input->d/input->m;j++)
			media[i*dm+j]/=occ[i];
 }
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
							dis[j]=dist(ds,centroidi,i*input->d+group*input->d/input->m+j,group*input->d/input->m+j*input->d,input->d/input->m);


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

	for(int i=0;i<input->m;i++){
			sub_k_means(input,i);
		}
		calcDistMatrix(input);
	 // stampaMappa(input);
	 // stampaCentroidi(input);

	// stampaQuantiMappatiPerOgniCentroide(input);
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
	// printf("GRUPPO %d\n\n", group);
	for(int i=0;i<input->tmin;i++){
		updateNN(input,group);
	 	float* newCentroids=mediaGeometrica(input,group);
		float increment=calcolaDifferenza(input,group,newCentroids);
		 //printf("Al passo %d i centroidi sono stati spostati di un totale di %1.3f\n",i, increment);
		 // writeCentroid(input,group,i);

		updateCentroids(input,group,newCentroids);
	//	stampaCentroidi(input);
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
			//printf("Passo %d newIncrement %f\n",max, lastIncrement);
			//stampaMappa(input);
		//	printf("\n");
		max--;
		oldLastIncrement=lastIncrement;

	}

}
