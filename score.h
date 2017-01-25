#ifndef SCORE_H
#define SCORE_H

    #include "generale.h"

    typedef struct{
        char mot[9];
        int points;
    } DonneesMot;

    typedef struct Mot{
        DonneesMot *info;
        struct Mot *suivant;
    } Mot;

    typedef struct{
        int nb_points;
        //tete de la liste chainée
        Mot *teteMots;
    } Score;


    int nb_points(char *);
    Mot *creer_mot(char*);
    Score* initialisation_score();
    void inserer_mot(Score *score,char *mot);
    boolean rechercher_mot(Score *score,char *mot);
    void afficher_score(Score* );
    void liberation_score(Score *score);



#endif // SCORE_H
