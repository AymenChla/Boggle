#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "score.h"


void afficher_score(Score* score)
{
    Mot *cour = NULL;
    cour = score->teteMots;


    fprintf(stdout,"score: %d\n",score->nb_points);
    while(cour != NULL)
    {
        fprintf(stdout,"%s\t%d\n",cour->info->mot,cour->info->points);
        cour = cour->suivant;
    }
}

int nb_points(char* mot)
{
        switch(strlen(mot))
        {
            case 3:
            case 4:
                return 1;
            break;

            case 5:
                return 2;
            break;

            case 6:
                return 3;
            break;

            case 7:
                return 5;
            break;

            default:
                return 11;
            break;
        }

}

Mot *creer_mot(char *mot)
{
        Mot *nvMot = Malloc(1,Mot);
        nvMot->info = Malloc(1,DonneesMot);

        strcpy(nvMot->info->mot,mot);
        nvMot->info->points = nb_points(mot);

        nvMot->suivant = NULL;

        return nvMot;
}

Score* initialisation_score()
{
    Score *score = Malloc(1,Score);
    score->nb_points = 0;
    score->teteMots = NULL;
    return score;
}


void inserer_mot(Score *score,char *mot)
{

    if(rechercher_mot(score,mot) == false)
    {
        Mot *nvMot = creer_mot(mot);

        score->nb_points += nvMot->info->points;

        nvMot->suivant = score->teteMots;
        score->teteMots = nvMot;

    }

}


boolean rechercher_mot(Score *score,char *mot)
{
    Mot *cour = NULL;

    cour = score->teteMots;

    while(cour != NULL && strcmp(mot,cour->info->mot)!= 0)
        cour = cour->suivant;

    if(cour != NULL)
        return true;

    return false;
}

void liberation_score(Score *score)
{
    Mot *cour = score->teteMots;
    Mot *temp;
    while(cour != NULL)
    {
        temp = cour;
        free(cour->info);
        cour = cour->suivant;
        free(temp);
    }
    free(score);
}
