# Simulador_de_Cache__Cache_Simualtor__C

----------------- --------------------------- -----------------
----------------- Simualador de Memoria Cache -----------------
----------------- --------------------------- -----------------

## Se adjunta un fichero make
    Compilazión: $ make crea


## Compilación de las trazas
    gcc -Wall -o traza_2 traza_2.c
    gcc -Wall -o traza traza.c
    /* esto lo hace el make file en play# */


## Ejecucion:
    make play1
    o
    make play2

## Resumen:
    Se ha realizado la oraganición de la memoria mediante una
    politica LRU (Least Recently Use), junto a una memoria
    asociativa por conjuntos


## El simulador consta de 9 funciones:

#### int Simualador:
  - Esta función realiza la lectura de los datos del fichero de traza y administra la memoria cache.

#### int adminMemo:
   - Esta función realiza la administración de la memoria cache y la victim Cache. Realizando las peticiones.

#### int insertaMemoCache:
   - Esta función realiza la inserción de datos en la memoria cache

#### int insertaVictim;
   - Esta función realiza la inserción de datos en la victim cache

#### int esta_datoVCache:
   - Esta función realiza la busqueda del dato den la victim cache

#### int hayHueco:
   - Esta función busca si hay hueco dentro del grupo de la memoria cache

#### int leeConfig:
   - Esta función realiza de la configuración del sistema.

#### int esta_datoMCache:
   - Esta función busca si el dato esta dentro del grupo de la memoria cache.

#### int inicaCache:
   - Esta función inicializa las caches a cero
