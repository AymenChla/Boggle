#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "joueur.h"


void liberation_joueur(Joueur* j)
{
    liberation_score(j->score);
    free(j);
}


Joueur* initialisation_joueur(char *nom)
{
    Joueur *nvJoueur = NULL;
    nvJoueur = Malloc(1,Joueur);
    strcpy(nvJoueur->nom,nom);
    nvJoueur->score = initialisation_score();

    return nvJoueur;
}
