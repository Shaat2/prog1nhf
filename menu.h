#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

void btn_draw(SDL_Surface *screen, Menu_btn btn);

int gombon_e(Menu_btn btn, Pont clickpos);

int melyik_gombon(Menu_btn *Menu, int gombokszama, Pont clickpos);

void menu_generate(struct Menu_btn *Menu, int gombokszama, int menu_width);

Menu_btn nyil_btn_generate(int irany /* 0=jobbra, 1=balra */,int p1_x,int p1_y,int p2_x,int p2_y, int no_hover_color);

void menu_szamol(SDL_Surface *screen, int ***eletter, Pont *size, Menu_btn *Menu, int gombokszama, int *running, clickaction click, Pont clickpos, clickaction screenpart, int *cellsize, int *timermax);

#endif // MENU_H_INCLUDED
