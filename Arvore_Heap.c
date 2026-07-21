#include <stdio.h>

void swap(int* arr, int i, int  j){
 int tmp;
 tmp=arr[i];
 arr[i]=arr[j];
 arr[j]=tmp;
}

void max_heapify(int Arr[], int i, int N){
 int largest;
 int left = 2*i;
 int right = 2*i + 1;

         if(left < N && Arr[left] > Arr[i]){
        largest = left;
        }

         else{
          largest = i;
            if(right < N && Arr[right] > Arr[largest]){
               largest = right;
            }
            if(largest != i){
                swap(Arr,i,largest);
                max_heapify(Arr, largest , N);
                }
        }
}


void build_maxheap(int Arr[ ], int N)
{
   for(int i =  N/2; i>=1; i--){
	max_heapify(Arr, i, N);
	}
} 

void heap_to_graphviz(int Arr[], int N, const char* nomearq){
    int i;
    FILE* arquivo;
    arquivo=fopen(nomearq,"wt");
    fprintf(arquivo,"graph G{\n");
    for (i=1;i<N;i++){
    int left = 2*i;        
    int right = 2*i+1;  
    if (left<N)fprintf(arquivo,"%d -- %d\n",Arr[i],Arr[left]);
    if (right<N)fprintf(arquivo,"%d -- %d\n",Arr[i],Arr[right]);
    }
    fprintf(arquivo,"}\n");
    fclose(arquivo);
}

int main()
{

    // A primeira posição contém zero pois não é usada. Poderíamos utilizar um 
    // remapeamento dos índices do array mas optaremos por esta abordagem por maior simplicidade
    // dot heap.dot -Tsvg -o heap.svg

    int arr[]={0,42,5,4,6,3,7,2,8,12,13,14,1,9,10};
    int N=15;
    int i;
    for (i=0;i<N;i++)
    printf("%d ",arr[i]);
    printf("\n");
    build_maxheap(arr, N);
    for (i=0;i<N;i++)
    printf("%d ",arr[i]);
    printf("Max val: %d\n",arr[1]);
    heap_to_graphviz(arr,N,"heap1.dot");
    arr[1]=arr[N-1];
    N--;
    build_maxheap(arr, N);
    heap_to_graphviz(arr,N,"heap2.dot");
    return 0;
}
