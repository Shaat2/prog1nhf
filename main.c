#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include "eletter.h"
#include "life_typedefs.h"
#include "menu.h"

Uint32 menu_ticker(Uint32 ms, void *param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;
}
int main(int argc, char *argv[]) {
    SDL_Event event;
    SDL_Surface *screen;
    int **eletter;

    Pont size={100,100}; //alapertelmezett ertekek
    int cellsize=10;  //ez minimum 4 kell legyen, az alatt mar nagyon lassu
    int cell_color=0x5555ffff;

    int correction=3;  // a szelen hany sor legyen, amit nem rajzol ki (a szomszedok szamolasanal a szelso cellakat nem lehet rendesen kiszamolni, ha ez kisebb mint 3)

    size.x+=correction*2;  //mindkét széléből le kell venni ennyit
    size.y+=correction*2;

    eletter=eletter_create(size);

    int menu_width=200;  // más értékekkel is működik, de 200-ra lett optimalizával a program

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

    Pont screensize;
    screensize.x=1024;
    screensize.y=768;

    screen=SDL_SetVideoMode(screensize.x, screensize.y, 0,  SDL_ANYFORMAT);
    if (!screen) {
        fprintf(stderr, "Nem sikerult megnyitni az ablakot!\n");
        exit(1);
    }
    SDL_WM_SetCaption("Game of Life", "Game of Life");
    SDL_FillRect(screen, 0, 0xffffffff);
    // első rajzolások
    eletter_nullaz(eletter, size);
    eletter_draw(screen, eletter, screensize, size, cellsize, menu_width, cell_color, correction);
    // menu készítése
    Menu_btn Menu[16];
    int gombokszama=16;
    menu_generate(Menu, gombokszama, menu_width);
    //az élettér sebessége
    int timer=0;
    int timermax=10; //alapértelmezett érték (0 és 60 között lehet)

    SDL_TimerID menu_tick;
    menu_tick= SDL_AddTimer(10, menu_ticker, NULL);

    int quit=0;
    Pont clickpos={0,0};
    Pont clickcell={0,0};
    clickaction click=none;
    clickaction screenpart=none;
    int running=0;

    while (!quit){
        SDL_WaitEvent(&event);
        switch(event.type){
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button==SDL_BUTTON_LEFT){
                    if (event.button.x>=menu_width){
                    click=rajzol;
                    clickpos.x=event.button.x-menu_width;
                    clickpos.y=event.button.y;
                    }
                    else {
                    click=menu;
                    clickpos.x=event.button.x;
                    clickpos.y=event.button.y;
                    }
                }
                else if (event.button.button==SDL_BUTTON_RIGHT){
                    click=torol;
                    clickpos.x=event.button.x-menu_width;
                    clickpos.y=event.button.y;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button==SDL_BUTTON_LEFT || event.button.button==SDL_BUTTON_RIGHT){
                    click=none;
                }
                break;
            case SDL_MOUSEMOTION:
                if (event.motion.x>=menu_width)
                    screenpart=rajzol;
                else
                    screenpart=menu;

                if (click==rajzol || click==torol){
                    clickpos.x=event.motion.x-menu_width;
                    clickpos.y=event.motion.y;
                }
                else{
                    clickpos.x=event.motion.x;
                    clickpos.y=event.motion.y;
                }
                break;

            case SDL_USEREVENT:
                //méret növelésénél stb. mindig akkora méretűre veszi a cellákat, hogy szépen kiférjen az egész élettér
                while(cellsize*(size.x-correction*2)>screensize.x-menu_width || cellsize*(size.y-correction*2)>screensize.y){
                    cellsize-=1;
                }
                if (running && timer==timermax){
                    click=none;     // miutan megtortent a gombnyomas ertelmezese, utana mar nincs megnyomva
                    eletter_szamol(eletter, size);
                    timer=0;
                }
                else if(running && timer<timermax){
                    timer++;
                }
                else if (!running)
                    timer=0;

                SDL_FillRect(screen, 0, 0xffffffff);
                menu_szamol(screen, &eletter, &size, Menu, gombokszama, &running, click, clickpos, screenpart, &cellsize, &timermax);
                eletter_draw(screen, eletter, screensize, size, cellsize, menu_width, cell_color, correction);

                SDL_Flip(screen);
                break;


            case SDL_QUIT:
                quit=1;
                break;
        }
        //meghatározódik
        clickcell=eletter_koord(size,screensize,menu_width, cellsize, clickpos, correction);
        switch(click){
            case rajzol:
                eletter[clickcell.y][clickcell.x]=1;
                break;
            case torol:
                eletter[clickcell.y][clickcell.x]=0;
                break;
            default:
                break;

        }
    }
    SDL_RemoveTimer(menu_tick);
    eletter_free(eletter);

    SDL_Quit();

    return 0;
}
