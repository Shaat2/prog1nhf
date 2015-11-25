#ifndef ELETTER_H_INCLUDED
#define ELETTER_H_INCLUDED
#include "life_typedefs.h"

int** eletter_create(Pont size);

void eletter_free(int **eletter);

void eletter_nullaz(int **eletter, Pont size);

void eletter_draw(SDL_Surface *screen,int **eletter,Pont screensize, Pont size,int cellsize, int menu_width, int color, int correction);

int szomszedok_szamol(int **eletter, int y, int x);

void eletter_szamol(int **eletter, Pont size);

Pont eletter_koord(Pont size, Pont screensize,int menu_width, int cellsize, Pont clickpos, int correction);

void eletter_masol(int **dest, int **src, Pont sourcesize);

void eletter_save(int **eletter, Pont size);

void eletter_load(int ***eletter, Pont *size, const char *filename);
#endif // ELETTER_H_INCLUDED
