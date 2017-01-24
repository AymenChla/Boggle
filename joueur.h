
#ifndef JOUEUR_H
#define JOUEUR_H

    #include "score.h"

    typedef struct {
        char nom[20];
        Score *score;
    } Joueur;


    void liberation_joueur(Joueur*);
    Joueur* initialisation_joueur();

#endif // JOUEUR_H
