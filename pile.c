
#include <stdio.h>
#include <stdlib.h>
#include "pile.h"

void empiler(Pile **pile,int i,int j)
{
    Pile *nvNoeud = Malloc(1,Pile);
    nvNoeud->coord.i = i;
    nvNoeud->coord.j = j;
    nvNoeud->suivant = *pile;
    *pile = nvNoeud;
    return pile;
}

Coord depiler(Pile **pile)
{
    if(!pile_vide(*pile))
    {
        Coord coord = {(*pile)->coord.i,(*pile)->coord.j};
        Pile *temp = *pile;
        *pile = (*pile)->suivant;
        free(temp);
        return coord;
    }
    return;
}

boolean existe(Pile *pile,int i, int j)
{
    while(pile!=NULL && (pile->coord.i != i || pile->coord.j != j))
        pile = pile->suivant;

    if(pile != NULL) return true;
    return false;
}

boolean pile_vide(Pile *pile)
{
    if (pile==NULL) return true;
    return false;
}
