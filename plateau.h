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
        Pile *coordonnee_lettre;
        int taille_mot_courant;
        SDL_Rect position;
        SDL_Surface *surface_plateau;
        Case cases[4][4];
        int nb_mots;
    }Plateau;


    Plateau *initialiser_plateau();
    void liberation_plateau(Plateau *);
    void afficher_plateau(Plateau *p);
    //boolean validation_sur_plateau(char *mot);


    //boolean validation_repetition(Joueur j,char *mot);

#endif // PLATEAU_H

