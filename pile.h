#ifndef PILE_H
#define PILE_H

#include "generale.h"

typedef struct{
    int i;
    int j;
}Coord;

typedef struct Tag
{
    Coord coord;
    struct Tag *suivant;

} Pile;

boolean pile_vide(Pile *pile);
void empiler(Pile **pile,int i,int j);
Coord depiler(Pile **pile);
boolean existe(Pile *pile,int i, int j);


#endif // PILE_H
