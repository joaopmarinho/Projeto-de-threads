#include "stdio.h"
#include "pthread.h"
#include "stdlib.h"

struct  valores{// struct para poder mandar as partes do vetor dividido
	int *array;
	int  parte1;
	int  parte2;
};


void quicksort(int arr[], int index_Inicial, int index_Final);
void* rotina(void *init){// rotina q sera executada por cada thread afim de fazer o sorting
	struct  valores *start = init;
	quicksort(start ->array , start ->parte1 , start ->parte2);// recebe cada parte do vetor q sera ordenada
	
}

void swap(int *a, int *b){

    int temp;
    temp = *a;
    *a = *b;
    *b = temp;

}

int partition(int arr[], int index_Inicial, int index_Final){//particiona o vetor para fazer o sorting

    int pivotIndex = index_Inicial + rand()%(index_Final - index_Inicial + 1); //pega um nmr aleatorio como pivo
    int pivot;
    int i = index_Inicial - 1;
    int j;

    pivot = arr[pivotIndex];//pega o pivot
    swap(&arr[pivotIndex], &arr[index_Final]);
    for (j = index_Inicial; j < index_Final; j++){

        if (arr[j] < pivot){// checa se cada elemento do vetor eh maior q o pivot, se n for rola o swap
        	i++;// segue para o proximo indice
           	swap(&arr[i], &arr[j]);
        }

    }
    swap(&arr[i + 1], &arr[index_Final]);

    return i + 1;// retorna o indice para realizar o quicksort

}

void quicksort(int arr[], int index_Inicial, int index_Final){

    int indice;

    if (index_Inicial < index_Final){
		
		pthread_t t1,t2;
    	indice = partition(arr, index_Inicial, index_Final);
		struct  valores arg1 = {arr , index_Inicial ,indice-1};
		struct  valores arg2 = {arr , indice+1 ,index_Final};
		pthread_create (&t1 , NULL , rotina , &arg1);// criacao das threads
		pthread_create (&t2 , NULL, rotina , &arg2);
		pthread_join(t1,NULL);//jjuncao das threads
		pthread_join(t2,NULL);

    }

}



int main(){

    int i;
	int tamArray;
	printf("Determine o tam do array\n");
	scanf("%d",&tamArray);
	int *arr = (int*) malloc(sizeof(int)*tamArray);
	if(arr == NULL)
		printf("erro de aloc\n");


    for (i = 0; i < tamArray; i++){//coloca nmrs aleatorios no vetor
        arr[i] = rand()%20;
		printf("%d \n",arr[i]);
	}
    quicksort(arr, 0, tamArray-1);

	printf("\n\n\n\n");

    for (i = 0; i < tamArray; i++)//printa o vetor ordenado
         printf("%d \n", arr[i]);

	free(arr);
		
    return 0;

}