#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <pthread.h>

int (*value)[6];
int first[4][3], second[3][6];

void prtScreen() {
    	printf("\nMultiplication Result Matrix 4x6:\n");
    	for(int i=0; i<4; i++) {
        	for(int j=0; j<6; j++) {
            		printf("%d\t", value[i][j]);
        	}
        	printf("\n");
    	}
   	printf("\n");
    	sleep(5);
}

void *multiplyMatrices(void *arg) {
    	int sum = 0;

    	for(int i=0; i<4; i++) {
        	for(int j=0; j<6; j++) {
            	for(int k=0; k<3; k++) {
                	sum += first[i][k] * second[k][j];
            	}
            	value[i][j] = sum;
            	sum = 0;
        	}
    	}
}

void inputMatriksB() {
    	for(int i=0; i<3; i++) {
        	for(int j=0; j<6; j++) {
            		scanf("%d", &second[i][j]);
        	}
    	}
}

void inputMatriksA() {
    	for(int i=0; i<4; i++) {
        	for(int j=0; j<3; j++) {
            		scanf("%d", &first[i][j]);
        	}
    	}
}

int main(void) {

    	key_t key = 12345;

    	int shmid = shmget(key, sizeof(int[4][6]), IPC_CREAT | 0666);
    	value = shmat(shmid, NULL, 0);

	printf("Enter Matrix 4x3 ;\n");
    	inputMatriksA();
	printf("Enter Matrix 3x6 ;\n");
    	inputMatriksB();

    	pthread_t tid[4];

    	for(int i=0; i<4; i++) {
        	pthread_create(&tid[i], NULL, multiplyMatrices, NULL);
    	}
    	for(int i=0; i<4; i++) {
        	pthread_join(tid[i], NULL);
    	}

    	prtScreen();

    	shmdt(value);
}
