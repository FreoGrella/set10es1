//CERRI, FARINELLI, GALERI, PARADISI
#ifndef SNAKEANDLADDER_H_INCLUDED
#define SNAKEANDLADDER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h> //necessario per i caratteri unicode utilizzati
#include "color.h"

#ifdef __linux__
#include <locale.h> //:^)
#elif _WIN64
#include <fcntl.h>
#include <io.h>
#endif

#define DELAY 50

#define MAPX 10
#define MAPY 10

#define DICEFACES 6
#define DICEMIN 1
#define DICEMAX (DICEFACES + DICEMIN)

//DEFINISCO LE VARIE CATEGORIE DI CASELLE DISPONIBILI DA ASSEGNARE A square.type
#define EMPTY 1
#define SKIP 2
#define STARTLADDER 3
#define ENDLADDER 4
#define SNAKEHEAD 5
#define SNAKETAIL 6

//DEFINISCO I CARATTERI DA ASSOCIARE ALLE CATEGORIE DI CASELLE NELLA RAPPRESENTAZIONE GRAFICA
#define G_SKIP          L'X'
#define G_STARTLADDER   L'┴'
#define G_ENDLADDER     L'┬'
#define G_SNAKEHEAD     L'§'
#define G_SNAKETAIL     L'┘'

#define clrscr() wprintf(L"\e[1;1H\e[2J")


typedef struct {
	short turn;
	short position;
	char skin; // le skin per i player in ascii possono essere: 2,3,4,5,6
} player;

typedef struct {
	short number;
	short type[2]; //[0] tipo (start/end ladder/snake), [1] number della casella associata tramite ladder/snake se presente
	wchar_t graphic[6];
} square;

int RollDice(void)
{
	time_t now = 0;
	int dice;

	//per l'effetto diceroll
	for (int i = 0; i < 30; i++) {
		wprintf(L"\r");
		dice = DICEMIN + rand() % DICEMAX;
		wprintf(L"%d", dice);
		now = clock();
		while ((clock() - now) < DELAY)
			continue;
	}

	now = clock();
	while ((clock() - now) < (DELAY * 25))
		continue;
	wprintf(L"\r");

	return dice;
}

//scelgo un numero fisso di scale, serpenti e skip da distribuire casualmente nella mappa
//ad ogni generazione, in questo caso: 7 scale, 6 serpenti e 10 skip
//deve essere necessariamente chiamata prima delle altre funzioni
square **MapGeneration(void)
{
	short ladder = 7, snake = 6, skip = 10;
	short number = 1;
	short randposx, randposy, temp;
	square **map;

	//INIZIO GENERAZIONE LOGICA DELLA MAPPA
	//INIZIALIZZO LA MATRICE
	map = (square **)malloc(sizeof(square *) * MAPY);
	for (int i = 0; i < MAPY; i++) {
		map[i] = (square *)malloc(sizeof(square) * MAPX);
		for (int j = 0; j < MAPX; j++) {
			map[i][j].number = number++;
			map[i][j].type[0] = EMPTY;
		}
	}

	srand(time(NULL));

	//INSERISCO LE SCALE
	do {
		randposx = rand() % MAPX;
		randposy = rand() % (MAPY - 1);
		if (map[randposy][randposx].type[0] == EMPTY) {	//SE LA CASELLA E' VUOTA PROSEGUO
			map[randposy][randposx].type[0] = STARTLADDER; //INSERISCO L'INIZIO SCALA
			do {
				temp = rand() % MAPX; //posizione x di endladder
				if (map[randposy + 1][temp].type[0] == EMPTY) { //SE LA CASELLA E' VUOTA PROSEGUO
					map[randposy + 1][temp].type[0] = ENDLADDER; //INSERISCO LA FINE DELLA SCALA
					break;
				}
			} while (1);
			map[randposy][randposx].type[1] = map[randposy + 1][temp].number; //ASSOCIO L'INIZIO DELLA SCALA CON LA SUA FINE
			ladder--;
		}
		else
			continue;
	} while (ladder > 0);

	

	//INSERISCO I SERPENTI
	do {
		randposx = rand() % MAPX;
		randposy = 1 + rand() % (MAPY - 1);
		if (map[randposy][randposx].type[0] == EMPTY) { //SE LA CASELLA E' VUOTA PROSEGUO
			map[randposy][randposx].type[0] = SNAKEHEAD; //INSERISCO LA TESTA DEL SERPENTE
			do {
				temp = rand() % MAPX; //posizione x di snaketail
				if (map[randposy - 1][temp].type[0] == EMPTY) { //SE LA CASELLA E' VUOTA PROSEGUO
					map[randposy - 1][temp].type[0] = SNAKETAIL; //INSERISCO LA CODA DEL SERPENTE
					break;
				}
			} while (1);
			map[randposy][randposx].type[1] = map[randposy - 1][temp].number; //ASSOCIO LA TESTA CON LA CODA DEL SERPENTE
			snake--;
		}
		else
			continue;
	} while (snake > 0);

	//INSERISCO GLI SKIP
	do {
		randposx = rand() % MAPX;
		randposy = rand() % MAPY;
		if (map[randposy][randposx].type[0] == EMPTY) { //SE LA CASELLA E' VUOTA PROSEUO
			map[randposy][randposx].type[0] = SKIP; //INSERISCO LO SKIP
			skip--;
		}
		else
			continue;

	} while (skip > 0);
	//FINE GENERAZIONE LOGICA DELLA MAPPA

	//INIZIO GENERAZIONE GRAFICA DELLA MAPPA

	#ifdef __linux__
	setlocale(LC_CTYPE, ""); //a quanto pare cambia il tipo di carattere della console
	#elif _WIN64
	_setmode(fileno(stdout), _O_U8TEXT); //^idem ma per windows, permette di usare UTF-8 (credo)
	#endif

	short typex;
	for (int i = MAPY - 1; i >= 0; i--) {
		for (int j = MAPX - 1; j >= 0; j--) {
			typex = map[i][j].type[0];
			switch (typex) {
			case SKIP:
				swprintf(map[i][j].graphic, sizeof(map[i][j].graphic), L" %3d%lc", map[i][j].number, G_SKIP);
				wprintf(L"%ls", map[i][j].graphic);
				break;
			case STARTLADDER:
				swprintf(map[i][j].graphic, sizeof(map[i][j].graphic), L" %3d%lc", map[i][j].number, G_STARTLADDER);
				wprintf(L"%ls", map[i][j].graphic);
				break;
			case ENDLADDER:
				swprintf(map[i][j].graphic, sizeof(map[i][j].graphic), L" %3d%lc", map[i][j].number, G_ENDLADDER);
				wprintf(L"%ls", map[i][j].graphic);
				break;
			case SNAKEHEAD:
				swprintf(map[i][j].graphic, sizeof(map[i][j].graphic), L" %3d%lc", map[i][j].number, G_SNAKEHEAD);
				wprintf(L"%ls", map[i][j].graphic);
				break;
			case SNAKETAIL:
				swprintf(map[i][j].graphic, sizeof(map[i][j].graphic), L" %3d%lc", map[i][j].number, G_SNAKETAIL);
				wprintf(L"%ls", map[i][j].graphic);
				break;
			default:
				swprintf(map[i][j].graphic, sizeof(map[i][j].graphic), L" %3d ", map[i][j].number);
				wprintf(L"%ls", map[i][j].graphic);
				break;
			}
		}
		fputws(L"\n", stdout);
	}

	return map;
}

//ANCORA NON FUNZIA NON SO COME SI USANO I DOPPI PUNTATORI ZIOCAN
void RefreshMap(square **map, player p)
{
	for (int i = MAPY - 1; i >= 0; i--) {
		for (int j = MAPX - 1; j >= 0; j--) {
			if (p.position == map[i][j].number)
				map[i][j].graphic[0] = p.skin;
			wprintf(L"%ls",map[i][j].graphic);
		}
		fputws(L"\n", stdout);
	}
}

#endif // SNAKEANDLADDER_H_INCLUDED
