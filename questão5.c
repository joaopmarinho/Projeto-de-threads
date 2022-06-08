#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#define TAM_MAX 10
int THREAD_NUM;

typedef struct elem{
    int value;
    struct elem *prox;
}Elem;

typedef struct blockingQueue{
    unsigned sizeBuffer, statusBuffer;
    Elem *head, *last;
}BlockingQueue;


pthread_cond_t condFull = PTHREAD_COND_INITIALIZER;
pthread_cond_t condEmpty = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutexBuffer;

BlockingQueue *fila;

// Função para criar a blocking queue
BlockingQueue *newBlockingQueue(int size){

    BlockingQueue *fila =(BlockingQueue*) malloc(sizeof(BlockingQueue)* 1 );
    if(fila ==NULL)printf("Deu ruim\n");
    fila[0].statusBuffer = 0 ;
    fila[0].sizeBuffer = size ;
    return  fila;

};

void *putBlockingQueue(BlockingQueue* Q, int newValue){
    //Como usaremos apenas uma fila, de acordo com os monitores, declaramos como um vetor
    //função para inserir um elemento na FIFO

    if(Q[0].statusBuffer == 0 ){
        
       Q[0].head = (Elem*)calloc(1 , sizeof(Elem));
       Q[0].last = Q->head;
        
    }
    else{
    Q[0].last->prox = (Elem*)calloc(1 , sizeof(Elem));
    Q[0].last =Q[0].last->prox ;
    }
    Q[0].last->value = newValue;
    
    Q[0].statusBuffer++;

}
int takeBlockingQueue(BlockingQueue* Q){
    //Função para retirar um elemento da FIFO
    int result;
    Elem* excl;
    excl = Q[0].head;
    result = excl->value;
    Q[0].head = Q[0].head->prox;
    excl->prox = NULL;
    Q[0].statusBuffer--;
    free(excl);

    return result;
}
/*
Basicamente é a parte do código onde fornecemos um elemento para a fila
e comunicamos através das condições com a thread consumidora
*/
void put(int i){
  pthread_mutex_lock(&mutexBuffer);
  while(fila[0].statusBuffer == fila[0].sizeBuffer) {
    printf("A fila esta cheia\n");
    // enquanto a fila estiver cheia, esperamos para que alguns elementos sejam consumidos
    // comunicando com a thread consumidora
    pthread_cond_wait(&condEmpty, &mutexBuffer);
  }
  putBlockingQueue(fila, i);
  sleep(1);
  if(fila[0].statusBuffer == 1) {
      //um sinal é enviado para a outra thread e esta fica esperando os elementos serem consumidos
      // sem exceder o limite do sizeBuffer 
    pthread_cond_signal(&condFull); }
  pthread_mutex_unlock(&mutexBuffer); 
}
//basicamente invoca a rotina da thread produtora
void *producer() {
  int i = 0;
  printf("Produtor\n");
  while(1) {
    put(i);
    printf("Produzi %d \n",i);
    ++i;
  }
  pthread_exit(NULL);
}
/*
Esta função opera de maneira análoga à put, de modo que retira os elementos da fila
menos qunado a mesma está vazia, e comunica-se com a thread consumidora através de condições
*/
int get(){
  int result;
  pthread_mutex_lock(&mutexBuffer);
 while(fila[0].statusBuffer == 0){
    printf("A fila esta vazia\n");
    // enquanto a fila estiver vazia, esperamos para que alguns elementos sejam produzidos
    // comunicando com a thread produtora
    pthread_cond_wait(&condFull, &mutexBuffer);
  }
  result = takeBlockingQueue(fila);
  if(fila[0].statusBuffer == fila[0].sizeBuffer - 1){
    //um sinal é enviado para a outra thread e esta fica esperando os elementos serem produzidos
    // caso já hajam mais elementos na fila
    pthread_cond_signal(&condEmpty);}
  pthread_mutex_unlock(&mutexBuffer);
  return result;
}
//basicamente invoca a rotina da thread consumidora
void *consumer() {
  int i,v;
  printf("Consumidor\n");
  while(1) {
    v = get();
    printf("Consumi %d  \n",v);
  }
  pthread_exit(NULL);
}

int main(){

    scanf("%d", &THREAD_NUM);
    srand(time(NULL));
    fila = newBlockingQueue(TAM_MAX);
    pthread_t th[THREAD_NUM];

    pthread_mutex_init(&mutexBuffer, NULL);


    int i;

    //para o caso de ser mais de uma fila, temos que alocar um certo número de filas
    // e repartir igualmente as threads entre elas, ou não, apenas atribuir consumidores e produtores
    

    //solução para criar uma um par produtor/ consumidor nas iterações
    for (i = 0; i < THREAD_NUM; i++) {
        if (i % 2 == 0) {
            if (pthread_create(&th[i], NULL, &producer, NULL) != 0) {
                perror("Failed to create thread");
            }
        } else {
            if (pthread_create(&th[i], NULL, &consumer, NULL) != 0) {
                perror("Failed to create thread");
            }
        }
    }

    //liberação das threads
    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }

    //liberação das variáveis de controle de conflitos
    pthread_cond_destroy(&condEmpty);
    pthread_cond_destroy(&condFull);
    pthread_mutex_destroy(&mutexBuffer);
    pthread_exit(NULL);
    return 0;
}