#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <time.h>
#include "eletter.h"
#include "life_typedefs.h"
#include "menu.h"

int** eletter_create(Pont size){
    //dinamikusan lefoglal egy y méretű, majd egy x * y méretű tömböt, és az első tömb elemeivel a tábla "sorainak" az elejére mutat
    int **eletter;
    int i;
    eletter=(int **) malloc(size.y*sizeof(int*));
    eletter[0]= (int *) malloc(size.y*size.x*sizeof(int));
    for (i=1; i<size.y; ++i){
        eletter[i] = eletter[0]+i*size.x;
    }
    return eletter;
}

void eletter_free(int **eletter){
    //elég 2 free, mert csak 2 malloc volt
    free(eletter[0]);
    free(eletter);
}

void eletter_nullaz(int **eletter, Pont size){
    int n, i;
    for(i=0; i<size.y; ++i){
        for(n=0; n<size.x; ++n){
            eletter[i][n]=0;
        }
    }
}

void eletter_draw(SDL_Surface *screen,int **eletter,Pont screensize, Pont size,int cellsize, int menu_width, int color, int correction){
    int x, y;
    // a padding úgy számítódik ki, hogy kb a képernyő közepére rajzolja az életteret
    Pont padding;
    padding.x=(screensize.x-menu_width-(cellsize*(size.x-correction*2)))/2;
    padding.y=(screensize.y-(cellsize*(size.y-correction*2)))/2;
    // a correction mindenhol a nem kirajzolandó sorok és oszlopok száma
    for(y=correction; y<size.y-correction; ++y){
        for(x=correction; x<size.x-correction; ++x){
            if(eletter[y][x])   // négyzeteket rajzol, az első négyzet úgy alakhul hog
                boxColor(screen, menu_width+padding.x-correction*cellsize + 1/*+1 hogy ne fedje a vonalakat*/+x*cellsize, -correction*cellsize + 1 + y*cellsize+padding.y, -correction*cellsize+ -1 +menu_width+(x+1)*cellsize+padding.x, -correction*cellsize+-1+(y+1)*cellsize+padding.y, color);
            else //fehér színnel ugyanez
                boxColor(screen, menu_width+padding.x-correction*cellsize + 1/*+1 hogy ne fedje a vonalakat*/+x*cellsize, -correction*cellsize + 1 + y*cellsize+padding.y, -correction*cellsize+ -1 +menu_width+(x+1)*cellsize+padding.x, -correction*cellsize+-1+(y+1)*cellsize+padding.y, 0xffffffff);
        }
    }
    for(y=0; y<size.y-correction*2; ++y){
        for(x=0; x<size.x-correction*2; ++x){
            lineColor(screen, padding.x+menu_width+x*cellsize, padding.y+y*cellsize, padding.x+menu_width+(x+1)*cellsize, padding.y+y*cellsize, 0x000000ff);
            lineColor(screen, padding.x+menu_width+x*cellsize, padding.y+y*cellsize, padding.x+menu_width+x*cellsize, padding.y+(y+1)*cellsize, 0x000000ff);
        }
    }
    lineColor(screen, padding.x+(size.x-correction*2)*cellsize+menu_width, padding.y, padding.x+(size.x-correction*2)*cellsize+menu_width, padding.y+(size.y-correction*2)*cellsize, 0x000000ff);
    lineColor(screen, padding.x+menu_width, padding.y+(size.y-correction*2)*cellsize, padding.x+(size.x-correction*2)*cellsize+menu_width, padding.y+(size.y-correction*2)*cellsize, 0x000000ff);
}

int szomszedok_szamol(int **eletter, int y, int x){
    // elég lényegretörő, megszámolja a szomszédos aktív cellákat
    int szam=0;
    int i, n;
    if (eletter[y][x]==1) // önmagát ne számolja bele
        szam=-1;
    for(i=y-1; i<=y+1; ++i){
        for(n=x-1; n<=x+1; ++n){
            if (eletter[i][n]==1)
                    szam+=1;
        }
    }
    return szam;
}

void eletter_szamol(int **eletter, Pont size){
    // ebben a függvényben találhatóak meg a Game of Life szabályai, itt végződik el a kiértékelés
    int y, x;
    //temp élettér az igazi méretével
    //a magyar wikipedia témához kötődő szócikkéből jött az ötlet, sokkal egyszerűbb temp-pel végrehajtani a számolást
    int **temp;
    temp=eletter_create(size);
    eletter_nullaz(temp, size);

    int szomszedok=0;
    for(y=1; y<size.y-1; ++y){
        for(x=1; x<size.x-1; ++x){
            szomszedok=szomszedok_szamol(eletter, y, x);    // megszamolja a szomszedjait minden cellanak
            if (eletter[y][x]==1){
                if (szomszedok==2 || szomszedok==3) // ha a cella él és 2 vagy 3 szomszédja van, akkor lemásoljuk a cellát
                    temp[y][x]=1;
//                else if (szomszedok<2 || szomszedok>3) // ha a cella él, de 2nél kevesebb vagy 3-nál több szomszédja van, akkor kinullázzuk a cellát
//                    temp[y][x]=0;                      // TBH ez elég fölösleges lépés lenne
            }
            else if (eletter[y][x]==0){  //ha 3 szomszédja van, de amúgy üres, akkor a tempben élővé tesszük
                if (szomszedok==3)
                    temp[y][x]=1;
            }
        }
    }
    //egy az egyben átmásolja az újba
    for(y=0; y<size.y; ++y){
        for(x=0; x<size.x; ++x){
            eletter[y][x]=temp[y][x];
        }
    }
    eletter_free(temp);
}

Pont eletter_koord(Pont size, Pont screensize,int menu_width, int cellsize, Pont clickpos, int correction){
    // különböző változók alapján kiszámítja hogy melyik cellába esett a klikkelésünk
    Pont koord;

    Pont padding;
    padding.x=(screensize.x-menu_width-(cellsize*(size.x-correction*2)))/2;
    padding.y=(screensize.y-(cellsize*(size.y-correction*2)))/2;
    koord.x=(clickpos.x-padding.x)/cellsize+correction;

    if (koord.x<correction)
        koord.x=correction;

    if (koord.x>size.x-correction)
        koord.x=size.x-correction-1;

    koord.y=(clickpos.y-padding.y)/cellsize+correction;
    if (koord.y<correction)
        koord.y=correction;

    if (koord.y>size.y-correction)
        koord.y=size.y-correction-1;
    return koord;
}
void eletter_masol(int **dest, int **src, Pont sourcesize){
    // felétételezi, hogy a source mérete <= dest méretével
    int x, y;
    for(y=0; y<sourcesize.y; ++y){
        for(x=0; x<sourcesize.x; ++x){
            dest[y][x]=src[y][x];
        }
    }
}
void eletter_save(int **eletter, Pont size){
    FILE *fp;
    //a jelenlegi ido stringge alakitasa
    time_t ido;
    ido= time(NULL);
    char *time_string;
    time_string= ctime(&ido);  // Www Mmm dd hh:mm:ss yyyy\n
    char filename[34]="./saves/";  // 30 hosszu filenevek max, +a kiterjesztes
    strncat(filename, time_string+4, 20); //a Www és a newline lehagyhato
    // az adatok koze kotojelek rakasa
    char *replacer;
    char key[] = " :";
    replacer = strpbrk (filename, key);
    while (replacer != NULL){
        *replacer= '-';
        replacer = strpbrk (replacer+1,key);
    }
    // .txt-vel megtoldas
    strcat(filename, ".txt");
    fp = fopen(filename, "wt");
    if (fp==NULL){
        perror(filename);
    }
    else {
        fprintf(fp, "%d %d\n", size.x, size.y);  // kiírja a méreti adatokat
        int x, y;
        for(y=0; y<size.y; ++y){
            for(x=0; x<size.x; ++x){
                if (eletter[y][x])
                    fprintf(fp, "x");
                else
                    fprintf(fp, ".");
            }
            fprintf(fp, "\n");
        }
    }
    fclose(fp);
}

void eletter_load(int ***eletter, Pont *size, const char *filename){
    FILE *fp;
    fp = fopen(filename, "rt");
    if (fp==NULL) perror(filename);
    else {
        int c;
        int x;
        int y;
        eletter_free(*eletter);
        fscanf(fp, "%d %d", &x, &y); // méreti adatok beolvasása
        size->x=x;
        size->y=y;
        *eletter=eletter_create(*size);  // megadott méretű élettér inicializálása
        eletter_nullaz(*eletter, *size);
        x=y=0;
        c=fgetc(fp);  // fájl többi részének a végigolvasása
        while (c!=EOF){
            if (c=='x'){
                (*eletter)[y-1][x]=1;
                x++;
            }
            else if(c=='\n'){
                y++;
                x=0;
            }
            else
                x++;
            c=fgetc(fp);
        }
    }
    fclose(fp);
}
