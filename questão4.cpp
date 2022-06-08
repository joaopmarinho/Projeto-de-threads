
#include "bits/stdc++.h"
#include "stdlib.h"
#include "pthread.h"
#include "stdio.h"


using namespace std;
#define NUM_ARESTAS 3;
#define NUM_VERTICES 5;
#define TAM_BUFFER 4;

vector<vector<int>> lista_Vizinho;
vector<int>flag_Visitado,buffer(4);
pthread_cond_t encher = PTHREAD_COND_INITIALIZER;
pthread_cond_t vazio = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexXD = PTHREAD_MUTEX_INITIALIZER;

int arestas = NUM_ARESTAS
int vertices = NUM_VERTICES;
int ultimo_Index = 0, primeiro_Index= 0, in_Buffer = 0;

void insert_Buffer(int num){// funcao para adicionar no buffer q guardara os itens no buffer

while(in_Buffer == 4)pthread_cond_wait(&vazio, &mutexXD);

buffer[ultimo_Index] = num;// adiciona o item a ultima posicao do buffer
ultimo_Index++;// adiciona a proxima posicao no buffer
in_Buffer++;// aumenta a qtd de itens do buffer

if(ultimo_Index == 4)ultimo_Index = 0;

if(in_Buffer == 1)pthread_cond_signal(&encher);

pthread_mutex_unlock(&mutexXD);
/*basicamente é a parte do codigo que insere no buffer os itens em questão, e dependendo da quantidade de intens presentes ja no buffer, ele aciona as condicionais vazio e encher.*/

}

void dfs(int starting_Vertice){//funcao da dfs propriamente dita 
    
    for(auto cont = lista_Vizinho[starting_Vertice].begin(); cont != lista_Vizinho[starting_Vertice].end();cont++){// percorre a o grafo todo
        
        pthread_mutex_lock(&mutexXD);
        
        if(!flag_Visitado[*cont])insert_Buffer(*cont);//se n tiver a flag de visitado, colocaremos o elemento em questao no buffer
        
        else pthread_mutex_unlock(&mutexXD);
    }
}
void *rotina(void *id){// rotina que sera executada por cada thread
    
    int thread_Id = *(int *)id; //vetor que sera criado para indicar o id de cada thread
    for(int cont= thread_Id; cont < vertices; cont += 2){// for para retirar itens do buffer para serem percorridos pelas threads, 2 = nmr de threads
        
        int got_Buffer;
        
        pthread_mutex_lock(&mutexXD);
        while(in_Buffer == 4)pthread_cond_wait(&encher, &mutexXD);// condicional para aguardar a retirada do buffer pela thread
        got_Buffer = buffer[primeiro_Index];// retira o item do buffer
        in_Buffer--;// diminui a qtd de itens no buffer
        primeiro_Index++;// passa o indice do primeiro index para o proximo
        flag_Visitado[got_Buffer] = 1;// coloca a flag de visitado para determinado item

        if(primeiro_Index == 4)primeiro_Index = 0;

        if(primeiro_Index == 4 - 1)pthread_cond_signal(&vazio);

        pthread_mutex_unlock(&mutexXD);

        cout << "thread: " << thread_Id << " " <<  "visitou node: " << primeiro_Index << endl;
        dfs(primeiro_Index);
        cout << endl;
    }
    pthread_exit(NULL);
}

int main(){

    int verticeA, vizinhoA;

    cout << "digite os vertices e os vizinhos"<< endl;//inicializacao dos vertices e dos seus vizinhos
    
    
    flag_Visitado.resize(vertices + 1);
    lista_Vizinho.resize(vertices + 1);// aumentar o tamanho do grafo e do vetor de visitado

    for(arestas; arestas >= 0; arestas--){//inicializacao dos vertices e dos seus vizinhos

        cin >> verticeA >> vizinhoA;
        lista_Vizinho[verticeA].push_back(vizinhoA);
    }
    insert_Buffer(0);//adicionar o primeiro elemento no buffer

    pthread_t thread[2];//inicializar as threads
    int *thread_Id[2];

    for(int cont = 0; cont < 3; cont++){
        
        thread_Id[cont] = new int;//alocar o vetor de id de threads
        *thread_Id[cont] = cont;
        pthread_create(&thread[cont], NULL, rotina, (void*)thread_Id[cont]);//botar cada thread para executar parte do dfs
    }

    for(int cont = 0; cont < 3; cont++)pthread_join(thread[cont], NULL);//juntar as info da execucao das threads
        
    pthread_exit(NULL);
}
