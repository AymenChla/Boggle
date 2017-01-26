#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <SDL.h>
#include <SDL_ttf.h>

#include "game.h"
#include "pile.h"


char* get_input(SDL_KeyboardEvent *key)
{
    char chaine[2];
    char c = (char) key->keysym.unicode;
    chaine[0]=c;
    chaine[1]='\0';

    return chaine;
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
    SDL_WM_SetIcon(SDL_LoadBMP("img/logo.bmp"), NULL);
    screen = SDL_SetVideoMode(WIDTH_SCREEN,HEIGHT_SCREEN,32,SDL_HWSURFACE | SDL_DOUBLEBUF);
    if(screen == NULL)
        error("impossible de charger le mode video");

    SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,255,255,255));

    SDL_WM_SetCaption("boggle",NULL);

    return screen;
}

TTF_Font* initialisation_ttf(char* nom_police,int size)
{
        TTF_Font *police = NULL;

        if(TTF_Init()== -1){

            fprintf(stderr,"%s: %s\n","impossible d'initialiser la ttf: s\n",TTF_GetError());
            exit(EXIT_FAILURE);
        }


        police = TTF_OpenFont(nom_police,size);
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

void play_game(int num_joueur)
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
    SDL_Surface  *image_background;

    int temp_actuel=0,temp_precedent=0,temp_debut_game=SDL_GetTicks();
    int m_secondes=1000*myGame.nbr_seconde;

    char time[6];
    SDL_Rect position_relatif;
    position_relatif.x=0;
    position_relatif.y=0;


    //position du caractere a l'interieur de son surface pour etre centré
    position.x = (myGame.plateau->cases[i][j].surface_case->w-myGame.plateau->cases[i][j].caractere->w)/2;
    position.y = (myGame.plateau->cases[i][j].surface_case->h-myGame.plateau->cases[i][j].caractere->h)/2;

    //clean interface
    SDL_FillRect(myGame.screen,NULL,SDL_MapRGB(myGame.screen->format,255,255,255));
    image_background = SDL_LoadBMP("img/background_joueur1.bmp");
    position.x=0;
    position.y=0;
    SDL_SetAlpha(image_background, SDL_SRCALPHA, 170);
    SDL_BlitSurface(image_background, NULL, myGame.screen,&position);


    //collage surface plateau dans l'ecran
    SDL_BlitSurface(myGame.plateau->surface_plateau,NULL,myGame.screen,&myGame.plateau->position);

    //collage bouton valider
    SDL_BlitSurface(myGame.btn_outils->valider,NULL,myGame.screen,&myGame.btn_outils->position_valider);

    //collage surface de score
    SDL_FillRect(myGame.btn_outils->score_surface,NULL,SDL_MapRGB(myGame.screen->format,45,75,20));
    SDL_BlitSurface(myGame.btn_outils->score_text,NULL,myGame.btn_outils->score_surface,&position_relatif);
    SDL_BlitSurface(myGame.btn_outils->score_surface,NULL,myGame.screen,&myGame.btn_outils->position_score);

    //collage surfaces time
    SDL_BlitSurface(myGame.btn_outils->time,NULL,myGame.btn_outils->surface_time,&position_relatif);
    SDL_BlitSurface(myGame.btn_outils->surface_time,NULL,myGame.screen,&myGame.btn_outils->position_time);

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

                            //si le mot existe
                            if(valider_mot())
                            {

                                inserer_mot(myGame.joueur[num_joueur]->score,myGame.plateau->mot_courant);
                                char message_score[50];
                                sprintf(message_score,"score: %d (+%d)",myGame.joueur[num_joueur]->score->nb_points,nb_points(myGame.plateau->mot_courant));
                                myGame.btn_outils->score_text =  TTF_RenderText_Blended(myGame.police,message_score,couleurBlanche);
                                SDL_FillRect(myGame.btn_outils->score_surface,NULL,SDL_MapRGB(myGame.screen->format,45,75,20));
                                SDL_BlitSurface(myGame.btn_outils->score_text,NULL,myGame.btn_outils->score_surface,&position_relatif);
                                SDL_BlitSurface(myGame.btn_outils->score_surface,NULL,myGame.screen,&myGame.btn_outils->position_score);

                            }

                       }
                break;

                default:
                break;
            }
        }


        temp_actuel = SDL_GetTicks();
        if(temp_actuel-temp_precedent > 1000)
        {
            m_secondes-=1000;
            strcpy(time,get_time(m_secondes));

            SDL_FillRect(myGame.btn_outils->surface_time,NULL,SDL_MapRGB(myGame.screen->format,45,75,20));
            myGame.btn_outils->time =  TTF_RenderText_Blended(myGame.police,time,couleurBlanche);
            myGame.btn_outils->position_time.x = myGame.plateau->position.x+myGame.plateau->surface_plateau->w-TAILLE_CASE;
            myGame.btn_outils->position_time.y = myGame.plateau->position.y-TAILLE_CASE/2;
            SDL_BlitSurface(myGame.btn_outils->time,NULL,myGame.btn_outils->surface_time,&position_relatif);
            SDL_BlitSurface(myGame.btn_outils->surface_time,NULL,myGame.screen,&myGame.btn_outils->position_time);

            temp_precedent = temp_actuel;
        }
        temp_actuel=SDL_GetTicks();
        if(temp_actuel-temp_debut_game>1000*myGame.nbr_seconde)
            continuer=false;

        SDL_Delay(1);

        SDL_Flip(myGame.screen);
    }
}

void initialisation_game()
{
    //initialisation Plateau
    myGame.plateau = initialiser_plateau();
     //initialisation btn_outils
    myGame.btn_outils= initialisation_btn_outils();

}


void end_game()
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

Btn_outils* initialisation_btn_outils()
{

    SDL_Color couleurBlanche = {255,255,255};
    Btn_outils* btn_outils = Malloc(1,Btn_outils);


    btn_outils->valider = TTF_RenderText_Blended(myGame.police,"Valider",couleurBlanche);

    //initialisation btn valider
    btn_outils->position_valider.x = myGame.plateau->position.x;
    btn_outils->position_valider.y = myGame.plateau->position.y+TAILLE_PLATEAU+70;


    //surface de score
    btn_outils->score_surface = SDL_CreateRGBSurface(SDL_HWSURFACE,200,35,32,0,0,0,0);
    btn_outils->score_text =  TTF_RenderText_Blended(myGame.police,"score: 0",couleurBlanche);
    btn_outils->position_score.x = myGame.plateau->position.x;
    btn_outils->position_score.y = myGame.plateau->position.y-TAILLE_CASE/2;


    //surface time
    btn_outils->surface_time = SDL_CreateRGBSurface(SDL_HWSURFACE,TAILLE_CASE,35,32,0,0,0,0);
    btn_outils->time =  TTF_RenderText_Blended(myGame.police,"3:00",couleurBlanche);
    btn_outils->position_time.x = myGame.plateau->position.x+myGame.plateau->surface_plateau->w-TAILLE_CASE;
    btn_outils->position_time.y = myGame.plateau->position.y-TAILLE_CASE/2;



    //surface mot en cours
    btn_outils->surface_mot_courant=SDL_CreateRGBSurface(SDL_HWSURFACE,TAILLE_PLATEAU,35,32,0,0,0,0);

    return btn_outils;
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

char* get_time(int m_secondes)
{
    char time[6];
    char min_c[3];
    char sec_c[3];
    int min=0,sec=0;
    m_secondes/=1000;
    min = m_secondes/60;
    sec = m_secondes%60;

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


//fonction menu 
void menu_game()
{
    SDL_Surface *menu=NULL,*surface_Nombre_joueur = NULL,*surface_Nombre_time = NULL;
    SDL_Surface *menu_surface=NULL,*Nombre_joueur=NULL,*Nombre_time=NULL;
    SDL_Color couleurBlanche={255,255,255};
    SDL_Color couleurBoggle={65,105,225};
    SDL_Color couleur={0,255,255};
    boolean continuer = true;
    SDL_Rect position,position_jouer,positionArrow;
    SDL_Event event;
    int i,cpt=1;

    //initialisation SDl
    myGame.screen = initialisation_sdl();
    //background interface
    menu = SDL_LoadBMP("img/background_menu.bmp");
    position.x = 0;
    position.y = 0;
    SDL_BlitSurface(menu, NULL, myGame.screen,&position);

    menu = SDL_LoadBMP("img/boggle.bmp");
    SDL_SetColorKey(menu, SDL_SRCCOLORKEY, SDL_MapRGB(menu->format, 0, 0, 0));
    position.x = 60;
    position.y = 100;
    SDL_BlitSurface(menu,NULL,myGame.screen,&position);

     //initialisation du bouton jouer
    myGame.police = initialisation_ttf("fonts/neuropol x rg.ttf",18);
    menu = TTF_RenderText_Blended(myGame.police,"PLAY",couleurBlanche);
    menu_surface=SDL_LoadBMP("img/play_bleu.bmp");


        //position du text dans play_surface
    position_jouer.x = (menu_surface->w-menu->w)/2;
    position_jouer.y = (menu_surface->h-menu->h)/2;
    SDL_BlitSurface(menu,NULL,menu_surface,&position_jouer);
        //position du play dans screen
    position_jouer.x = (myGame.screen->w-menu_surface->w)/2;
    position_jouer.y = myGame.screen->h-menu_surface->h-50;
    SDL_BlitSurface(menu_surface,NULL,myGame.screen,&position_jouer);


        //players and times

    position.x=15;
    position.y=300;
    myGame.police = initialisation_ttf("fonts/neuropol x rg.ttf",22);
    menu = TTF_RenderText_Blended(myGame.police,"Players :",couleurBlanche);
    SDL_BlitSurface(menu,NULL,myGame.screen,&position);
    menu = TTF_RenderText_Blended(myGame.police,"Time :",couleurBlanche);
    position.y+=TAILLE_CASE;
    SDL_BlitSurface(menu,NULL,myGame.screen,&position);

        //position Arrow et interface

    positionArrow.x=TAILLE_CASE*2;
    positionArrow.y=290;

    for(i=0;i<2;i++){
     menu = SDL_LoadBMP("img/left.bmp");
     SDL_SetColorKey(menu, SDL_SRCCOLORKEY, SDL_MapRGB(menu->format, 255, 255, 255));
     SDL_BlitSurface(menu, NULL, myGame.screen,&positionArrow);
     positionArrow.y+=TAILLE_CASE;

    }
    positionArrow.x=TAILLE_CASE*4;
    positionArrow.y=290;

    for(i=0;i<2;i++){
     menu = SDL_LoadBMP("img/right.bmp");
     SDL_SetColorKey(menu, SDL_SRCCOLORKEY, SDL_MapRGB(menu->format, 255, 255, 255));
     menu_surface=SDL_CreateRGBSurface(SDL_HWSURFACE,TAILLE_CASE-35,TAILLE_CASE-35,32,0,0,0,0);
     SDL_BlitSurface(menu, NULL, myGame.screen,&positionArrow);
     positionArrow.y+=TAILLE_CASE;

    }
    positionArrow.x=TAILLE_CASE*2;
    positionArrow.y=290;




    menu= SDL_LoadBMP("img/groups.bmp");
    SDL_SetColorKey(menu, SDL_SRCCOLORKEY, SDL_MapRGB(menu->format, 255, 255, 255));
    position.x=TAILLE_CASE*3-30;
    position.y=290;
    SDL_BlitSurface(menu, NULL, myGame.screen,&position);

    surface_Nombre_joueur=SDL_LoadBMP("img/surface_nombre.bmp");
    Nombre_joueur = TTF_RenderText_Blended(myGame.police,"1",couleur);
    position.x = (surface_Nombre_joueur->w-Nombre_joueur->w)/2;
    position.y = (surface_Nombre_joueur->h-Nombre_joueur->h)/2;
    SDL_BlitSurface(Nombre_joueur,NULL,surface_Nombre_joueur,&position);
    position.x=TAILLE_CASE*3+38;
    position.y=290;
    SDL_BlitSurface(surface_Nombre_joueur,NULL,myGame.screen,&position);
    myGame.nbr_joueur=1;





    menu = SDL_LoadBMP("img/time.bmp");
    SDL_SetColorKey(menu, SDL_SRCCOLORKEY, SDL_MapRGB(menu->format, 255, 255, 255));
    position.x=TAILLE_CASE*3-40;
    position.y=290+TAILLE_CASE-10;
    SDL_BlitSurface(menu, NULL, myGame.screen,&position);





    surface_Nombre_time=SDL_LoadBMP("img/surface_time.bmp");
    Nombre_time= TTF_RenderText_Blended(myGame.police,"1:00",couleur);
    position.x = (surface_Nombre_time->w-Nombre_time->w)/2;
    position.y = (surface_Nombre_time->h-Nombre_time->h)/2+8;
    SDL_BlitSurface(Nombre_time,NULL,surface_Nombre_time,&position);
    position.x=TAILLE_CASE*3+4;
    position.y=300+TAILLE_CASE-8;
    SDL_BlitSurface(surface_Nombre_time,NULL,myGame.screen,&position);
    myGame.nbr_seconde=60;





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
                //print_(&event.key);
                switch(event.key.keysym.sym)
                {
                    case SDLK_KP1:
                        continuer=false;
                    break;

                    case SDLK_KP2:
                    break;

                    default:
                    break;

                }
            break;
            case SDL_MOUSEMOTION:
                if( (event.button.x >= position_jouer.x) && (event.button.x<= position_jouer.x+4*TAILLE_CASE-30)
                   && (event.button.y >= position_jouer.y) && (event.button.y<= position_jouer.y+TAILLE_CASE-10)
                   ){
                         myGame.police = initialisation_ttf("fonts/neuropol x rg.ttf",18);
                         menu = TTF_RenderText_Blended(myGame.police,"PLAY",couleurBlanche);
                         menu_surface=SDL_LoadBMP("img/play_orange.bmp");

                             //position du text dans play_surface
                         position.x = (menu_surface->w-menu->w)/2;
                         position.y = (menu_surface->h-menu->h)/2;
                         SDL_BlitSurface(menu,NULL,menu_surface,&position);
                             //position du play dans screen
                         position.x = (myGame.screen->w-menu_surface->w)/2;
                         position.y = myGame.screen->h-menu_surface->h-50;
                         SDL_BlitSurface(menu_surface,NULL,myGame.screen,&position);



                    }else{
                        myGame.police = initialisation_ttf("fonts/neuropol x rg.ttf",18);
                        menu = TTF_RenderText_Blended(myGame.police,"PLAY",couleurBlanche);
                        menu_surface=SDL_LoadBMP("img/play_bleu.bmp");


                            //position du text dans play_surface
                        position.x = (menu_surface->w-menu->w)/2;
                        position.y = (menu_surface->h-menu->h)/2;
                        SDL_BlitSurface(menu,NULL,menu_surface,&position);
                            //position du play dans screen
                        position.x = (myGame.screen->w-menu_surface->w)/2;
                        position.y = myGame.screen->h-menu_surface->h-50;
                        SDL_BlitSurface(menu_surface,NULL,myGame.screen,&position);
                    }

                    break;
            case SDL_MOUSEBUTTONDOWN:

                //clique sur bouton jouer
                if( (event.button.x >= position_jouer.x) && (event.button.x<= position_jouer.x+4*TAILLE_CASE-30)
                   && (event.button.y >= position_jouer.y) && (event.button.y<= position_jouer.y+TAILLE_CASE-10)
                   ){
                        initialisation_game();

                        //allocation nombre de joueur soit 1 seul joueur ou 2 joueur
                        myGame.joueur = Malloc(myGame.nbr_joueur,Joueur*);
                        for(i=0 ; i < myGame.nbr_joueur ; i++)
                            myGame.joueur[i] = Malloc(1,Joueur);
                        //initialisation de joueur returne le nom de joueur
                        myGame.joueur[0]=initialisation_joueur();
                        fprintf(stdout,"%s\n",myGame.joueur[0]->nom);

                        play_game(0);
                        afficher_score(myGame.joueur[0]->score);
                        continuer=false;
                   }





                   if( (event.button.x >= positionArrow.x) && (event.button.x<= positionArrow.x+TAILLE_CASE-35)
                   && (event.button.y >= positionArrow.y) && (event.button.y<= positionArrow.y+TAILLE_CASE-35)
                   ){

                         surface_Nombre_joueur=SDL_LoadBMP("img/surface_nombre.bmp");
                            Nombre_joueur = TTF_RenderText_Blended(myGame.police,"1",couleur);
                            position.x = (surface_Nombre_joueur->w-Nombre_joueur->w)/2;
                            position.y = (surface_Nombre_joueur->h-Nombre_joueur->h)/2;
                            SDL_BlitSurface(Nombre_joueur,NULL,surface_Nombre_joueur,&position);
                            position.x=TAILLE_CASE*3+38;
                            position.y=290;
                            SDL_BlitSurface(surface_Nombre_joueur,NULL,myGame.screen,&position);
                            myGame.nbr_joueur=1;


                   }

                   if( (event.button.x >= positionArrow.x+2*TAILLE_CASE) && (event.button.x<= positionArrow.x+3*TAILLE_CASE-35)
                   && (event.button.y >= positionArrow.y) && (event.button.y<= positionArrow.y+TAILLE_CASE-35)
                   ){

                           surface_Nombre_joueur=SDL_LoadBMP("img/surface_nombre.bmp");
                            Nombre_joueur = TTF_RenderText_Blended(myGame.police,"2",couleur);
                            position.x = (surface_Nombre_joueur->w-Nombre_joueur->w)/2;
                            position.y = (surface_Nombre_joueur->h-Nombre_joueur->h)/2;
                            SDL_BlitSurface(Nombre_joueur,NULL,surface_Nombre_joueur,&position);
                            position.x=TAILLE_CASE*3+38;
                            position.y=290;
                            SDL_BlitSurface(surface_Nombre_joueur,NULL,myGame.screen,&position);
                            myGame.nbr_joueur=1;

                   }



                    if( (event.button.x >= positionArrow.x) && (event.button.x<= positionArrow.x+TAILLE_CASE-35)
                   && (event.button.y >= positionArrow.y+TAILLE_CASE) && (event.button.y<= positionArrow.y+2*TAILLE_CASE-35)
                   ){
                        SDL_FreeSurface(Nombre_time);

                       if(cpt>1) cpt--;
                        myGame.nbr_seconde=60*cpt;


                    }
                      if( (event.button.x >= positionArrow.x+2*TAILLE_CASE) && (event.button.x<= positionArrow.x+3*TAILLE_CASE-35)
                   && (event.button.y >= positionArrow.y+TAILLE_CASE) && (event.button.y<= positionArrow.y+2*TAILLE_CASE-35)
                   ){
                        SDL_FreeSurface(Nombre_time);
                       if(cpt<5) cpt++;
                        myGame.nbr_seconde=60*cpt;

                   }




                     switch (cpt)
                       {
                        case 1:
                            Nombre_time= TTF_RenderText_Blended(myGame.police,"1:00",couleur);
                            myGame.nbr_seconde=60;
                            break;

                        case 2:
                            Nombre_time= TTF_RenderText_Blended(myGame.police,"2:00",couleur);

                            break;
                        case 3:
                            Nombre_time= TTF_RenderText_Blended(myGame.police,"3:00",couleur);

                            break;
                        case 4:
                            Nombre_time= TTF_RenderText_Blended(myGame.police,"4:00",couleur);
                            break;
                        case 5:
                            Nombre_time= TTF_RenderText_Blended(myGame.police,"5:00",couleur);
                            break;
                       }

                         surface_Nombre_time=SDL_LoadBMP("img/surface_time.bmp");
                        position.x = (surface_Nombre_time->w-Nombre_time->w)/2;
                        position.y = (surface_Nombre_time->h-Nombre_time->h)/2+8;
                        SDL_BlitSurface(Nombre_time,NULL,surface_Nombre_time,&position);
                        position.x=TAILLE_CASE*3+4;
                        position.y=300+TAILLE_CASE-8;
                        SDL_BlitSurface(surface_Nombre_time,NULL,myGame.screen,&position);

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
