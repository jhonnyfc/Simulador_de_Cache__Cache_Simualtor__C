#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define COL 2
#define COLVC 2
int 	hayVCa;
float 	numFallos;
float 	peticiones;
int 	nlinGrupo;
int		bitTag[2];

int intro();
int simulador(FILE *traza, int numLineas, unsigned long int MemoriaCache[][numLineas], int bitGrupo[],int vcLines, unsigned long int VictimCache[][vcLines]);
int adminMemo(unsigned long int direccionmm, int numLineas, unsigned long int MemoriaCache[][numLineas], int bitGrupo[],int vcLines, unsigned long int VictimCache[][vcLines]);
int insertaMemoCache(int numLineas, unsigned long int MemoriaCache[][numLineas],unsigned long int direccionmm,int inicio,int offset);
int insertaVictim(int vcLines, unsigned long int VictimCache[][vcLines],unsigned long int dato);
int esta_datoVCache(unsigned long int direccionmm, int vcLines, unsigned long int VictimCache[][vcLines],int bitGrupo[]);
int hayHueco(int grupo, int numLineas, unsigned long int MemoriaCache[][numLineas]);
int leeConfig(int *numLineas,int *tamLin, int *nlinGrupo, int *vcLines, FILE *f);
int esta_datoMCache(unsigned long int direccionmm,int grupo, int numLineas, unsigned long int MemoriaCache[][numLineas]);
int inicaCache(int numLineas, unsigned long int MemoriaCache[][numLineas]);
unsigned long int rangobits(unsigned long int n, int bitmenor, int bitmayor);

int main(int argc, char *argv[]){
	int 	numLineas;
	int  	tamLin;
	int 	vcLines;
	int 	bitGrupo[2];

	intro();
	//Leemos datos de la chache
	FILE *f;
	if ((f = fopen("config.txt","r")) == NULL){
		perror("No se ha encontrado fichero de configuracion\n");
		exit(-1);
	}
	leeConfig(&numLineas,&tamLin,&nlinGrupo,&vcLines,f);
	fclose(f);
	if (vcLines == 0) hayVCa = 0;
	else hayVCa =1;

	//Desigmanos bits que dan el grupo
	int auxBit;
	if (nlinGrupo >= 1)//Selecionamos rango de grupo
		auxBit 	= log(numLineas/nlinGrupo)/log(2);

	int auxBitlin 	= log(tamLin)/log(2);//nuemro de bits que necesitamos para designar el BYTE
	bitGrupo[0] 	= auxBitlin;
	bitGrupo[1] 	= auxBitlin + auxBit - 1;

	//Asigancion bits TAG
	bitTag[0] = bitGrupo[1]+1;
	bitTag[1] = 47;

	//Inicio MC
	unsigned long int MemoriaCache[COL][numLineas];
	inicaCache(numLineas,MemoriaCache);

	//Inicio VC
	unsigned long int VictimCache[COLVC][vcLines];
	inicaCache(vcLines,VictimCache);

	//Simulacio
	printf("\n");
	printf("Empezamos la Simulacion...\n");
	FILE *traza;
	if ((traza = fopen("traza.txt","r")) == NULL){
		perror("No se ha encontrado fichero de  traza\n");
		exit(-1);
	}
	simulador(traza,numLineas,MemoriaCache,bitGrupo,vcLines,VictimCache);
	fclose(traza);
	return 0;
}


int simulador(FILE *traza, int numLineas, unsigned long int MemoriaCache[][numLineas], int bitGrupo[],int vcLines, unsigned long int VictimCache[][vcLines]){
	unsigned long int direccionmm;
	numFallos = 0;
	peticiones = 0;

	printf("\n");
	while (fscanf(traza,"%lx ",&direccionmm) != EOF){
		//fgets(buffer,100,traza);
		//sscanf(buffer,"%p ",&direccionmm);
		adminMemo(direccionmm,numLineas,MemoriaCache,bitGrupo,vcLines,VictimCache);
	}
	printf("El numero de fallos es: %2.f\n", numFallos);
	printf("El nuemro de peticiones es: %2.f\n", peticiones);

	printf("La tasa de fallos es %5.2f %% \n", numFallos/peticiones * 100);
	return 1;
}

int adminMemo(unsigned long int direccionmm, int numLineas, unsigned long int MemoriaCache[][numLineas], int bitGrupo[],int vcLines, unsigned long int VictimCache[][vcLines]){
	int lineaHueco 		= 0;
	int grupo 			= rangobits(direccionmm,bitGrupo[0],bitGrupo[1]);
	int lineBus 		= (nlinGrupo*grupo) % numLineas;
	peticiones++;

	if (esta_datoMCache(direccionmm,grupo,numLineas,MemoriaCache) == 1){
		//EL dato esta en la MCache
		return 1;
	}else if (hayVCa == 0){													//NO hay VCAche
		if ((lineaHueco = hayHueco(grupo,numLineas,MemoriaCache)) > -1){
			//SI hay hueco en la cache
		   	//Bajamos una posocion e insetramos en la memoria cahce  con normalidad
			insertaMemoCache(numLineas,MemoriaCache,direccionmm,lineaHueco,lineBus);
	   }else{
		   	//NO hay heuco en la cahe
			insertaMemoCache(numLineas,MemoriaCache,direccionmm,lineBus+nlinGrupo-1,lineBus);
		}
		numFallos++;
	}else{																	//SI hay VCAche
		if (esta_datoVCache(direccionmm,vcLines,VictimCache,bitGrupo) == 0)
			numFallos++;//El dato NO esta en la VC sumamos fallo

		if ((lineaHueco = hayHueco(grupo,numLineas,MemoriaCache)) > -1){
			//SI hay hueco en la cache
		   	//Bajamos una posocion e insetramos en la memoria cahce  con normalidad
			insertaMemoCache(numLineas,MemoriaCache,direccionmm,lineaHueco,lineBus);
	   }else{
		   	//NO hay heuco en la cahe y lo guardamos en la VictimCache
			insertaVictim(vcLines,VictimCache,MemoriaCache[1][lineBus+nlinGrupo-1]);//dato insertado en a VC
			insertaMemoCache(numLineas,MemoriaCache,direccionmm,lineBus+nlinGrupo-1,lineBus);
		}
	}
		return  1;
}

int insertaMemoCache(int numLineas, unsigned long int MemoriaCache[][numLineas],unsigned long int direccionmm,int inicio,int offset){
	for (size_t i = inicio; i > offset; i--) {
		MemoriaCache[1][i] = MemoriaCache[1][i-1];
		MemoriaCache[0][i] = 1;
	}
	MemoriaCache[1][offset] = direccionmm;//simepre insertamos un dato en la primera posicion de cada grupo
	MemoriaCache[0][offset] = 1;
	return 1;
}

int insertaVictim(int vcLines, unsigned long int VictimCache[][vcLines],unsigned long int dato){
	for (int i = vcLines-1; i > 0; i--) {
		//Recorremos una posicion hacia abajo todos los datos de la VictimCache
		VictimCache[1][i] = VictimCache[1][i-1];
		VictimCache[0][i] = VictimCache[0][i-1];
	}
	VictimCache[1][0] = dato;
	VictimCache[0][0] = 1;
	return 1;
}

int esta_datoVCache(unsigned long int direccionmm, int vcLines, unsigned long int VictimCache[][vcLines], int bitGrupo[]){
	for (size_t i = 0; i < vcLines; i++) {
		if ((rangobits(VictimCache[1][i],bitTag[0],bitTag[1]) == rangobits(direccionmm,bitTag[0],bitTag[1])) &&  (rangobits(VictimCache[1][i],bitGrupo[0],bitGrupo[1]) == rangobits(direccionmm,bitGrupo[0],bitGrupo[1])) ){//Si se encuentra el dato en la VC
			if (i == vcLines-1){
				//Limpimos si esta el dato en la ultima  linea
				VictimCache[1][i] = 0;
				VictimCache[0][i] = 0;
			}if (VictimCache[0][i+1] == 1){
				//Reocrremos la VictimCache -- LRU
				int j = i;
				while (j < vcLines-1 && VictimCache[0][j] == 1) {
					VictimCache[1][j] = VictimCache[1][j+1];
					VictimCache[0][j] = 1;
					j++;
				}
				VictimCache[1][j] = 0;
				VictimCache[0][j] = 0;
			}
			return 1;
		}
	}
	return 0;
}

int hayHueco(int grupo, int numLineas, unsigned long int MemoriaCache[][numLineas]){
	int lineBus = (nlinGrupo*grupo) % numLineas;
	for (int i=lineBus; i < lineBus + nlinGrupo; i++) {
		if (MemoriaCache[0][i] == 0)
			return i;
	}
	return -1;
}

int esta_datoMCache(unsigned long int direccionmm,int grupo, int numLineas,unsigned long int MemoriaCache[][numLineas]){
	int lineBus = (nlinGrupo*grupo) % numLineas;

	for (int i=lineBus; i < lineBus + nlinGrupo; i++) {
		if (rangobits(MemoriaCache[1][i],bitTag[0],bitTag[1]) == rangobits(direccionmm,bitTag[0],bitTag[1]) ){
			//Si el dato esta hay que llevarolo al top del grupo memoria cache
			for (int j = i; j < lineBus; --j){
				MemoriaCache[1][j] = MemoriaCache[1][j-1];
				MemoriaCache[0][j] = 1;
			}
			MemoriaCache[1][lineBus] = direccionmm;
			MemoriaCache[0][lineBus] = 1;
			return 1;
		}
	}
	return 0;
}

unsigned long int rangobits(unsigned long int n, int bitmenor, int bitmayor){
	unsigned long int bit2=1, bit1=1;

	if (bitmayor < 0 || bitmayor > 47) {
		printf("Error en bitmayor: %d\n", bitmayor);
		exit(0); }
	else if (bitmenor < 0 || bitmenor > 47) {
		printf("Error en bitmenor: %d\n", bitmenor);
		exit(0); }
	else if (bitmayor < bitmenor) {
		printf("Error en orden\n");
		exit(0); }
	else if(bitmayor-bitmenor+1 < 48 ) {
		/* calcula valor 2 elevado a bitmenor */
		bit1= bit1 << bitmenor;
		/* calcula valor 2 elevado a (bitmayor-bitmenor + 1) **/
		bit2=bit2 << (bitmayor-bitmenor + 1);
		n=n/bit1;
		n=n%bit2; }
	return n;
}

int inicaCache(int numLineas, unsigned long int MemoriaCache[][numLineas]){
	for (int j = 0; j < numLineas; j++){
		MemoriaCache[0][j] = 0;
		MemoriaCache[1][j] = 0;
	}
	return 1;
}

int leeConfig(int *numLineas,int *tamLin, int *nlinGrupo, int *vcLines, FILE *f){
	char aux[20];

	printf("Configuracion:\n");
	fscanf(f,"%s %d",aux,&(*numLineas));
	if (*numLineas <= 0){
		printf("ERROR en el numero de Lineas de la Memoria CACHE\n");
		exit(1);
	}
	printf("%s %d\n",aux,*numLineas );

	fscanf(f,"%s %d",aux,&(*tamLin));
	if (*tamLin < 1 ){
		printf("ERROR en el tamaño de linea\n");
		exit(1);
	}
	printf("%s %d\n",aux,*tamLin );

	fscanf(f,"%s %d",aux,&(*nlinGrupo));//Asociatividad
	if (*nlinGrupo < 1){
		printf("ERROR en el numero de lineas por conjunto de la chache\n");
		printf("      numero muy pequeño o no especificado\n");
		exit(1);
	}
	printf("%s %d\n",aux,*nlinGrupo );

	fscanf(f,"%s %d",aux,&(*vcLines));
	printf("%s   %d\n",aux,*vcLines);
	return 1;
}

int intro(){
	printf("----------------- --------------------------- -----------------\n");
	printf("----------------- Simualador de Memoria Cache -----------------\n");
	printf("-------------- Jhonny Fabricion Chicaiza Palomo ---------------\n");
	printf("----------------- --------------------------- -----------------\n");
	printf("\n");
	return 1;
}
