crea:
	gcc -Wall -g -o simuladorCache simuladorCache.c -lm

play1:
    gcc -Wall -o traza traza.c
	./traza
	./simuladorCache

play2:
	gcc -Wall -o traza_2 traza_2.c
	./traza_2
	./simuladorCache
