
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "game.h"
#include "pile.h"

void print_(SDL_KeyboardEvent *key)
{
        fprintf(stdout, "%c", (char)key->keysym.unicode);

}

void error(char *msg)
{
    fprintf(stderr,"%s: %s\n",msg,SDL_GetError());
    exit(EXIT_FAILURE);
}


SDL_Surface *initialisation_sdl()
{
    SDL_Surface *screen = NULL;
    if(SDL_Init(SDL_INIT_VIDEO |SDL_INIT_AUDIO) == -1)
        error("impossible d'initialiser la sdl");

    screen = SDL_SetVideoMode(WIDTH_SCREEN,HEIGHT_SCREEN,32,SDL_HWSURFACE | SDL_DOUBLEBUF);
    if(screen == NULL)
        error("impossible de charger le mode video");

    SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,45,75,20));

    SDL_WM_SetCaption("boggle",NULL);

    return screen;
}

TTF_Font* initialisation_ttf(char* nom_police)
{
        TTF_Font *police = NULL;

        if(TTF_Init()== -1){

            fprintf(stderr,"%s: %s\n","impossible d'initialiser la ttf: s\n",TTF_GetError());
            exit(EXIT_FAILURE);
        }


        police = TTF_OpenFont(nom_police,24);
        if(police == NULL)
        {
            fprintf(stderr,"%s: %s\n","impossible d'ouvrir le font: s\n",TTF_GetError());
            exit(EXIT_FAILURE);
        }

        return police;
}

boolean is_last_click(int i,int j)
{
    if(pile_vide(myGame.plateau->coordonnee_lettre)) return true;
    if(myGame.plateau->coordonnee_lettre->coord.i == i && myGame.plateau->coordonnee_lettre->coord.j == j)return true;
    return false;
}

boolean can_be_hovered(int i,int j)
{
    int cmpt=0;

    if(j -1 >=0 ) cmpt+=is_last_click(i,j-1);
    if(i-1 >= 0 && j -1 >=0 ) cmpt+=is_last_click(i-1,j-1);
    if(i-1 >= 0 ) cmpt+=is_last_click(i-1,j);
    if(i-1 >= 0 && j + 1 < NB_COLONNE ) cmpt+=is_last_click(i-1,j+1);
    if(j +1 <NB_COLONNE ) cmpt+=is_last_click(i,j+1);
    if(i+1 < NB_LIGNE && j + 1 <NB_COLONNE ) cmpt+=is_last_click(i+1,j+1);
    if(i+1 <NB_COLONNE ) cmpt+=is_last_click(i+1,j);
    if(i+1 < NB_COLONNE && j -1 >=0 ) cmpt+=is_last_click(i+1,j-1);


    if(cmpt>0) return true;
    return false;
}

void play_game(Game myGame)
{
    int i=0,j=0;
    SDL_Event event;
    boolean continuer=true;
    Uint32 couleur_hover=SDL_MapRGB(myGame.plateau->surface_plateau->format,45,78,102);
    Uint32 couleur_click=SDL_MapRGB(myGame.screen->format,75,87,92);
    Uint32 couleur_par_defaut=SDL_MapRGB(myGame.screen->format,47,87,145);
    SDL_Color couleur_valider_hover={45,78,102};
    SDL_Color couleurBlanche = {255,255,255};
    SDL_Rect position;
    int temp_actuel=0,temp_precedent=0;
    int secondes=1000*60*3;
    char time[6];
    SDL_Rect position_relatif;
    position_relatif.x=0;
    position_relatif.y=0;

    position.x = (myGame.plateau->cases[i][j].surface_case->w-myGame.plateau->cases[i][j].caractere->w)/2;
    position.y = (myGame.plateau->cases[i][j].surface_case->h-myGame.plateau->cases[i][j].caractere->h)/2;

    while(continuer)
    {

        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    continuer=false;
                break;

                case SDL_MOUSEMOTION:

                    for(i=0 ; i < NB_LIGNE ; i++)
                    {
                        for(j=0 ; j < NB_LIGNE ; j++)
                        {
                            //si case non cliqué et (peut etre choisi ou bien on a rien choisi)
                            if(event.motion.x >= (myGame.plateau->cases[i][j].position.x+myGame.plateau->position.x)
                               && event.motion.x <= (myGame.plateau->cases[i][j].position.x+myGame.plateau->position.x+TAILLE_CASE)
                               && event.motion.y >= (myGame.plateau->cases[i][j].position.y+myGame.plateau->position.y)
                               && event.motion.y <= (myGame.plateau->cases[i][j].position.y+myGame.plateau->position.y+TAILLE_CASE)
                               && myGame.plateau->cases[i][j].clicked==false
                               && (can_be_hovered(i,j)) )
                            {

                                SDL_FillRect(myGame.plateau->cases[i][j].surface_case,NULL,couleur_hover);
                                SDL_BlitSurface(myGame.plateau->cases[i][j].caractere,NULL,myGame.plateau->cases[i][j].surface_case,&position);
                                SDL_BlitSurface(myGame.plateau->cases[i][j].surface_case,NULL,myGame.plateau->surface_plateau,&myGame.plateau->cases[i][j].position);
                                SDL_BlitSurface(myGame.plateau->surface_plateau,NULL,myGame.screen,&myGame.plateau->position);
                            }
                            //si la case n'est pas deja cliqué
                            else if(myGame.plateau->cases[i][j].clicked==false)
                            {
                                SDL_FillRect(myGame.plateau->cases[i][j].surface_case,NULL,couleur_par_defaut);
                                SDL_BlitSurface(myGame.plateau->cases[i][j].caractere,NULL,myGame.plateau->cases[i][j].surface_case,&position);
                                SDL_BlitSurface(myGame.plateau->cases[i][j].surface_case,NULL,myGame.plateau->surface_plateau,&myGame.plateau->cases[i][j].position);
                                SDL_BlitSurface(myGame.plateau->surface_plateau,NULL,myGame.screen,&myGame.plateau->position);
                            }

                        }
                    }


                    //hover bouton valider
                    if(event.motion.x >= (myGame.btn_outils->position_valider.x)
                               && event.motion.x <= (myGame.btn_outils->position_valider.x+myGame.btn_outils->position_valider.w)
                               && event.motion.y >= (myGame.btn_outils->position_valider.y)
                               && event.motion.y <= (myGame.btn_outils->position_valider.y+myGame.btn_outils->position_valider.h)
                       )
                       {
                           myGame.btn_outils->valider = TTF_RenderText_Blended(myGame.police,"Valider",couleur_valider_hover);
                           SDL_BlitSurface(myGame.btn_outils->valider,NULL,myGame.screen,&myGame.btn_outils->position_valider);

                       }
                    else
                    {
                        myGame.btn_outils->valider = TTF_RenderText_Blended(myGame.police,"Valider",couleurBlanche);
                        SDL_BlitSurface(myGame.btn_outils->valider,NULL,myGame.screen,&myGame.btn_outils->position_valider);
                    }
                break;

                case SDL_MOUSEBUTTONDOWN:

                    for(i=0 ; i < NB_LIGNE ; i++)
                    {
                        for(j=0 ; j < NB_LIGNE ; j++)
                        {

                            if(event.button.x >= (myGame.plateau->cases[i][j].position.x+myGame.plateau->position.x)
                               && event.button.x <= (myGame.plateau->cases[i][j].position.x+myGame.plateau->position.x+TAILLE_CASE)
                               && event.button.y >= (myGame.plateau->cases[i][j].position.y+myGame.plateau->position.y)
                               && event.button.y <= (myGame.plateau->cases[i][j].position.y+myGame.plateau->position.y+TAILLE_CASE)
                               )
                            {
                                //si on a le droit de cliqué dessus
                                if( can_be_hovered(i,j)  && myGame.plateau->cases[i][j].clicked == false){

                                    myGame.plateau->cases[i][j].clicked=true;
                                    empiler(&myGame.plateau->coordonnee_lettre,i,j);
                                    myGame.plateau->mot_courant[myGame.plateau->taille_mot_courant++]=myGame.plateau->cases[i][j].lettre_face;
                                    myGame.plateau->mot_courant[myGame.plateau->taille_mot_courant]='\0';



                                    SDL_FillRect(myGame.plateau->cases[i][j].surface_case,NULL,couleur_click);
                                    SDL_BlitSurface(myGame.plateau->cases[i][j].caractere,NULL,myGame.plateau->cases[i][j].surface_case,&position);
                                    SDL_BlitSurface(myGame.plateau->cases[i][j].surface_case,NULL,myGame.plateau->surface_plateau,&myGame.plateau->cases[i][j].position);
                                    SDL_BlitSurface(myGame.plateau->surface_plateau,NULL,myGame.screen,&myGame.plateau->position);



                                   //actualiser affichage du mot courant
                                    SDL_FillRect(myGame.btn_outils->surface_mot_courant,NULL,SDL_MapRGB(myGame.screen->format,45,75,20));
                                    myGame.btn_outils->mot_courant=TTF_RenderText_Blended(myGame.police,myGame.plateau->mot_courant,couleurBlanche);
                                    myGame.btn_outils->position_mot_courant.x = myGame.plateau->position.x;
                                    myGame.btn_outils->position_mot_courant.y = myGame.plateau->position.y+TAILLE_PLATEAU+10;
                                    SDL_BlitSurface(myGame.btn_outils->mot_courant,NULL,myGame.btn_outils->surface_mot_courant,&position_relatif);
                                    SDL_BlitSurface(myGame.btn_outils->surface_mot_courant,NULL,myGame.screen,&myGame.btn_outils->position_mot_courant);
                               }
                               //pour retirer le clique
                               else if(myGame.plateau->cases[i][j].clicked == true && myGame.plateau->coordonnee_lettre->coord.i==i && myGame.plateau->coordonnee_lettre->coord.j==j)
                               {

                                    myGame.plateau->cases[i][j].clicked = false;
                                    depiler(&myGame.plateau->coordonnee_lettre);
                                    myGame.plateau->mot_courant[--myGame.plateau->taille_mot_courant]='\0';
                                    SDL_FillRect(myGame.plateau->cases[i][j].surface_case,NULL,couleur_par_defaut);
                                    SDL_BlitSurface(myGame.plateau->cases[i][j].caractere,NULL,myGame.plateau->cases[i][j].surface_case,&position);
                                    SDL_BlitSurface(myGame.plateau->cases[i][j].surface_case,NULL,myGame.plateau->surface_plateau,&myGame.plateau->cases[i][j].position);
                                    SDL_BlitSurface(myGame.plateau->surface_plateau,NULL,myGame.screen,&myGame.plateau->position);


                                     //actualiser affichage du mot courant
                                    SDL_FillRect(myGame.btn_outils->surface_mot_courant,NULL,SDL_MapRGB(myGame.screen->format,45,75,20));
                                    myGame.btn_outils->mot_courant=TTF_RenderText_Blended(myGame.police,myGame.plateau->mot_courant,couleurBlanche);
                                    myGame.btn_outils->position_mot_courant.x = myGame.plateau->position.x;
                                    myGame.btn_outils->position_mot_courant.y = myGame.plateau->position.y+TAILLE_PLATEAU+10;
                                    SDL_BlitSurface(myGame.btn_outils->mot_courant,NULL,myGame.btn_outils->surface_mot_courant,&position_relatif);
                                    SDL_BlitSurface(myGame.btn_outils->surface_mot_courant,NULL,myGame.screen,&myGame.btn_outils->position_mot_courant);
                               }

                            }


                        }
                    }

                    //cliquer sur valide
                    if(event.button.x >= (myGame.btn_outils->position_valider.x)
                               && event.button.x <= (myGame.btn_outils->position_valider.x+myGame.btn_outils->position_valider.w)
                               && event.button.y >= (myGame.btn_outils->position_valider.y)
                               && event.button.y <= (myGame.btn_outils->position_valider.y+myGame.btn_outils->position_valider.h)
                       )
                       {
                           myGame.btn_outils->valider = TTF_RenderText_Blended(myGame.police,"Valider",couleur_valider_hover);
                           SDL_BlitSurface(myGame.btn_outils->valider,NULL,myGame.screen,&myGame.btn_outils->position_valider);

                            valider_mot();

                       }
                break;

                default:
                break;
            }
        }


        temp_actuel = SDL_GetTicks();
        if(temp_actuel-temp_precedent > 1000)
        {
            secondes-=1000;
            strcpy(time,get_time(secondes));

            SDL_FillRect(myGame.btn_outils->surface_time,NULL,SDL_MapRGB(myGame.screen->format,45,75,20));
            myGame.btn_outils->time =  TTF_RenderText_Blended(myGame.police,time,couleurBlanche);
            myGame.btn_outils->position_time.x = myGame.plateau->position.x+myGame.plateau->surface_plateau->w-TAILLE_CASE;
            myGame.btn_outils->position_time.y = myGame.plateau->position.y-TAILLE_CASE/2;
            SDL_BlitSurface(myGame.btn_outils->time,NULL,myGame.btn_outils->surface_time,&position_relatif);
            SDL_BlitSurface(myGame.btn_outils->surface_time,NULL,myGame.screen,&myGame.btn_outils->position_time);

            temp_precedent = temp_actuel;
        }
        SDL_Delay(1);

        SDL_Flip(myGame.screen);
    }
}

void initialisation_game()
{
    SDL_Rect position;



    //initialisation Plateau
    myGame.plateau = initialiser_plateau();



    //collage surface plateau dans l'ecran
    myGame.plateau->position.x=(myGame.screen->w-myGame.plateau->surface_plateau->w)/2;
    myGame.plateau->position.y=TAILLE_CASE;

    SDL_BlitSurface(myGame.plateau->surface_plateau,NULL,myGame.screen,&myGame.plateau->position);


     //initialisation btn_outils
    initialisation_btn_outils();

    SDL_Flip(myGame.screen);

}


void end_game(Game myGame)
{

    liberation_plateau(myGame.plateau);
}


void saisir_mot(char *mot)
{

    do{
        printf("saisir mot: ");
        scanf("%s",mot);

        if(strlen(mot)>8 || strlen(mot) <3)
            printf("mot invalide veuillez saisir a nouveau\n");
    }while(strlen(mot)>8 || strlen(mot) <3 );

}

boolean validation_sur_dictionnaire(char *nom_fichier,char *mot)
{
    boolean trouver = false;
    FILE* dictionnaire = NULL;
    char mot_dictionnaire[50];

    //ouverture du fichier
    dictionnaire = fopen(nom_fichier,"r");
    if(dictionnaire != NULL)
    {

        do{
            fscanf(dictionnaire, "%s" ,mot_dictionnaire);
        }while(!feof(dictionnaire) && strcmp(mot,mot_dictionnaire) != 0);

        if(!strcmp(mot,mot_dictionnaire))
            trouver = true;

        fclose(dictionnaire);

    }else{
        perror("fichier n'est pas ouvert\n");
    }

    return trouver;
}

void initialisation_btn_outils()
{
    SDL_Rect position;
    position.x=0;
    position.y=0;
    SDL_Color couleurBlanche = {255,255,255};
    myGame.btn_outils = Malloc(1,Btn_outils);


    myGame.btn_outils->valider = TTF_RenderText_Blended(myGame.police,"Valider",couleurBlanche);

    //collage bouton valider
    myGame.btn_outils->position_valider.x = myGame.plateau->position.x;
    myGame.btn_outils->position_valider.y = myGame.plateau->position.y+TAILLE_PLATEAU+70;
    SDL_BlitSurface(myGame.btn_outils->valider,NULL,myGame.screen,&myGame.btn_outils->position_valider);

    //surface de score
    myGame.btn_outils->score =  TTF_RenderText_Blended(myGame.police,"score",couleurBlanche);
    myGame.btn_outils->position_score.x = myGame.plateau->position.x;
    myGame.btn_outils->position_score.y = myGame.plateau->position.y-TAILLE_CASE/2;
    SDL_BlitSurface(myGame.btn_outils->score,NULL,myGame.screen,&myGame.btn_outils->position_score);

    //surface time
    myGame.btn_outils->surface_time = SDL_CreateRGBSurface(SDL_HWSURFACE,TAILLE_CASE,35,32,0,0,0,0);
    SDL_FillRect(myGame.btn_outils->surface_time,NULL,SDL_MapRGB(myGame.screen->format,45,75,20));
    myGame.btn_outils->time =  TTF_RenderText_Blended(myGame.police,"3:00",couleurBlanche);
    myGame.btn_outils->position_time.x = myGame.plateau->position.x+myGame.plateau->surface_plateau->w-TAILLE_CASE;
    myGame.btn_outils->position_time.y = myGame.plateau->position.y-TAILLE_CASE/2;
    SDL_BlitSurface(myGame.btn_outils->time,NULL,myGame.btn_outils->surface_time,&position);
    SDL_BlitSurface(myGame.btn_outils->surface_time,NULL,myGame.screen,&myGame.btn_outils->position_time);


    //surface mot en cours
    myGame.btn_outils->surface_mot_courant=SDL_CreateRGBSurface(SDL_HWSURFACE,TAILLE_PLATEAU,35,32,0,0,0,0);
}


boolean valider_mot()
{
    Uint32 couleur_par_defaut=SDL_MapRGB(myGame.screen->format,47,87,145);
    char mot[16];
    int taille_mot=myGame.plateau->taille_mot_courant;
    Coord coord;
    int i=0,j=0;
    SDL_Rect position;

    position.x = (myGame.plateau->cases[i][j].surface_case->w-myGame.plateau->cases[i][j].caractere->w)/2;
    position.y = (myGame.plateau->cases[i][j].surface_case->h-myGame.plateau->cases[i][j].caractere->h)/2;

    mot[taille_mot]='\0';
    while(!pile_vide(myGame.plateau->coordonnee_lettre))
    {
            coord = depiler(&myGame.plateau->coordonnee_lettre);
            myGame.plateau->taille_mot_courant--;

            mot[--taille_mot] = tolower(myGame.plateau->cases[coord.i][coord.j].lettre_face);

    }

    for(i=0 ;i < NB_LIGNE ;i++)
    {
        for(j=0 ;j < NB_COLONNE ;j++)
        {

            myGame.plateau->cases[i][j].clicked = false;
            SDL_FillRect(myGame.plateau->cases[i][j].surface_case,NULL,couleur_par_defaut);
            SDL_BlitSurface(myGame.plateau->cases[i][j].caractere,NULL,myGame.plateau->cases[i][j].surface_case,&position);
            SDL_BlitSurface(myGame.plateau->cases[i][j].surface_case,NULL,myGame.plateau->surface_plateau,&myGame.plateau->cases[i][j].position);
            SDL_BlitSurface(myGame.plateau->surface_plateau,NULL,myGame.screen,&myGame.plateau->position);
        }
    }


    if(validation_sur_dictionnaire("dict.txt",mot))
        return true;
    return false;
}

char* get_time(int secondes)
{
    char time[6];
    char min_c[3];
    char sec_c[3];
    int min=0,sec=0;
    secondes/=1000;
    min = secondes/60;
    sec = secondes%60;

    sprintf(min_c,"%d",min);
    sprintf(sec_c,"%d",sec);

    time[0]='0';
    strcat(time,min_c);
    time[2]=':';
    if(sec<10)
        time[3]='0';
    strcat(time,sec_c);

    return time;
}

void menu_game()
{

    SDL_Surface *menu=NULL;
    SDL_Surface *menu_surface=NULL;
    SDL_Color couleurBlanche={255,255,255};
    boolean continuer = true;
    SDL_Rect position;
    SDL_Event event;

    //initialisation SDl
    myGame.screen = initialisation_sdl();

    //initialisation TTF
    myGame.police = initialisation_ttf("angelina.ttf");


    menu = TTF_RenderText_Blended(myGame.police,"Jouer",couleurBlanche);
    menu_surface=SDL_CreateRGBSurface(SDL_HWSURFACE,3*TAILLE_CASE,TAILLE_CASE,32,0,0,0,0);
    SDL_FillRect(menu_surface,NULL,SDL_MapRGB(myGame.screen->format,255,55,255));

    //position du text dans menu_surface
    position.x = (menu_surface->w-menu->w)/2;
    position.y = (menu_surface->h-menu->h)/2;

    SDL_BlitSurface(menu,NULL,menu_surface,&position);

    //position du menu dans screen
    position.x = (myGame.screen->w-menu_surface->w)/2;
    position.y = (myGame.screen->h-menu_surface->h)/2;

    SDL_BlitSurface(menu_surface,NULL,myGame.screen,&position);

    SDL_EnableUNICODE(1);

    //boucle principale
    while(continuer)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                continuer = false;
            break;

            case SDL_KEYDOWN:
                print_(&event.key);
            break;

            case SDL_MOUSEBUTTONDOWN:

                //clique sur bouton jouer
                if( (event.button.x >= position.x) && (event.button.x<= position.x+menu_surface->w)
                   && (event.button.y >= position.y) && (event.button.y<= position.y+menu_surface->h)
                   ){

                        initialisation_game();
                        play_game(myGame);
                        continuer=false;
                   }
            break;
            default:
            break;
        }

        SDL_Flip(myGame.screen);
    }

    SDL_Quit();
}

void preparation_joueurs()
{
    myGame.joueur[0] = initialisation_joueur("aymen");
    myGame.joueur[1] = initialisation_joueur("youssef");
}
