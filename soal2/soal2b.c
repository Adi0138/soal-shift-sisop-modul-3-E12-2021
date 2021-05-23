#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define x 4
#define z 6

pthread_t tid[x*z];
pthread_attr_t attr;
int iret[x*z];
key_t key = 12345;
int *matrix;
int cnt=0;
int matrixA[x][z];
int matrixB[x][z];
int total=x*z;

void *faktorial(void *arguments);

struct arg_struct{
    	int arg1;
    	int arg2;
};

void assign_matrixA(){
    	cnt=0;
    	for(int i=0; i<x; i++){
        	for(int j=0; j<z; j++){
            		matrixA[i][j] = matrix[cnt];
            		cnt++;
        	}
    	}
}

void call_thread(){
    	for(int i=1; i<x+1; i++){
        	for(int j=1; j<z+1; j++){
            		struct arg_struct *args = (struct arg_struct *) malloc(sizeof(struct arg_struct));
            		args->arg1 = i-1;
            		args->arg2 = j-1;
            		pthread_attr_init(&attr);
            		iret[cnt] = pthread_create(&tid[cnt], &attr, faktorial, args);
            		if(iret[cnt]){
                		fprintf(stderr,"Error - pthread_create() return code: %d\n", iret[cnt]);
                		exit(EXIT_FAILURE);
            		}
            		pthread_join(tid[cnt], NULL);
            		cnt++;
        	}
        	printf("\n");
    	}

    	for(int i=0; i<total; i++){
        	pthread_join(tid[i], NULL);
    	}
}

int main(void){
    	int shmid = shmget(key, sizeof(matrix), IPC_CREAT | 0666);
    	matrix = shmat(shmid, 0, 0);

    	assign_matrixA();
    	cnt=0;
    	//input matrixB
	    printf("Enter Matrix B 4x6 :\n");
    	for (int i = 0; i < x; i++) {
      		for (int j = 0; j < z; j++) {
         		scanf("%d", &matrixB[i][j]);
      		}
   	  }  
	    printf("Matrix Result 4x6 :\n");
    	call_thread();

    	shmdt(matrix);
    	shmctl(shmid, IPC_RMID, NULL);

    	return 0;
}

void *faktorial(void *arguments){
        struct arg_struct *args = arguments;

        int baris=args->arg1;
        int kolom=args->arg2;
        long long int hasil=1;

        if (matrixA[baris][kolom] == 0 || matrixB[baris][kolom] == 0){
                printf("0");
        }
        else if(matrixA[baris][kolom]>matrixB[baris][kolom]){
        //a!/(a-b)!
                int batas = matrixA[baris][kolom] - matrixB[baris][kolom];
                for (int i = matrixA[baris][kolom]; i > batas; i--){
                        hasil = hasil*i;
                }
                printf("%lld", hasil);

        }
        else if(matrixB[baris][kolom]>matrixA[baris][kolom]){
        //a!
                for (int i = matrixA[baris][kolom]; i > 0; i--){
                        hasil = hasil * matrixA[baris][kolom];
                        matrixA[baris][kolom]--;
                }
                printf("%lld", hasil);
        }
        printf("\t\t");
        pthread_exit(0);
}
