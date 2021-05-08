#include <stdio.h>
#include <string.h>

int main() {
  int m1[10][10], m2[10][10], m3[10][10];
	memset(m1, 0, sizeof m1);
	memset(m2, 0, sizeof m2);
	memset(m3, 0, sizeof m3);

	printf("Enter Matrix 4x3 :\n");
	for(int i=0 ; i<4 ; i++) {
		for(int j=0 ; j<3 ; j++) scanf("%d", &m1[i][j]);
	}
	printf("\nEnter Matrix 3x6 :\n");
	for(int i=0 ; i<3 ; i++) {
                for(int j=0 ; j<6 ; j++) scanf("%d", &m2[i][j]);
        }
	for(int i=0 ; i<4 ; i++) {
                for(int j=0 ; j<6 ; j++) {
			m3[i][j] = 0;
			for(int k=0 ; k<6 ; k++) m3[i][j] += (m1[i][k] * m2[k][j]);
		}
        }
	printf("\nMultiplication Result Matrix 4x6:\n");
	for(int i=0 ; i<4 ; i++) {
                for(int j=0 ; j<6 ; j++) {
			printf("%d\t", m3[i][j]);
		}
		printf("\n");
        }
	return 0;
}
