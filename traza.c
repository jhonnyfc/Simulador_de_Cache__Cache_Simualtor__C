#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 50

//gcc -Wall -o traza traza.c

int main() {
	double v[MAX];
	double res;
	double other = 0;
	srand(time(0));
	for (int i=0;i<MAX;++i){
		v[i]=rand()%100;
	}
	FILE *f;
	f=fopen("traza.txt","w");
	for (size_t i = 0; i < 2; i++)
	for (int i=0 ;i<MAX ; i++) {
		res=res+v[i];
		other += res;
		fprintf(f, "%p\n", &res);
		fprintf(f, "%p\n", &v[i]);
		fprintf(f, "%p\n", &res);
		fprintf(f, "%p\n", &other);
	}
}
