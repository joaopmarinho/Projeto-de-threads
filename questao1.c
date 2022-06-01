#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

pthread_mutex_t abrirArquivo = PTHREAD_MUTEX_INITIALIZER;
int numArq;
int numThread;
int produtos;
pthread_mutex_t *mutex;
int *estante = NULL;
int *indice;

void *rotina(void *init){
    int aux;
    for(int k = *((int*)init) ; k < numArq ; k += numThread){
        // basicamente criei duas strings para concatenar antes da leitura do arquivo
        // a primeira string indicando o diretório e a segunda string indicando o tipo do arquivo
        char string[30];
        char string2[4];
        strcpy(string2, ".in");
        char text[10];
        
        strcpy(string, "../threads/arquivos/");
        aux = indice[*((int*)init)] + 1;
        sprintf(text, "%d", aux);
        strcat(string, text);
        strcat(string, string2);

        // para a análise do número do arquivo temos que cada thread inicializada vai pegar um número
        // Por exemplo: 3 threads (t0, t1 e t2) pegam os arquivos 1, 2 e 3
        // nas iterações subsequentes as threads vão tratar do arquivo análogo, separado estaticamente
        // ou seja, thread 0 pega os arquivos: 1, 1+3, 1+3+3, 1+3+3+3, até percorrer ou chegar no num total
        // de arquivos 
        FILE *ptr = NULL;
        ptr = fopen(string, "r");
        int i = 0;
        // criamos um vetor de mutexes, adequado para a quantidade de produtos, cada mutex trata de um produto
        // separadamente, portanto não precisamos de tratar o vetor todo como uma região crítica, mas apenas uma
        // parte do vetor associada ao index, ou seja, se modificarmos um produto, não poderemos modificá-lo 
        // paralelamente, mas poderemos modificar as outras posições do vetor
        // Ao incrementar a quantidade de produtos 1 com uma thread, não poderemos incrementar o produto 1 na
        // thread 2, mas poderemos incrementar o número de produtos 3 ou 4 por exemplo.
        while(fscanf(ptr, "%d", &i) == 1){
            pthread_mutex_lock(&mutex[i]);
            estante[i]++;
            pthread_mutex_unlock(&mutex[i]);
        }   
        fclose(ptr);
        indice[*((int*)init)] += numThread;
    }
}

int main(){
    //chamando a quantidade de itens pelo usuário
    scanf("%d", &numArq); // quantidade de arquivos total
    scanf("%d", &numThread); // número de threads a serem utilizadas
    scanf("%d", &produtos); // estante de produtos

    //alocando e declarando itens e threads
    
    int *ids[numThread];
    int j = 0;
    pthread_t threads[numThread];
    indice = (int*) malloc(sizeof(int)*numThread);
    mutex = (pthread_mutex_t*) malloc ( sizeof(pthread_mutex_t)*produtos);
    estante = (int*)calloc(produtos, sizeof(int));

    //inicializando o vetor de mutexes
    for(int i = 0; i< produtos; i++){
        pthread_mutex_init(&mutex[i], NULL);
    }

    //criando threads
    for(j=0; j < numThread; j++){
        ids[j] =(int *) malloc(sizeof(int));
        indice[j] = j;
		*ids[j] = j;
        pthread_create(&threads[j], NULL, rotina, (void*)ids[j]);
    }

    //liberando as threads e mutexes depois de seu término
    for(j = 0; j<numThread; j++){
        pthread_join(threads[j], NULL);
    }

    for(int i = produtos-1; i >= 0; i--){
        pthread_mutex_destroy(&mutex[i]);
    }
    // Printando quantidades de produtos lidos nos arquivos
    for(int i = 0; i < produtos; i++){
        printf("Produto %d = %d \n", i, estante[i]);
    }

    for(int i = 0; i< numThread ; i++){
        free(ids[i]);
    }
    free(estante);
    pthread_exit(NULL);
    return 0;
}