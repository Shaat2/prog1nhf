#ifndef LIFE_TYPEDEFS_H_INCLUDED
#define LIFE_TYPEDEFS_H_INCLUDED

typedef struct{
    int x;
    int y;
}Pont;
typedef enum{
    none,
    rajzol,
    torol,
    menu
}clickaction;
typedef enum Btnstate{
    no_hover,
    hovering,
    clicked,
    inactive
}Btnstate;
typedef struct Menu_btn{
    Pont p1;
    Pont p2;
    int no_hover_color;
    int hover_color;
    int inactive_color;
    int clicked_color;
    char szoveg[20];
    int clicked;  //azert, hogy bizonyos gombokon a hosszu nyomvatartas ne inditsa ujra tobbszor a gomb funkciojat
    Btnstate btnstate;
}Menu_btn;

#endif // LIFE_TYPEDEFS_H_INCLUDED
