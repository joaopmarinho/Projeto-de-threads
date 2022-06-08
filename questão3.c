#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#define COLUNAS 2
#define LINHAS 2
#define P 10 
#define INC 2

pthread_t *threads;
pthread_barrier_t barrier;
int n_threads;

int **AlocaMatriz(int m, int n){
  int **M;
  int i;
 
  M = (int **)malloc(sizeof(int *)*m);
  if(M == NULL){
    printf("Memoria insuficiente.\n");
    exit(1);
  }
  for(i = 0; i < m; i++){
    M[i] = (int *)malloc(sizeof(int)*n);
    if(M[i] == NULL){
      printf("Memoria insuficiente.\n");
      exit(1);
    }
  }
  return M;
}
int MatXY[2][2] = { { 2, 1 }, { 5, 7 } } ;//matriz dos coef
int res[2] = {11,13};//matriz dos resultados das eq;

int **idt;//matriz que sera alocada identificanco com que variavel ficara cada thread;
double K[P + 1][INC];//matriz que aloca sol parciais
int *kont ;//contador de thread 

pthread_barrier_t barrier;

double somatorio(int i , int C){ //somatorio do algoritmo

    double soma = 0;

    for(int j = 0 ; j < COLUNAS ;j++){

        if(i != j) {

            soma += ((double)MatXY[i][j])*K[C - 1][j];
           
        }
    }
    return soma;
}


 void *sol(void *threadid) {
    int id = *((int*)threadid);
    printf("Usando a thread %d \n", id);
    while(kont[id] != P){
        for(int j = 0 ; j < INC; j++){

            if(idt[id][j] == 1){


                double aii = (1/(double)MatXY[j][j]);
                double bi =  (double) res[j];
                double soma = somatorio(j, kont[id]);
                K[kont[id]][j] = aii*(bi - soma);


            }   
        }
        pthread_barrier_wait(&barrier);
    kont[id]++;
    }
 } 

 int main(){

     printf("Digite o número threads \n");
     scanf("%d",&n_threads);

    

     if(n_threads > INC){

         
         n_threads = INC;
         printf("NÃO FAZ SENTIDO !!! então vamos botar %d threads\n",n_threads);
         

     }
    threads = (pthread_t*)malloc(sizeof(pthread_t) * n_threads);
     
     
     pthread_barrier_init(&barrier, NULL, n_threads);
     

     idt = AlocaMatriz(n_threads, INC);

     kont = (int*)calloc(n_threads,sizeof(int));

      for(int j = 0 ; j < n_threads ; j++){

            kont[j] = 1;
        }
     

    for(int i = 0 ; i < n_threads ; i++){ //zera a matriz de identificação

        for(int j = 0 ; j < INC ; j++){

            idt[i][j] = 0;
        }
     }

     int  dist = INC/n_threads ; //distribuição n exata das incognatas para threads;
     int  val_livre = INC - (n_threads*dist) ; //cuidadando das variaveis que sobram ,essas serão geridas pela ultima thread;
     
     int col = 0;
     int i;

     for(i = 0 ; i < n_threads ; i++){

         int cont = 0;

         if(i != (n_threads - 1)){

            while( cont < dist){
                
                idt[i][col] = 1;
                col++;
                cont++;
            }
         }
         else {
                 while( col <= INC){
                
                    idt[i][col] = 1;
                    col++;
                }

            }
    
         }


        for(int i = 0 ; i < P + 1 ; i++){ //inicia a matriz solução

            for(int j = 0 ; j < INC ; j++){

            K[i][j] = 1.0;  
            }
        }

        int **id =(int **)malloc(sizeof(int *)* n_threads);

        for(int i = 0; i < n_threads; i++){

            id[i] = (int *)malloc(sizeof(int));
            *(id[i]) = i;
             pthread_create(&threads[i], NULL, sol, (void *)id[i]);
        
        }
      
        for(int i = 0; i < n_threads ; i++){
        pthread_join(threads[i], NULL);
        } 

    

    pthread_barrier_destroy(&barrier);


    for(int j = 0; j < INC; j++){

        printf("X%d = %lf\n", j, K[P-1][j]);
    }

    
    free(threads);
    free(kont);
    pthread_exit(NULL);
     return 0; 
}