#ifndef PLATEAU_H
#define PLATEAU_H

    #include <SDL.h>
    #include "generale.h"
    #include "pile.h"

    typedef struct{
        SDL_Rect position;
        boolean clicked;
        SDL_Surface* surface_case;
        SDL_Surface* caractere;
        char lettre_face;
        char lettres[7];
    }Case;

    typedef struct {

        //pour la permission du choix du dé
        Pile *coordonnee_lettre;

        char mot_courant[17];
        int taille_mot_courant;

        SDL_Rect position;
        SDL_Surface *surface_plateau;
        Case cases[4][4];

        //for future version
        int nb_mots;

    }Plateau;


    Plateau *initialiser_plateau();

#endif // PLATEAU_H

