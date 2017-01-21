
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "plateau.h"
#include "generale.h"
#include "game.h"

Plateau *initialiser_plateau()
{
    int i,j;
    char *pChar=NULL;
    Plateau *nvPlat = NULL;
    SDL_Color couleurBlanche = {255,255,255};
    SDL_Rect position;


    nvPlat = (Plateau *) malloc(sizeof(Plateau));

    nvPlat->mot_courant[0]='\0';
    nvPlat->taille_mot_courant=0;
    nvPlat->coordonnee_lettre=NULL;

    srand(time(NULL));


    //initialiser les lettres des faces
    strcpy(nvPlat->cases[0][0].lettres,"ETUKNO");
    strcpy(nvPlat->cases[0][1].lettres,"EVGTIN");
    strcpy(nvPlat->cases[0][2].lettres,"DECAMP");
    strcpy(nvPlat->cases[0][3].lettres,"IELRUW");
    strcpy(nvPlat->cases[1][0].lettres,"EHIFSE");
    strcpy(nvPlat->cases[1][1].lettres,"RECALS");
    strcpy(nvPlat->cases[1][2].lettres,"ENTDOS");
    strcpy(nvPlat->cases[1][3].lettres,"OFXRIA");
    strcpy(nvPlat->cases[2][0].lettres,"NAVEDZ");
    strcpy(nvPlat->cases[2][1].lettres,"EIOATA");
    strcpy(nvPlat->cases[2][2].lettres,"GLENYU");
    strcpy(nvPlat->cases[2][3].lettres,"BMAQJO");
    strcpy(nvPlat->cases[3][0].lettres,"TLIBRA");
    strcpy(nvPlat->cases[3][1].lettres,"SPULTE");
    strcpy(nvPlat->cases[3][2].lettres,"AIMSOR");
    strcpy(nvPlat->cases[3][3].lettres,"ENHRIS");

    for(i=0 ; i < NB_LIGNE ; i++)
    {
        for(j=0; j < NB_COLONNE ; j++)
        {
            nvPlat->cases[i][j].clicked = false;
            nvPlat->cases[i][j].lettre_face = nvPlat->cases[i][j].lettres[(rand() % (5 - 0 + 1)) + 0];
            pChar = &nvPlat->cases[i][j].lettre_face;
            pChar[1]='\0';

            nvPlat->cases[i][j].caractere = TTF_RenderText_Blended(myGame.police, pChar , couleurBlanche);
            if(nvPlat->cases[i][j].caractere==NULL)
                error("ttf_render caractere");

            nvPlat->cases[i][j].surface_case = SDL_CreateRGBSurface(SDL_HWSURFACE,TAILLE_CASE,TAILLE_CASE,32,0,0,0,0);
             if(nvPlat->cases[i][j].surface_case==NULL)
                error("ttf_render caractere");

            SDL_FillRect(nvPlat->cases[i][j].surface_case,NULL,SDL_MapRGB(nvPlat->cases[i][j].surface_case->format,47,87,145));

            position.x = (nvPlat->cases[i][j].surface_case->w-nvPlat->cases[i][j].caractere->w)/2;
            position.y = (nvPlat->cases[i][j].surface_case->h-nvPlat->cases[i][j].caractere->h)/2;

            SDL_BlitSurface(nvPlat->cases[i][j].caractere,NULL,nvPlat->cases[i][j].surface_case,&position);
        }
    }


     nvPlat->surface_plateau = SDL_CreateRGBSurface(SDL_HWSURFACE,TAILLE_PLATEAU,TAILLE_PLATEAU,32,0,0,0,0);
     SDL_FillRect(nvPlat->surface_plateau,NULL,SDL_MapRGB(nvPlat->surface_plateau->format,255,255,255));




    position.y = DECALAGE;
    //liaison des surfaces
    for(i=0 ; i < NB_LIGNE ;i++)
    {
        position.x = DECALAGE;
        for(j=0 ; j < NB_COLONNE ; j++)
        {
            nvPlat->cases[i][j].position.x = position.x;
            nvPlat->cases[i][j].position.y = position.y;
            SDL_BlitSurface(nvPlat->cases[i][j].surface_case,NULL,nvPlat->surface_plateau,&position);

            position.x += TAILLE_CASE+DECALAGE;
        }
        position.y += TAILLE_CASE+DECALAGE;
    }


    return nvPlat;
}

void liberation_plateau(Plateau *plat)
{
    free(plat);
}


/*void afficher_plateau(Plateau *p)
{
    int i,j;

    for(i=0 ; i < NB_LIGNE ; i++)
    {
        for(j = 0 ; j < NB_COLONNE ; j++)
        {
            printf("|\t%c\t",p->matrice[i][j]);
        }
        printf("\n");
    }
}*/




