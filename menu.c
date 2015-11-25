#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include "eletter.h"
#include "life_typedefs.h"
#include "menu.h"
#include <stdio.h>

const int lapozassebesseg=40;  // a load gomb nyilai mennyi idő alatt adják a következő filet

void btn_draw(SDL_Surface *screen, Menu_btn btn){
    // simán csak külön esetek lekezelésével kirajzolja a gombot
    switch (btn.btnstate){
        case no_hover:
            boxColor(screen,btn.p1.x, btn.p1.y, btn.p2.x, btn.p2.y, btn.no_hover_color);
            rectangleColor(screen, btn.p1.x, btn.p1.y, btn.p2.x, btn.p2.y, 0x000000ff);
            stringColor(screen, btn.p1.x+10, (btn.p1.y+btn.p2.y)/2, btn.szoveg, 0x000000ff);
            break;
        case hovering:
            boxColor(screen,btn.p1.x, btn.p1.y, btn.p2.x, btn.p2.y, btn.hover_color);
            rectangleColor(screen, btn.p1.x, btn.p1.y, btn.p2.x, btn.p2.y, 0x000000ff);
            stringColor(screen, btn.p1.x+10, (btn.p1.y+btn.p2.y)/2, btn.szoveg, 0x000000ff);
            break;
        case clicked:
            boxColor(screen,btn.p1.x, btn.p1.y, btn.p2.x, btn.p2.y, btn.clicked_color);
            rectangleColor(screen, btn.p1.x, btn.p1.y, btn.p2.x, btn.p2.y, 0x000000ff);
            stringColor(screen, btn.p1.x+10, (btn.p1.y+btn.p2.y)/2, btn.szoveg, 0x000000ff);
            break;
        case inactive:
            boxColor(screen,btn.p1.x, btn.p1.y, btn.p2.x, btn.p2.y, btn.inactive_color);
            rectangleColor(screen, btn.p1.x, btn.p1.y, btn.p2.x, btn.p2.y, 0x000000ff);
            stringColor(screen, btn.p1.x+10, (btn.p1.y+btn.p2.y)/2, btn.szoveg, 0xddddddff);
            break;
    }
}

int gombon_e(Menu_btn btn, Pont clickpos){
    // ha a gomb koordinátái közé esik, akkor igen.
    if (clickpos.x>=btn.p1.x && clickpos.x<=btn.p2.x && clickpos.y>=btn.p1.y && clickpos.y<=btn.p2.y)
        return 1;
    return 0;
}

int melyik_gombon(Menu_btn *Menu, int gombokszama, Pont clickpos){
    // megnézi hogy a gombok közül melyiken van
    int i;
    for(i=0; i<gombokszama; ++i){
        if (gombon_e(Menu[i], clickpos))
            return i;
    }
    return 0;
}
Menu_btn nyil_btn_generate(int irany /* 0=jobbra, 1=balra */,int p1_x,int p1_y,int p2_x,int p2_y, int no_hover_color){
    Menu_btn uj;
    uj.p1.x=p1_x;
    uj.p2.x=p2_x;
    uj.p1.y=p1_y;
    uj.p2.y=p2_y;
    uj.no_hover_color=no_hover_color;
    uj.inactive_color=no_hover_color+0x29292900; //világosít
    uj.hover_color=no_hover_color-0x11111100;   // sötétít
    uj.clicked_color=no_hover_color-0x33333300; // jobban sötétít
    uj.clicked=0;
    uj.btnstate=none;
    if(irany)
        strcpy(uj.szoveg, "<");
    else
        strcpy(uj.szoveg, ">");
    return uj;
}
void menu_generate(struct Menu_btn *Menu, int gombokszama, int menu_width){
    int i;
    int gombok_tavolsaga=70; //100 pixel koztuk
    int padding=10; //mennyire legyenek a kerettol
    int gomb_magassag=30; //milyen magasak legyenek (negyzetes gombok eseteben milyen szelesek is)
    /*
    sajat tetszes szerint beallitott ertekek, kicsit hosszadalmas sajnos innentol
    */
    for(i=0; i<gombokszama; ++i){
            if (i<6){  // a fo gombok kinezete
            Menu[i].p1.x=padding;
            Menu[i].p1.y=padding+i*gombok_tavolsaga;
            Menu[i].p2.x=menu_width-padding;
            Menu[i].p2.y=Menu[i].p1.y+gomb_magassag;
            Menu[i].no_hover_color=0xccccccff;
            Menu[i].inactive_color=0xf5f5f5ff;
            Menu[i].hover_color=0xbbbbbbff;
            Menu[i].clicked_color=0x999999ff;
            Menu[i].btnstate=none;
            Menu[i].clicked=0;
            }
            else{
                switch(i){

                    case 6:  // a load select balra, majd jobbra mutato nyila
                        Menu[i]=nyil_btn_generate(1, padding, padding+5*gombok_tavolsaga+gomb_magassag, padding+gomb_magassag, padding+5*gombok_tavolsaga+gomb_magassag*2, 0xccccccff);
                        break;
                    case 7:
                        Menu[i]=nyil_btn_generate(0, menu_width-padding-gomb_magassag, padding+5*gombok_tavolsaga+gomb_magassag, menu_width-padding, padding+5*gombok_tavolsaga+gomb_magassag*2, 0xccccccff);
                        break;
                    case 8: // cellsize  balra, utana jobbra
                        Menu[i]=nyil_btn_generate(1, padding, padding+6*gombok_tavolsaga+gomb_magassag, padding+gomb_magassag, padding+6*gombok_tavolsaga+gomb_magassag*2, 0xccccccff);
                        break;
                    case 9:
                        Menu[i]=nyil_btn_generate(0, padding+gomb_magassag, padding+6*gombok_tavolsaga+gomb_magassag, padding+gomb_magassag*2, padding+6*gombok_tavolsaga+gomb_magassag*2, 0xccccccff);
                        break;
                    case 10: // simul.speed  balra, utana jobbra
                        Menu[i]=nyil_btn_generate(1, menu_width-padding-gomb_magassag*2, padding+6*gombok_tavolsaga+gomb_magassag, menu_width-padding-gomb_magassag, padding+6*gombok_tavolsaga+gomb_magassag*2, 0xccccccff);
                        break;
                    case 11:
                        Menu[i]=nyil_btn_generate(0, menu_width-padding-gomb_magassag, padding+6*gombok_tavolsaga+gomb_magassag, menu_width-padding, padding+6*gombok_tavolsaga+gomb_magassag*2, 0xccccccff);
                        break;
                    case 12: // horizontal size balra, utana jobbra
                        Menu[i]=nyil_btn_generate(1, padding, padding+7*gombok_tavolsaga+gomb_magassag, padding+gomb_magassag, padding+7*gombok_tavolsaga+gomb_magassag*2, 0xccccccff);
                        break;
                    case 13:
                        Menu[i]=nyil_btn_generate(0, padding+gomb_magassag, padding+7*gombok_tavolsaga+gomb_magassag, padding+gomb_magassag*2, padding+7*gombok_tavolsaga+gomb_magassag*2, 0xccccccff);
                        break;
                    case 14: // vertical size balra, utana jobbra
                        Menu[i]=nyil_btn_generate(1, menu_width-padding-gomb_magassag*2, padding+7*gombok_tavolsaga+gomb_magassag, menu_width-padding-gomb_magassag, padding+7*gombok_tavolsaga+gomb_magassag*2, 0xccccccff);
                        break;
                    case 15:
                        Menu[i]=nyil_btn_generate(0, menu_width-padding-gomb_magassag, padding+7*gombok_tavolsaga+gomb_magassag, menu_width-padding, padding+7*gombok_tavolsaga+gomb_magassag*2, 0xccccccff);
                        break;

                }
            }
            switch(i){ // a fo gombok szovegei
                case 0:
                    strncpy(Menu[i].szoveg, "Reset", 20);
                    break;
                case 1:
                    strncpy(Menu[i].szoveg, "Run", 20);
                    break;
                case 2:
                    strncpy(Menu[i].szoveg, "Stop", 20);
                    break;
                case 3:
                    strncpy(Menu[i].szoveg, "1 step", 20);
                    break;
                case 4:
                    strncpy(Menu[i].szoveg, "Save", 20);
                    break;
                case 5:
                    strncpy(Menu[i].szoveg, "Load", 20);
                    break;
            }
            Menu[5].p1.x=Menu[6].p2.x+1;
            Menu[5].p1.y=Menu[6].p1.y;
            Menu[5].p2.x=Menu[7].p1.x-1;
            Menu[5].p2.y=Menu[7].p2.y;
    }
}

void menu_szamol(SDL_Surface *screen, int ***eletter, Pont *size, Menu_btn *Menu, int gombokszama, int *running, clickaction click, Pont clickpos, clickaction screenpart, int *cellsize, int *timermax){
    // a mentések könyvtárban levő fájlok neveinek betöltsése
    int i=1;
    static int filepointer=0;

    int filepointermax=0;
    char filelist[500][34]={{0}};  //max az elso 500 filet listazza ki, es a filenev max 30(+kiterjesztes) lehet
    struct dirent *dp;

    DIR *dfd=opendir("./saves/");
    if(dfd!=NULL){
        while ((dp=readdir(dfd))!=NULL){
            if (strstr(dp->d_name, ".txt")){
                strcpy(filelist[i], "./saves/");
                strcat(filelist[i], dp->d_name);
                i++;
            }
        }
        closedir(dfd);
    }
    else perror("./saves/ can't be opened");
    filepointermax=i-1;
    // gombok helyzeteinek lekezelése
    if (!(click == rajzol || click==torol)){
        if (*running){
            Menu[0].btnstate=inactive;
            Menu[1].btnstate=inactive;
            Menu[2].btnstate=no_hover;
            //többi gomb is inactive
            for (i=4; i<gombokszama; i++)
                Menu[i].btnstate=inactive;

        }
        else{
            Menu[0].btnstate=no_hover;
            Menu[1].btnstate=no_hover;
            Menu[2].btnstate=inactive;
            //többi gomb is no_hover
            for (i=4; i<gombokszama; i++)
                Menu[i].btnstate=no_hover;
        }
        for(i=0; i<gombokszama; ++i){
            if(Menu[i].btnstate!=inactive){
                if(screenpart==menu && gombon_e(Menu[i], clickpos)){
                    if (click==menu){
                        Menu[i].btnstate=clicked;
                    }
                    else
                        Menu[i].btnstate=hovering;
                }
                else
                    Menu[i].btnstate=no_hover;
            }
        }
    }
    // gombok kirajzolasa
    for(i=0; i<gombokszama; ++i){
        btn_draw(screen, Menu[i]);
    }
    char timerchar[2];  // a szimulacios sebesseg kiirasara
    sprintf(timerchar, "%d", *timermax);

    char currentload[25]={0}; // az eppen betoltheto file nevenek kiirasa
    strncat(currentload, filelist[filepointer]+8, 25);
    //
    stringColor(screen, 10, 425, currentload, 0x000000ff);
    stringColor(screen, 105, 470, timerchar, 0x000000ff);
    stringColor(screen, 10, 450, "Cellsize", 0x000000ff);
    stringColor(screen, 100, 450, "Simul. speed", 0x000000ff);
    stringColor(screen, 10, 520, "Ver. size", 0x000000ff);
    stringColor(screen, 120, 520, "Hor. size", 0x000000ff);

    /*
        1-step gomb: a gond az hogy elég ideig kell megnyomva lennie a gombnak
        simulation speed függvényében változik, 1-es sebességnél tökéletesen működik a gomb
        néha egérrel sem jön össze, laptop touchpaddal meg egyenesen nehéz
    */
    if(Menu[3].btnstate==clicked && *running==0 && Menu[3].clicked==0){
        *running=1;
        Menu[3].clicked=1;
    }
    else if (Menu[3].btnstate==(clicked || hovering) && *running==1 && Menu[3].clicked==1){
        Menu[3].clicked=0;
        *running=0;
    }
    else if(Menu[3].btnstate==clicked && *running==1 && Menu[3].clicked==0)
        *running=0;

    //reset gomb
    else if (Menu[0].btnstate==clicked)
        eletter_nullaz(*eletter, *size);
    //run gomb
    else if (Menu[1].btnstate==clicked)
        *running=1;
    //stop gomb
    else if (Menu[2].btnstate==clicked)
        *running=0;
    //save gomb
    else if (Menu[4].btnstate==clicked)
        eletter_save(*eletter, *size);
    //load gomb
    else  if (Menu[5].btnstate==clicked)
        eletter_load(eletter, size, filelist[filepointer]);
    // load lapozas balra
    else if (Menu[6].btnstate==clicked && Menu[6].clicked==0){
        filepointer-=1;
        if (filepointer<0)
            filepointer=filepointermax-1;
        Menu[6].clicked+=1;
    }
    else if (Menu[6].btnstate==clicked && Menu[6].clicked<=lapozassebesseg){ // lapozassebesseg a kod tetejen constban
        if (Menu[6].clicked==lapozassebesseg)
            Menu[6].clicked=0;
        else Menu[6].clicked+=1;   // nem tul szep de a valtozo int volta kihasznalhato szamlalokent
    }
    else if (Menu[6].btnstate!=clicked && Menu[6].clicked!=0)
        Menu[6].clicked=0;
    // load lapozas jobbra
    else if (Menu[7].btnstate==clicked && Menu[7].clicked==0){
        filepointer+=1;
        if (filepointer>filepointermax)
            filepointer=0;
        Menu[7].clicked+=1;
    }
    else if (Menu[7].btnstate==clicked && Menu[7].clicked<=lapozassebesseg){
       if (Menu[7].clicked==lapozassebesseg)
            Menu[7].clicked=0;
        else Menu[7].clicked+=1;
    }
    else if (Menu[7].btnstate!=clicked && Menu[7].clicked!=0)
        Menu[7].clicked=0;

    // cellakicsinyites
    else if (Menu[8].btnstate==clicked){
        if (*cellsize>4)   // 4-nel kisebb ertekeket mar nem kezel jol
            *cellsize-=1;
    }
    // cellanagyitas
    else if (Menu[9].btnstate==clicked){
        if (*cellsize<200)  //200-nal nagyobb nem túlzottan elkepzelheto
            *cellsize+=1;
    }
    //szimulacio gyorsitas
    else if (Menu[10].btnstate==clicked){
        if (*timermax>1)
            *timermax-=1;
    }
    //szimulacio lassitas
    else if (Menu[11].btnstate==clicked){
        if (*timermax<60)
            *timermax+=1;
    }
    //eletter szelesseg csokkentes
    else if (Menu[12].btnstate==clicked){
        if (size->x>6){
            size->x-=1;    //sorok/oszlopok csokkentesenel nem tortenhez tulindexeles, ezert nem szukseges uj eletteret kesziteni
        }
    }
    //eletter szelesseg noveles
    else if (Menu[13].btnstate==clicked){
        if (size->x<=100){
            int **eletter_temp=*eletter;    // elozo eletter adatainak eltarolasa
            Pont tempsize=*size;
            size->x+=1;
            *eletter=eletter_create(*size);     //elozo eletter lecserelese uj mereture
            eletter_nullaz(*eletter, *size);

            eletter_masol(*eletter, eletter_temp, tempsize); // temp adatainak atmasolasa az ujba
            eletter_free(eletter_temp); // temp felszabaditasa
        }
    }
    //eletter magassag csokkentes
    else if (Menu[14].btnstate==clicked){
        if (size->y>6){
            size->y-=1;
        }
    }
    //eletter magassag noveles
    else if (Menu[15].btnstate==clicked){
        if (size->y<=100){
            int **eletter_temp=*eletter;    // elozo eletter adatainak eltarolasa
            Pont tempsize=*size;
            size->y+=1;
            *eletter=eletter_create(*size);     //elozo eletter lecserelese uj mereture
            eletter_nullaz(*eletter, *size);

            eletter_masol(*eletter, eletter_temp, tempsize); // temp adatainak atmasolasa az ujba
            eletter_free(eletter_temp); // temp felszabaditasa
        }
    }
}
