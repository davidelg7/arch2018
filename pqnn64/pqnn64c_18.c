/**************************************************************************************
 *
 * CdL Magistrale in Ingegneria Informatica
 * Corso di Architetture e Programmazione dei Sistemi di Elaborazione - a.a. 2018/19
 *
 * Progetto dell'algoritmo di Product Quantization for Nearest Neighbor Search
 * in linguaggio assembly x86-32 + SSE
 *
 * Fabrizio Angiulli, aprile 2019
 *
 **************************************************************************************/

/*

 Software necessario per l'esecuzione:

     NASM (www.nasm.us)
     GCC (gcc.gnu.org)

 entrambi sono disponibili come pacchetti software
 installabili mediante il packaging tool del sistema
 operativo; per esempio, su Ubuntu, mediante i comandi:

     sudo apt-get install nasm
     sudo apt-get install gcc

 potrebbe essere necessario installare le seguenti librerie:

     sudo apt-get install lib32gcc-4.8-dev (o altra versione)
     sudo apt-get install libc6-dev-i386

 Per generare il file eseguibile:

 nasm -f elf32 pqnn32.nasm && gcc -O0 -m32 -msse pqnn32.o pqnn32c.c -o pqnn32c && ./pqnn32c

 oppure

 ./runpqnn32

*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <xmmintrin.h>
#include <omp.h>

#define	MATRIX		float*
#define	VECTOR		int*
#define MAP 			int*
int TEST =1; //1 if testing
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
	MAP map2;
	MATRIX res;
	MATRIX quant2;

	MATRIX ds2; //contriene nr punti residui casuali

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







/*
 *
 *	Le funzioni sono state scritte assumento che le matrici siano memorizzate
 * 	mediante un array (float*), in modo da occupare un unico blocco
 * 	di memoria, ma a scelta del candidato possono essere
 * 	memorizzate mediante array di array (float**).
 *
 * 	In entrambi i casi il candidato dovrà inoltre scegliere se memorizzare le
 * 	matrici per righe (row-major order) o per colonne (column major-order).
 *
 * 	L'assunzione corrente è che le matrici siano in row-major order.
 *
 */


void* get_block(int size, int elements) {
	return _mm_malloc(elements*size,32);
}


void free_block(void* p) {
	_mm_free(p);
}


MATRIX alloc_matrix(int rows, int cols) {
	return (MATRIX) get_block(sizeof(double),rows*cols);
}


void dealloc_matrix(MATRIX mat) {
	free_block(mat);
}


/*
 *
 * 	load_data
 * 	=========
 *
 *	Legge da file una matrice di N righe
 * 	e M colonne e la memorizza in un array lineare in row-major order
 *
 * 	Codifica del file:
 * 	primi 4 byte: numero di righe (N) --> numero intero a 32 bit
 * 	successivi 4 byte: numero di colonne (M) --> numero intero a 32 bit
 * 	successivi N*M*4 byte: matrix data in row-major order --> numeri floating-point a precisione doppia
 *
 *****************************************************************************
 *	Se lo si ritiene opportuno, è possibile cambiare la codifica in memoria
 * 	della matrice.
 *****************************************************************************
 *
 */
MATRIX load_data(char* filename, int *n, int *d) {
	FILE* fp;
	int rows, cols, status, i;

	fp = fopen(filename, "rb");

	if (fp == NULL) {
		printf("'%s' : bad data file name!\n", filename);
		exit(0);
	}

	status = fread(&cols, sizeof(int), 1, fp);
	status = fread(&rows, sizeof(int), 1, fp);
	//HO MODIFICATO ROWS CON COLS
	MATRIX data = alloc_matrix(rows,cols);
	status = fread(data, sizeof(double), rows*cols, fp);
	fclose(fp);

	*n = rows;
	*d = cols;
https://cs.stanford.edu/~jure/pubs/node2vec-kdd16.pdf
	return data;
}


void save_ANN(char* filename, int* ANN, int nq, int knn) {
	FILE* fp;
	int i, j;
	char fpath[256];

	sprintf(fpath, "%s.ann", filename);
	fp = fopen(fpath, "w");
	for (i = 0; i < nq; i++) {
		for (j = 0; j < knn; j++)
			fprintf(fp, "%d ", ANN[i*knn+j]);
		fprintf(fp, "\n");
	}
	fclose(fp);
}


extern void pqnn64_index(params* input);
extern int* pqnn64_search(params* input);


/*
 *	pqnn_index
 * 	==========
 */
void popolaANN(MATRIX qs,MATRIX centroids,MAP ann,MAP map,MATRIX dis,int n, int nq,int d, int m, int k,int knn, int exaustive, int symmetric);
void printMatrix(MATRIX,int,int);
void writeQuery(char* filename);
void writeDataset(char* filename);
void k_means(MATRIX ds, MATRIX centroids, MAP map, int n, int d, int m, int k, int tmin,int tmax, float eps);
void coarse(MATRIX ds, MATRIX centroids, MAP map, int n, int d, int k, int tmin,int tmax, float eps);
void popolaRes(MATRIX ds,MAP map2, MATRIX res, MATRIX quant2, int n , int d, int kc);
void popolaDs2(MATRIX res,MATRIX ds2,MAP map2,int nr,int d);
void popolaANN_NES(MATRIX qs,MATRIX coarse,MAP mapCoarse,MATRIX centroidiPq,MAP mapPq,MATRIX residui,MAP ann,int n,int nq,int d,int m,int kc,int w,int nr,int k,int knn);
void popolaANN_NEA(MATRIX qs,MATRIX coarse,MAP mapCoarse,MATRIX centroidiPq,MAP mapPq,MATRIX residui,MAP ann,int n,int nq,int d,int m,int kc,int w,int nr,int k,int knn);
float dista(MATRIX,MATRIX,int,int,int);
void pqnn_index(params* input) {
	MATRIX ds= input->ds;
	MATRIX centroids= input->quant;
	MAP map= input->map;
	int n= input->n;
	int d= input->d;
	int m= input->m;
	int k= input->k;
	int tmin= input->tmin;
	int tmax= input->tmax;
	float eps= input->eps;
	MAP map2= input->map2;
	MATRIX quant2 = input->quant2;
	MATRIX res = input->res;
	int kc = input->kc;
	MATRIX ds2= input->ds2;
	int nr= input->nr;
	MAP map3= input->map3;
	MATRIX quant3 = input->quant3;
	if(input->exaustive==1)
		k_means(ds, centroids, map, n, d, m, k, tmin, tmax, eps);
	else{
		coarse(ds, quant2, map2, n, d, kc, tmin, tmax, eps);
		popolaRes(ds, map2, res, quant2, n, d, kc);
		popolaDs2(res,ds2,map2,nr,d);
		k_means(ds2, quant3, map3, nr, d, m, k, tmin, tmax, eps);
	}
    // -------------------------------------------------
    // Codificare qui l'algoritmo di indicizzazione
		//
    // -------------------------------------------------

    pqnn64_index(input); // Chiamata funzione assembly

    // -------------------------------------------------

}


/*
 *	pqnn_search
 * 	===========
 */
void writeANN(MATRIX,MAP,MATRIX,int,int);

void pqnn_search(params* input) {
    // -------------------------------------------------
    // Codificare qui l'algoritmo di interrogazione
    // -------------------------------------------------

		if(input->exaustive==1)
		popolaANN(input->qs,input-> quant,input->ANN,input-> map,input-> dis,input-> n, input-> nq,input-> d, input-> m, input-> k,input-> knn, input-> exaustive, input-> symmetric);
		else
		if(input->symmetric==1)
		popolaANN_NES(input->qs,input->quant2,input->map2,input->quant3,input->map3,input->res,input->ANN,input->n,input->nq,input->d,input->m,input-> kc,input-> w,input->nr,input->k,input->knn);
		else
		popolaANN_NEA(input->qs,input->quant2,input->map2,input->quant3,input->map3,input->res,input->ANN,input->n,input->nq,input->d,input->m,input-> kc,input-> w,input->nr,input->k,input->knn);
 		// writeANN(input-> qs,input->ANN,input-> ds,input-> nq,input->d);
	  pqnn64_search(input); // Chiamata funzione assembly

	// Restituisce il risultato come una matrice di nq * knn
	// identificatori associati agli ANN approssimati delle nq query.
	// La matrice è memorizzata per righe
    // -------------------------------------------------

}
float dist(MATRIX m1, MATRIX m2, int x1, int x2, int k);
int main(int argc, char** argv) {

	char fname[256];
	int i, j;

	//
	// Imposta i valori di default dei parametri
	//

	params* input = malloc(sizeof(params));

	input->filename = NULL;
	input->exaustive = 1;
	input->symmetric = 1;
	input->knn = 1;
	input->m = 8;
	input->k = 256;
	input->kc = 8192;
	input->w = 16;
	input->eps = 0.01;
	input->tmin = 10;
	input->tmax = 100;
	input->silent = 0;
	input->display = 0;
	input->nr = 400;

	//
	// Legge i valori dei parametri da riga comandi
	//

	int par = 1;
	while (par < argc) {
		if (par == 1) {
			input->filename = argv[par];
			par++;
		} else if (strcmp(argv[par],"-s") == 0) {
			input->silent = 1;
			par++;
		} else if (strcmp(argv[par],"-d") == 0) {
			input->display = 1;
			par++;
		} else if (strcmp(argv[par],"-knn") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing knn value!\n");
				exit(1);
			}
			input->knn = atoi(argv[par]);
			par++;
		} else if (strcmp(argv[par],"-m") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing m value!\n");
				exit(1);
			}
			input->m = atoi(argv[par]);
			par++;
		} else if (strcmp(argv[par],"-k") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing k value!\n");
				exit(1);
			}
			input->k = atoi(argv[par]);
			par++;
		} else if (strcmp(argv[par],"-kc") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing kc value!\n");
				exit(1);
			}
			input->kc = atoi(argv[par]);
			par++;
		} else if (strcmp(argv[par],"-w") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing w value!\n");
				exit(1);
			}
			input->w = atoi(argv[par]);
			par++;
		} else if (strcmp(argv[par],"-nr") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing nr value!\n");
				exit(1);
			}
			input->nr = atoi(argv[par]);
			par++;
		} else if (strcmp(argv[par],"-eps") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing eps value!\n");
				exit(1);
			}
			input->eps = atof(argv[par]);
			par++;
		} else if (strcmp(argv[par],"-tmin") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing tmin value!\n");
				exit(1);
			}
			input->tmin = atoi(argv[par]);
			par++;
		} else if (strcmp(argv[par],"-tmax") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing tmax value!\n");
				exit(1);
			}
			input->tmax = atoi(argv[par]);
			par++;
 		} else if (strcmp(argv[par],"-exaustive") == 0) {
 			input->exaustive = 1;
 			par++;
 		} else if (strcmp(argv[par],"-noexaustive") == 0) {
 			input->exaustive = 0;
 			par++;
 		} else if (strcmp(argv[par],"-sdc") == 0) {
 			input->symmetric = 1;
 			par++;
 		} else if (strcmp(argv[par],"-adc") == 0) {
 			input->symmetric = 0;
 			par++;
		} else
			par++;
	}

	//
	// Visualizza la sintassi del passaggio dei parametri da riga comandi
	//

	if (!input->silent) {
		printf("Usage: %s <data_name> [-d][-s][-exaustive|-noexaustive][-sdc|-adc][...]\n", argv[0]);
		printf("\nParameters:\n");
		printf("\t-d : display ANNs\n");
		printf("\t-s : silent\n");
		printf("\t-m: PQ groups\n");
		printf("\t-k: PQ centroids\n");
		printf("\t-kc: coarse VQ centroids\n");
		printf("\t-w: coarse VQ centroids to be selected\n");
		printf("\t-nr: residual sample size\n");
		printf("\t-eps: k-means termination threshold\n");
		printf("\t-tmin: min k-means iterations\n");
		printf("\t-tmax: max k-means iterations\n");
		printf("\n");
	}

	//
	// Legge il data set ed il query set
	//
	// writeDataset("Test");
	// writeQuery("Test");
	if (input->filename == NULL || strlen(input->filename) == 0) {
		printf("Missing input file name!\n");
		exit(1);
	}

	sprintf(fname, "%s.ds", input->filename);
	input->ds = load_data(fname, &input->n, &input->d);


	sprintf(fname, "%s.qs", input->filename);
	input->qs = load_data(fname, &input->nq, &input->d);
	if(input->nr<1)
		input->nr = input->n/20;

	//
	// Visualizza il valore dei parametri
	//
	if (!input->silent) {
		printf("Input file name: '%s'\n", input->filename);
		printf("Data set size [n]: %d\n", input->n);
		printf("Number of dimensions [d]: %d\n", input->d);
		printf("Query set size [nq]: %d\n", input->nq);
		printf("Number of ANN [knn]: %d\n", input->knn);
		printf("PQ groups [m]: %d\n", input->m);
		printf("PQ centroids [k]: %d\n", input->k);
		if (!input->exaustive) {
			printf("Coarse VQ centroids [kc]: %d\n", input->kc);
			printf("Coarse VQ centroids to be selected [w]: %d\n", input->w);
			printf("Number of residuals used to determine PQ centroids [nr]: %d\n", input->nr);
		}
		printf("K-means parameters: eps = %.4f, tmin = %d, tmax = %d\n", input->eps, input->tmin, input->tmax);
	}

	//
	// Costruisce i quantizzatori
	//
	//printMatrix(input);

	input->ANN = (MAP) get_block(input->nq*input->knn,sizeof(int));
	if(input->symmetric==1){
		input->dis=(MATRIX)get_block(sizeof(float),input->m*input->k*input->k);//alloc_matrix(input->m,input->k*input->k);
	}
	if(input->exaustive==1){
		input->quant=(MATRIX)get_block(sizeof(float), input->k*input->d);
		input->map=(MAP)get_block(sizeof(int), input->n*input->m);
	}
	if(input->exaustive==0){
		input->quant2=(MATRIX)get_block(sizeof(float), input->kc*input->d);
		input->map2=(MAP)get_block(sizeof(int), input->n*input->m);
		input->res=(MATRIX)get_block(sizeof(float),input->n*input->d);
		input->ds2=(MATRIX)get_block(sizeof(float),input->nr*input->d);
		input->quant3=(MATRIX)get_block(sizeof(float), input->k*input->d);
		input->map3=(MAP)get_block(sizeof(int), input->nr*input->m);

	}

	clock_t t = clock();
	pqnn_index(input);
	t = clock() - t;

	if (!input->silent)
		printf("\nIndexing time = %.3f secs\n", ((float)t)/CLOCKS_PER_SEC);
	else
		printf("%.3f\n", ((float)t)/CLOCKS_PER_SEC);

	//
	// Determina gli ANN
	//



	t = clock();
	pqnn_search(input);
	t = clock() - t;

	if (!input->silent)
		printf("\nSearching time = %.3f secs\n", ((float)t)/CLOCKS_PER_SEC);
	else
		printf("%.3f\n", ((float)t)/CLOCKS_PER_SEC);

	//
	// Salva gli ANN
	//
	int  a[2000]={4882, 6938, 1732, 624, 7595, 1329, 6541, 6888, 5794, 7176, 411, 4217, 623, 5269, 6283, 2801, 3644, 7004, 6711, 4881, 1803, 7487, 1395, 3103, 2874, 4374, 7487, 3842, 3287, 828, 2314, 1692, 2806, 2433, 6639, 7758, 4005, 3295, 3272, 6135, 3407, 224, 2841, 7617, 4347, 7965, 405, 384, 1754, 5496, 1680, 3478, 4779, 224, 1743, 6275, 6382, 3252, 3893, 3644, 6282, 1497, 5451, 2611, 5627, 5635, 2489, 5409, 1883, 2488, 4129, 1893, 3354, 6151, 1324, 3655, 2231, 984, 7873, 189, 6034, 7632, 1561, 3986, 1036, 2209, 3090, 2017, 1210, 2390, 4542, 4555, 1717, 2047, 1868, 2120, 3618, 4468, 4793, 3011, 776, 862, 4893, 3055, 4187, 145, 6202, 4104, 808, 609, 7610, 4854, 7333, 3216, 3945, 783, 1394, 3389, 2555, 1546, 5146, 2716, 2683, 2423, 2151, 3153, 5835, 5818, 710, 7944, 5617, 6594, 4651, 6524, 3936, 6110, 4008, 6793, 4218, 4034, 7164, 1544, 2514, 5476, 1487, 5083, 1998, 6007, 890, 91, 6430, 6178, 4570, 2483, 2565, 2547, 5563, 2674, 905, 4701, 6050, 4590, 6326, 2893, 5584, 5261, 7372, 6228, 3098, 1039, 531, 7899, 5908, 5094, 4754, 5606, 6799, 5165, 4117, 540, 3367, 3849, 776, 5869, 4058, 446, 3338, 1364, 3653, 6357, 2135, 4576, 7173, 607, 2287, 2341, 1031, 637, 6697, 2632, 3076, 1355, 4201, 358, 3013, 3251, 7358, 28, 1391, 6829, 5673, 5433, 6436, 603, 1106, 5621, 4460, 7736, 5059, 4944, 2705, 2497, 1258, 6303, 7194, 7128, 6816, 4548, 495, 758, 3202, 6700, 2770, 7877, 1199, 1119, 6017, 7753, 5578, 7582, 6260, 6717, 6587, 4843, 5714, 2383, 241, 7199, 5193, 2730, 1744, 6996, 2653, 449, 7151, 304, 6249, 7868, 6099, 3546, 3123, 172, 7673, 7192, 6314, 1136, 78, 2918, 4315, 3813, 4229, 2333, 58, 6460, 559, 3805, 4505, 1292, 4315, 1910, 2461, 99, 2464, 852, 777, 4758, 787, 7623, 5926, 4460, 7948, 6686, 2049, 6492, 765, 4331, 2470, 1729, 1356, 233, 2596, 2186, 2632, 7336, 6178, 305, 6716, 2283, 643, 2319, 2285, 216, 257, 6287, 1704, 1144, 3456, 2521, 4693, 3078, 7901, 5078, 4470, 4564, 13, 2859, 1233, 7501, 5571, 5189, 2594, 189, 7866, 1353, 3068, 5380, 1708, 2783, 7851, 7268, 3025, 7292, 2826, 2222, 7272, 1098, 6847, 3565, 7779, 1298, 5096, 2334, 5201, 6077, 3878, 411, 4084, 627, 782, 5255, 54, 2312, 669, 3429, 28, 916, 5676, 5829, 7949, 5307, 6001, 187, 7155, 3501, 5368, 4917, 1132, 6958, 2338, 3135, 6365, 7440, 2101, 3178, 3827, 65, 2517, 6260, 6591, 7530, 6982, 6230, 2906, 562, 4493, 1826, 1774, 4650, 5795, 5061, 5147, 4880, 2493, 4393, 4141, 5461, 6579, 998, 7114, 2995, 3355, 1653, 5862, 3916, 7151, 5560, 6466, 6228, 1244, 4429, 5772, 2781, 698, 724, 4230, 3312, 2612, 159, 318, 624, 4493, 6282, 2233, 1140, 6859, 444, 4569, 15, 1029, 6677, 470, 3778, 2580, 4058, 2668, 5114, 3917, 2932, 1424, 5204, 6358, 3264, 5650, 2550, 5911, 1868, 372, 2265, 6437, 1380, 2152, 3791, 397, 3615, 4674, 5591, 119, 7954, 7450, 1037, 1265, 3954, 4894, 649, 6313, 6061, 5173, 344, 2077, 3016, 6455, 3455, 6789, 5111, 7025, 3524, 2747, 4766, 4597, 4685, 2608, 4835, 7113, 3214, 7943, 2577, 2003, 4698, 4234, 5816, 5197, 7968, 1186, 3099, 2881, 6173, 7712, 1807, 3863, 6436, 3357, 7654, 1921, 4089, 2536, 7574, 5970, 828, 2091, 7953, 5606, 7679, 1322, 5967, 5502, 974, 3316, 794, 5414, 5053, 5081, 7817, 4513, 7731, 1041, 4368, 5338, 4316, 1657, 2823, 3202, 5808, 1323, 3439, 3996, 6887, 582, 3503, 2947, 3066, 6426, 7156, 5877, 7355, 3647, 332, 7507, 3775, 7453, 6152, 2100, 851, 509, 5193, 4735, 3161, 4000, 5760, 4766, 164, 1329, 2404, 6288, 180, 5538, 5712, 466, 7309, 2307, 4870, 4156, 1004, 3040, 6377, 468, 2402, 3929, 6968, 506, 2459, 1734, 881, 4536, 4238, 4336, 1734, 4358, 7171, 7329, 2906, 6268, 3594, 1790, 6636, 1697, 1463, 5409, 1447, 6342, 6168, 6719, 4297, 5376, 1139, 947, 7101, 1853, 3726, 696, 4565, 7189, 4765, 1834, 5195, 7900, 7238, 56, 4549, 4479, 1253, 3854, 1356, 1030, 4445, 5546, 6605, 743, 7120, 1828, 919, 7667, 1234, 5262, 2240, 6726, 3247, 4714, 5368, 512, 3058, 7076, 6797, 6556, 7478, 1701, 5881, 1184, 3828, 4606, 1010, 4925, 6245, 7344, 7526, 5265, 5836, 2728, 795, 6135, 6620, 4012, 5193, 4815, 2767, 6643, 2860, 6008, 5679, 2265, 7759, 155, 4242, 6445, 7676, 468, 7007, 1328, 670, 400, 4575, 386, 916, 6638, 1672, 3893, 1700, 833, 408, 1278, 2897, 4858, 6172, 6418, 3815, 5400, 6255, 6456, 2967, 704, 887, 2377, 4064, 2785, 3025, 3993, 5362, 935, 3998, 3980, 7393, 5905, 6751, 7167, 1339, 3100, 1880, 6527, 6340, 1862, 5908, 1701, 7051, 4116, 3047, 3172, 6635, 4840, 7935, 996, 2795, 3979, 3544, 7881, 760, 1, 6263, 3884, 1547, 1894, 4032, 6387, 7366, 254, 2750, 4039, 950, 582, 7140, 2546, 2316, 2589, 5676, 29, 7585, 4173, 1968, 6265, 985, 3114, 1301, 1825, 651, 1247, 7351, 3258, 7781, 1104, 4161, 4431, 4684, 7259, 879, 2338, 7618, 7298, 3746, 2059, 1175, 6613, 4353, 4670, 2973, 4082, 7762, 5437, 2451, 347, 5385, 7249, 6245, 5872, 5785, 44, 5310, 7419, 6091, 57, 4648, 4407, 7082, 2163, 7597, 4402, 4760, 5279, 7568, 1345, 2411, 6357, 2791, 4437, 546, 1983, 5494, 3206, 7900, 6212, 4132, 2628, 538, 7811, 3062, 5965, 4470, 6669, 818, 4797, 1774, 7680, 5741, 2942, 3198, 636, 7460, 1697, 2982, 6266, 385, 7423, 3960, 7155, 447, 2385, 5164, 6612, 6256, 1051, 1575, 2448, 6823, 1515, 3018, 1194, 4002, 30, 2943, 4667, 7537, 5971, 161, 7679, 649, 2798, 3150, 1364, 2470, 3614, 3841, 5388, 366, 879, 4034, 3805, 6620, 1197, 3042, 1666, 88, 4928, 4262, 5665, 6402, 7214, 5672, 5631, 703, 3386, 5580, 5520, 1554, 6307, 1917, 1734, 2649, 5820, 2947, 7538, 375, 7123, 3318, 4905, 6953, 2767, 2338, 3580, 6599, 6330, 2037, 616, 4025, 6303, 4402, 4509, 6784, 6076, 1298, 1374, 1399, 3974, 896, 6474, 4836, 6893, 1497, 7557, 7658, 7218, 3029, 7353, 560, 3398, 6899, 3499, 3905, 1614, 7725, 6052, 4310, 1322, 1606, 3111, 6214, 4321, 4370, 7032, 1164, 1010, 4434, 3086, 2909, 2594, 3492, 5533, 2598, 3696, 5937, 1941, 6761, 3110, 2440, 5711, 3009, 4883, 3170, 701, 896, 603, 5712, 4212, 2563, 7000, 4897, 6467, 1420, 4825, 1284, 7151, 3645, 7453, 2770, 4690, 316, 994, 7541, 3752, 2356, 6818, 277, 2076, 7866, 5271, 804, 4382, 4756, 4314, 1465, 4121, 2608, 3177, 5094, 3188, 3338, 358, 3459, 5144, 339, 1715, 587, 850, 2847, 1492, 6936, 7875, 296, 5529, 5696, 2852, 3349, 5949, 4994, 1148, 4581, 1047, 2141, 2244, 3413, 5774, 7054, 2810, 389, 384, 764, 4112, 7739, 7388, 1877, 4789, 1897, 2488, 3116, 2189, 3225, 2569, 6438, 1599, 7601, 2508, 299, 5017, 5273, 7902, 2688, 1836, 4390, 4884, 2909, 4367, 6414, 3355, 1382, 4667, 6236, 698, 7621, 6465, 789, 4875, 1333, 7905, 5360, 5777, 6616, 677, 2982, 7178, 1439, 7548, 6484, 3144, 1434, 5698, 497, 4156, 5334, 420, 4969, 1672, 6475, 4628, 1003, 7377, 2308, 3554, 6809, 1799, 3624, 3172, 2327, 2406, 4457, 6036, 725, 3516, 7041, 3325, 7429, 2365, 240, 6820, 776, 7529, 6568, 5786, 4758, 4743, 1303, 2967, 1770, 7484, 447, 7858, 322, 800, 5699, 2469, 3533, 2049, 6281, 7265, 6026, 6809, 7448, 2265, 4507, 1099, 6650, 5902, 7023, 6270, 2794, 3923, 1457, 2291, 2198, 5883, 676, 5658, 7973, 3533, 5687, 535, 396, 2263, 972, 1749, 5432, 1152, 3255, 3295, 7979, 3924, 3856, 3416, 2121, 3401, 2909, 4165, 4574, 2391, 4500, 852, 3823, 4182, 3351, 4155, 483, 5626, 373, 5811, 3755, 6845, 2849, 1614, 6832, 5017, 5255, 4426, 4229, 549, 6574, 4334, 2594, 7895, 5595, 3275, 3302, 844, 554, 1737, 5200, 4054, 3327, 3229, 1534, 4806, 1146, 44, 5608, 1596, 458, 4277, 7857, 5626, 3283, 3399, 511, 7662, 2904, 6383, 3749, 777, 7399, 6010, 7156, 219, 7510, 6413, 5029, 2444, 5339, 6660, 3069, 395, 4746, 5435, 4713, 635, 950, 3078, 6436, 6295, 3564, 3635, 1581, 983, 180, 3390, 7700, 2981, 130, 5139, 209, 3272, 7969, 5724, 2666, 2884, 1749, 1056, 845, 342, 4969, 2401, 2715, 1686, 413, 2698, 3336, 2396, 5138, 4490, 7058, 2019, 3116, 2787, 2603, 1700, 5278, 3089, 3215, 3308, 183, 3796, 6456, 5572, 7226, 2866, 1413, 3301, 5820, 2103, 5926, 1243, 11, 4158, 6469, 65, 310, 3065, 6527, 151, 1341, 836, 2283, 4232, 7602, 5331, 4340, 4614, 887, 283, 6089, 7085, 3513, 3203, 4676, 477, 1431, 5909, 6049, 1741, 6540, 3946, 5840, 5091, 3026, 4370, 5727, 12, 2031, 7467, 381, 6566, 4481, 5413, 5814, 5594, 5459, 2043, 7944, 7038, 5711, 2663, 5266, 4245, 4161, 2890, 1258, 6489, 2250, 7445, 4788, 6878, 6508, 3297, 7352, 883, 1351, 5968, 3152, 216, 3449, 5982, 4693, 1686, 3432, 999, 5686, 1457, 2153, 4374, 3871, 1972, 5700, 3490, 123, 1517, 1711, 1052, 5487, 7726, 279, 135, 4747, 5402, 3126, 3314, 4402, 4909, 7800, 3178, 6797, 118, 1842, 512, 4901, 3992, 2505, 4792, 2645, 7129, 4958, 4365, 2901, 1595, 81, 1679, 1260, 5437, 547, 2729, 7743, 1329, 109, 4192, 5644, 273, 1691, 1290, 3429, 1568, 1659, 2269, 4888, 2826, 6139, 5167, 4371, 6851, 4575, 4116, 4038, 7349, 3977, 858, 7804, 6809, 3152, 7300, 1609, 6933, 6480, 3063, 844, 373, 6336, 4882, 7432, 5850, 2664, 7067, 2451, 286, 3414, 400, 229, 3436, 7261, 2095, 7621, 6519, 7075, 2836, 3315, 7639, 7536, 5144, 2996, 4991, 2341, 1588, 6971, 3610, 2762, 4314, 6320, 7119, 5833, 1242, 4913, 1821, 6081, 1891, 3235, 3970, 7016, 2702, 5792, 1582, 4004, 5150, 100, 6533, 7566, 4615, 6559, 1366, 6428, 2647, 835, 5113, 2682, 5333, 5485, 3221, 5720, 2459, 5555, 5051, 1140, 6966, 4118, 132, 2206, 4976, 2047, 478, 3009, 6577, 7028, 1665, 207, 507, 7132, 7214, 3759, 1575, 2546, 3327, 6508, 1678, 3031, 6485, 2179, 393, 1929, 111, 183, 5654, 1351, 1123, 3846, 5014, 6527, 7689, 6560, 5544, 7801, 6381, 1272, 7968, 1391, 368, 5040, 3966, 2206, 6043, 3188, 4776, 3879, 4085, 7991, 5442, 4022, 4093, 7216, 7103, 776, 2694, 2511, 1407, 7751, 4635, 837, 4278, 3217, 1590, 5848, 4134, 2525, 5113, 3750, 2296, 4934, 3095, 5523, 2827, 1183, 658, 5852, 1633, 5401, 7803, 1587, 7365, 3800, 4772, 817, 389, 4154, 4410, 3773, 5623, 1254, 1213, 1471, 6218, 1794, 1955, 7688, 6084, 2159, 4007, 3622, 7519, 5596, 6436, 5131, 4143, 2003, 3241, 4259, 1352, 632, 1487, 7323, 1304, 7348, 1811, 302, 3312, 4378, 5270, 7655, 3705, 4221, 1590, 4225, 4898, 7563, 7960, 4369, 1527, 4483, 5424, 4018, 4820, 3654, 160, 7154, 2133, 5406, 2160, 3505, 2382, 1974, 4004, 70, 1766, 836, 4246, 1632, 2486, 3646, 6921, 2915, 3255, 1363, 4351, 928, 2303, 5627, 5596, 7866, 1004, 7479, 5859, 587, 7559, 4630, 5935, 3584, 5272, 3666, 531, 2180, 4716, 725, 3603, 106, 1376, 1353, 2968, 1113, 3102, 4501, 4329, 5950, 4505, 6865, 4113, 6627, 5855, 3053, 3336, 4893, 925, 3466, 3611, 3615, 5201, 2219, 3735, 1243, 7235, 41, 7112, 3877, 859, 4916, 6638, 1978, 1268, 302, 4343, 2641, 4529, 2387, 6265, 4892, 2391, 4467, 5380, 1920, 425, 5325, 2151, 1811, 5280, 356, 7694, 3154, 1136, 2097, 6595, 433, 27, 6949, 3850, 7448, 1951, 5435, 6508, 3153, 7091, 5893, 6556, 4375, 4458, 3947, 5292, 364, 7661, 766, 544, 7211, 5617, 96, 2866, 619, 2248, 2871, 1301, 3193, 854, 7033, 6056, 555, 464, 4139, 317, 2911, 1782, 7006, 3085, 2742, 6676, 5433, 4255, 3511, 4862, 2742, 7669, 1664, 843, 3816, 958, 5829, 3151, 1493, 5821, 3954, 6051, 1868, 4792, 5924, 1075, 689, 7120, 37, 2033, 1913, 4513, 1900, 3302, 6853, 4732, 3201, 151, 7555, 7104, 4524, 482, 641, 6820, 7735, 3457, 6591, 5675, 7981, 3025, 6428, 4701, 4613, 3261, 7225, 5050, 5999, 6678, 290, 1960, 6275, 5078, 106, 795, 5966, 29, 6166, 151, 763, 2913, 3621, 7094, 7960, 913, 7106, 1941, 1262, 6925, 758, 1179, 2856, 5103, 90, 7274, 2559, 4370, 1699, 4995, 1348, 7815, 1912, 5269, 3438, 7094, 2858, 3153, 1909, 3258, 5021, 1946, 5138, 3553, 3544, 4297, 6144, 5271, 487, 7134, 4696, 2005, 3265, 4332, 7342, 7913, 3019, 2566, 194, 7190, 6849, 7783, 4037, 3364, 4818, 1165, 5569, 2919, 7328, 4465, 2266, 1299, 2443, 48, 2433, 4107, 7069, 2655, 5100, 3366, 6716, 4556, 3314, 6592, 3247, 76, 198, 3521, 4158, 3227, 4368, 4882, 3996, 1793, 6661, 4416, 5952, 3985, 2365, 7577, 1164, 5533, 7949, 4343, 6463, 3964, 7024, 663, 2456, 3928, 6984, 2119, 3634, 5863, 1060, 5646, 6953, 417, 5376, 6379, 5772, 4090, 904, 7267, 3620, 7115, 6733, 4054, 152, 2198, 3870, 352, 4690, 166, 7606, 7521, 3239, 1361, 268, 7076, 7085, 3261, 5379, 4221, 6403, 7550, 2379, 4105, 5359, 2151, 3390};
	if (input->ANN != NULL)
 	{
 		if (!input->silent && input->display) {
 			printf("\nANN:\n");
 			for (i = 0; i < input->nq; i++) {
				printf("query #%d:", i);
				for (j = 0; j < input->knn; j++)
					printf(" %d", input->ANN[i*input->knn+j]);

				printf("\n");
 			}
 		}

		if(TEST==1)
		for(int i=0;i<input->nq;i++){
			float ottimo=dist(input->qs,input->ds,i*input->d,(a[i]-1)*input->d,input->d);
			for(int j=0;j<input->knn;j++){
			float d=dist(input->qs,input->ds,i*input->d,input->ANN[i*input->knn+j]*input->d,input->d);
			printf("%1.3f\t",ottimo-d);
			}
			printf("\n");
		}
 		save_ANN(input->filename, input->ANN, input->nq, input->knn);
	}

	if (!input->silent)
		printf("\nDone.\n");

	return 0;

}
void printMatrix(MATRIX m, int r, int c){
for (int i = 0; i < r; i++) {
	for(int j=0;j<c;j++){
			printf("%1.1f,\t",m[i*c+j] );
	}
		printf("\n");
	}
}
