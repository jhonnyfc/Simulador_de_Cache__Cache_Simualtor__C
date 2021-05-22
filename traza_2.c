#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX	1024
#define ROW 64

//gcc -Wall -o traza_2 traza_2.c

int main(){
	double v[ROW][MAX];
	double res;
	srand(time(0));
	for (int i=0;i<ROW;++i){
		for (int j=0; j < MAX; ++j)
			v[i][j]=rand()%1000;
	}

	FILE *f;
	f=fopen("traza.txt","w");
	for (int j = 0; j < MAX; ++j){
		for (int i=0 ;i< ROW; i++) {
			//res=res+v[i][j];
			fprintf(f, "%p ", &res);
			fprintf(f, "%p ", &v[i][j]);
			//fprintf(f, "%p ", &res);
		}
		//fprintf(f, "\n ");
	}
	fclose(f);
}
