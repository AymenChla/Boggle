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

    int i=0,j=0,partie=0;
    SDL_Event event;
    Uint32 couleur_hover=SDL_MapRGB(myGame.plateau->surface_plateau->format,45,78,102);
    Uint32 couleur_click=SDL_MapRGB(myGame.screen->format,0, 68, 204);
    Uint32 couleur_par_defaut=SDL_MapRGB(myGame.screen->format,255,255,255);
    SDL_Color couleur_valider_hover={45,78,102};
    SDL_Color couleurBlanche = {255,255,255};
    SDL_Color couleurRed={255, 0, 0};
    SDL_Color couleur_text = {255,225,225};
    SDL_Rect position,position_time;
    SDL_Surface  *image_background,*plateau_mot;

    int temp_actuel=0,temp_precedent=0,temp_debut_game=SDL_GetTicks();
    int m_secondes=1000*myGame.nbr_seconde;
    int continuer=true;

    char time[6];

    //position du mot courant
    myGame.btn_outils->position_mot_courant.x = myGame.plateau->position.x;
    myGame.btn_outils->position_mot_courant.y = myGame.plateau->position.y+TAILLE_PLATEAU+10;


    //position du caractere a l'interieur de son surface pour etre centré
    position.x = (myGame.plateau->cases[i][j].surface_case->w-myGame.plateau->cases[i][j].caractere->w)/2;
    position.y = (myGame.plateau->cases[i][j].surface_case->h-myGame.plateau->cases[i][j].caractere->h)/2;

    //clean interface
    SDL_FillRect(myGame.screen,NULL,SDL_MapRGB(myGame.screen->format,255,255,255));
    image_background = SDL_LoadBMP("img/background3.bmp");
    position.x=0;
    position.y=0;
    SDL_SetAlpha(image_background, SDL_SRCALPHA, 170);
    SDL_BlitSurface(image_background, NULL, myGame.screen,&position);
    SDL_FreeSurface(image_background);


    //collage surface plateau dans l'ecran
    SDL_BlitSurface(myGame.plateau->surface_plateau,NULL,myGame.screen,&myGame.plateau->position);

    //collage bouton valider
    SDL_BlitSurface(myGame.btn_outils->valider,NULL,myGame.screen,&myGame.btn_outils->position_valider);
    SDL_FreeSurface(myGame.btn_outils->valider);

    //down and up
    plateau_mot = SDL_LoadBMP("img/up.bmp");
    SDL_SetColorKey(plateau_mot, SDL_SRCCOLORKEY, SDL_MapRGB(plateau_mot->format, 0, 0, 0));
    position.x=WIDTH_SCREEN-25;
    position.y=450;
    SDL_BlitSurface(plateau_mot,NULL,myGame.screen,&position);
    SDL_FreeSurface(plateau_mot);
    plateau_mot = SDL_LoadBMP("img/down.bmp");
    SDL_SetColorKey(plateau_mot, SDL_SRCCOLORKEY, SDL_MapRGB(plateau_mot->format, 0, 0, 0));
    position.x=WIDTH_SCREEN-25;
    position.y=TAILLE_PLATEAU+270;
    SDL_BlitSurface(plateau_mot,NULL,myGame.screen,&position);
    SDL_FreeSurface(plateau_mot);


    //creer l'interface plateur des mot affichable
    plateau_mot = SDL_LoadBMP("img/plateau_mots.bmp");
    SDL_SetColorKey(plateau_mot, SDL_SRCCOLORKEY, SDL_MapRGB(plateau_mot->format, 0, 0, 0));
    position.x=130;
    position.y=440;
    SDL_BlitSurface(plateau_mot,NULL,myGame.screen,&position);
    SDL_FreeSurface(plateau_mot);

    myGame.btn_outils->surface_mot_courant = SDL_LoadBMP("img/input.bmp");
    SDL_SetColorKey(myGame.btn_outils->surface_mot_courant, SDL_SRCCOLORKEY, SDL_MapRGB(myGame.btn_outils->surface_mot_courant->format, 0, 0, 0));
    SDL_BlitSurface(myGame.btn_outils->surface_mot_courant,NULL,myGame.screen,&myGame.btn_outils->position_mot_courant);
    SDL_FreeSurface(myGame.btn_outils->surface_mot_courant);



    //collage surface de score
    position.x=6;
    position.y=8;
    SDL_FillRect(myGame.btn_outils->score_surface,NULL,SDL_MapRGB(myGame.screen->format,255, 159, 128));
    SDL_BlitSurface(myGame.btn_outils->score_text,NULL,myGame.btn_outils->score_surface,&position);
    SDL_BlitSurface(myGame.btn_outils->score_surface,NULL,myGame.screen,&myGame.btn_outils->position_score);
    //#SDL_FreeSurface(myGame.btn_outils->score_text);
    position.x = (myGame.plateau->cases[i][j].surface_case->w-myGame.plateau->cases[i][j].caractere->w)/2;
    position.y = (myGame.plateau->cases[i][j].surface_case->h-myGame.plateau->cases[i][j].caractere->h)/2;

    while(!myGame.quitter && continuer)
    {

        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    myGame.quitter=true;
                break;

                case SDL_MOUSEMOTION:

                    for(i=0 ; i < NB_LIGNE ; i++)
                    {
                        for(j=0 ; j < NB_LIGNE ; j++)
                        {
                            position.x = (myGame.plateau->cases[i][j].surface_case->w-myGame.plateau->cases[i][j].caractere->w)/2;
                            position.y = (myGame.plateau->cases[i][j].surface_case->h-myGame.plateau->cases[i][j].caractere->h)/2;
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
                           //SDL_FreeSurface(myGame.btn_outils->valider);

                       }
                    else
                    {
                        myGame.btn_outils->valider = TTF_RenderText_Blended(myGame.police,"Valider",couleurBlanche);
                        SDL_BlitSurface(myGame.btn_outils->valider,NULL,myGame.screen,&myGame.btn_outils->position_valider);
                        //SDL_FreeSurface(myGame.btn_outils->valider);
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
                                    //SDL_FillRect(myGame.btn_outils->surface_mot_courant,NULL,SDL_MapRGB(myGame.screen->format,45,75,20));
                                    myGame.btn_outils->surface_mot_courant = SDL_LoadBMP("img/input.bmp");
                                    SDL_SetColorKey(myGame.btn_outils->surface_mot_courant, SDL_SRCCOLORKEY, SDL_MapRGB(myGame.btn_outils->surface_mot_courant->format, 0, 0, 0));
                                        position.x=12;
                                        position.y=8;
                                    myGame.btn_outils->mot_courant=TTF_RenderText_Blended(myGame.police,myGame.plateau->mot_courant,couleur_text);
                                    SDL_BlitSurface(myGame.btn_outils->mot_courant,NULL,myGame.btn_outils->surface_mot_courant,&position);
                                    SDL_BlitSurface(myGame.btn_outils->surface_mot_courant,NULL,myGame.screen,&myGame.btn_outils->position_mot_courant);
                                    SDL_FreeSurface(myGame.btn_outils->mot_courant);
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
                                    position.x=12;
                                    position.y=8;

                                     //actualiser affichage du mot courant
                                       myGame.btn_outils->surface_mot_courant = SDL_LoadBMP("img/input.bmp");
                                     SDL_SetColorKey(myGame.btn_outils->surface_mot_courant, SDL_SRCCOLORKEY, SDL_MapRGB(myGame.btn_outils->surface_mot_courant->format, 0, 0, 0));

                                    myGame.btn_outils->mot_courant=TTF_RenderText_Blended(myGame.police,myGame.plateau->mot_courant,couleur_text);
                                    SDL_BlitSurface(myGame.btn_outils->mot_courant,NULL,myGame.btn_outils->surface_mot_courant,&position);
                                    SDL_BlitSurface(myGame.btn_outils->surface_mot_courant,NULL,myGame.screen,&myGame.btn_outils->position_mot_courant);
                                    SDL_FreeSurface(myGame.btn_outils->mot_courant);

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
                          // SDL_FreeSurface(myGame.btn_outils->valider);

                            //si le mot existe
                            if(valider_mot() && strlen(myGame.plateau->mot_courant)>2)
                            {

                                partie = 0;
                                inserer_mot(myGame.joueur[num_joueur]->score,myGame.plateau->mot_courant);
                                char message_score[50];
                                sprintf(message_score,"score: %d (+%d)",myGame.joueur[num_joueur]->score->nb_points,nb_points(myGame.plateau->mot_courant));
                                myGame.btn_outils->score_text =  TTF_RenderText_Blended(myGame.police,message_score,couleurRed);
                                position.x=6;
                                position.y=8;
                                SDL_FillRect(myGame.btn_outils->score_surface,NULL,SDL_MapRGB(myGame.screen->format,255, 159, 128));
                                SDL_BlitSurface(myGame.btn_outils->score_text,NULL,myGame.btn_outils->score_surface,&position);
                                SDL_BlitSurface(myGame.btn_outils->score_surface,NULL,myGame.screen,&myGame.btn_outils->position_score);
                                SDL_FreeSurface(myGame.btn_outils->score_text);
                                plateau_mot = SDL_LoadBMP("img/plateau_mots.bmp");
                                SDL_SetColorKey(plateau_mot, SDL_SRCCOLORKEY, SDL_MapRGB(plateau_mot->format, 0, 0, 0));
                                position.x=130;
                                position.y=440;
                                SDL_BlitSurface(plateau_mot,NULL,myGame.screen,&position);
                                SDL_FreeSurface(plateau_mot);
                                afficher_cinq_mot(num_joueur,partie);

                            }

                       }

                    if(event.button.x >= WIDTH_SCREEN-25 &&  event.button.y >=450 && event.button.x <= WIDTH_SCREEN && event.button.y <= 450+25 && partie>0){
                        partie--;
                        plateau_mot = SDL_LoadBMP("img/plateau_mots.bmp");
                        SDL_SetColorKey(plateau_mot, SDL_SRCCOLORKEY, SDL_MapRGB(plateau_mot->format, 0, 0, 0));
                        position.x=130;
                        position.y=440;
                        SDL_BlitSurface(plateau_mot,NULL,myGame.screen,&position);
                        SDL_FreeSurface(plateau_mot);
                        afficher_cinq_mot(num_joueur,partie);

                    }else{

                         if(event.button.x >= WIDTH_SCREEN-25 &&  event.button.y >=TAILLE_PLATEAU+270 && event.button.x <= WIDTH_SCREEN && event.button.y <= TAILLE_PLATEAU+295 && partie*5 < myGame.joueur[num_joueur]->score->nb_mots){
                        partie++;
                        if(partie*5 < myGame.joueur[num_joueur]->score->nb_mots){
                        plateau_mot = SDL_LoadBMP("img/plateau_mots.bmp");
                        SDL_SetColorKey(plateau_mot, SDL_SRCCOLORKEY, SDL_MapRGB(plateau_mot->format, 0, 0, 0));
                        position.x=130;
                        position.y=440;
                        SDL_BlitSurface(plateau_mot,NULL,myGame.screen,&position);
                        SDL_FreeSurface(plateau_mot);
                        afficher_cinq_mot(num_joueur,partie);
                    }

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

            SDL_FillRect(myGame.btn_outils->surface_time,NULL,SDL_MapRGB(myGame.screen->format,255,75,20));
            myGame.btn_outils->time =  TTF_RenderText_Blended(myGame.police,time,couleurBlanche);
            myGame.btn_outils->position_time.x = myGame.plateau->position.x+myGame.plateau->surface_plateau->w-2*TAILLE_CASE+40;
            myGame.btn_outils->position_time.y = myGame.plateau->position.y-TAILLE_CASE+20;
            position_time.x =15;
            position_time.y =8;
            SDL_BlitSurface(myGame.btn_outils->time,NULL,myGame.btn_outils->surface_time,&position_time);
            SDL_BlitSurface(myGame.btn_outils->surface_time,NULL,myGame.screen,&myGame.btn_outils->position_time);
            SDL_FreeSurface(myGame.btn_outils->time);
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
    SDL_Color couleurRed ={255,0,0};
    Btn_outils* btn_outils = Malloc(1,Btn_outils);

    myGame.police = initialisation_ttf("fonts/neuropol x rg.ttf",18);
    btn_outils->valider = TTF_RenderText_Blended(myGame.police,"Valider",couleurBlanche);

    //initialisation btn valider
    btn_outils->position_valider.x = myGame.plateau->position.x;
    btn_outils->position_valider.y = myGame.plateau->position.y+TAILLE_PLATEAU+70;


    //surface de score
    btn_outils->score_surface = SDL_CreateRGBSurface(SDL_HWSURFACE,TAILLE_PLATEAU-2*TAILLE_CASE+40,40,32,0,0,0,0);
    btn_outils->score_text =  TTF_RenderText_Blended(myGame.police,"score: 0",couleurBlanche);
    btn_outils->position_score.x = myGame.plateau->position.x;
    btn_outils->position_score.y = myGame.plateau->position.y-TAILLE_CASE+20;


    //surface time
    btn_outils->surface_time = SDL_CreateRGBSurface(SDL_HWSURFACE,2*TAILLE_CASE-40,40,32,0,0,0,0);

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
    char time[5];
    char min_c[2];
    char sec_c[3];
    int min=0,sec=0;
    m_secondes/=1000;
    min = m_secondes/60;
    sec = m_secondes%60;

    sprintf(min_c,"%d",min);
    sprintf(sec_c,"%d",sec);

    time[0]='\0';
    strcat(time,min_c);
    time[1]=':';
    time[2]='\0';
    if(sec<10)
    {
        time[2]='0';
        time[3]='\0';
    }
    strcat(time,sec_c);

    return time;
}

void afficher_score_board_graphics(int start,int nbr_secondes,int indice_hover)
{


    Joueur *nvJoueur = NULL;
    SDL_Event event;
    SDL_Surface *input_text,*nom_text;
    SDL_Surface *input_box;
   	SDL_Surface *play=NULL;
   	SDL_Surface *play_surface=NULL;
    SDL_Rect position,position_relatif,position_nom_text;
    SDL_Rect position_input_box;
    SDL_Rect position_play;
    SDL_Rect position_temp;




    //requires for score board
    int i;
    char formatage_des_scores[10][500],time[10];
    Tab_Score_board records;
    SDL_Surface *record_surface,*fleche_droite,*fleche_gauche,*surface_Nombre_time,*Nombre_time;
    SDL_Color couleurBlanche = {255, 255, 255};
    SDL_Color couleurJaune = {255, 255, 0};
    SDL_Rect positionArrow_left;
    SDL_Rect positionArrow_right;
    SDL_Rect positionArrow_left_time,positionArrow_right_time;
    char header_mot[20];
    SDL_Surface *header=NULL,*header_surface=NULL,*image_background=NULL;
    SDL_Color couleurNoire = {0,0,0};
    SDL_Color couleurBleu = {65,105,225};
    int width_voir,height_voir;

    //1 background
    image_background = SDL_LoadBMP("img/background_joueur1.bmp");
    position.x=0;
    position.y=0;
    SDL_BlitSurface(image_background, NULL, myGame.screen,&position);
    SDL_FreeSurface(image_background);
    //1 background avec setAlpha
    image_background=SDL_LoadBMP("img/background_score.bmp");
    position.x=0;
    position.y=0;
    SDL_SetAlpha(image_background, SDL_SRCALPHA, 200);
    SDL_BlitSurface(image_background, NULL, myGame.screen,&position);
    SDL_FreeSurface(image_background);


    myGame.police = initialisation_ttf("fonts/ComicNeueSansID.ttf",15);
    header_surface=SDL_CreateRGBSurface(SDL_HWSURFACE,WIDTH_SCREEN,50,32,0,0,0,0);
    SDL_SetAlpha(header_surface, SDL_SRCALPHA, 180);
    SDL_FillRect(header_surface,NULL,SDL_MapRGB(myGame.screen->format, 255, 255, 255));
    //concatenation de header
    strcpy(header_mot,"       Score                ");
    strcat(header_mot,"      time ");
    strcat(header_mot,get_time(nbr_secondes*1000));
    strcat(header_mot,"  4x4");

    header = TTF_RenderText_Blended(myGame.police,header_mot,couleurNoire);
    TTF_CloseFont(myGame.police);
    position.x=15;
    position.y=12;
    SDL_BlitSurface(header,NULL,header_surface,&position);
    SDL_FreeSurface(header);

    position.x=0;
    position.y=0;
    SDL_BlitSurface(header_surface,NULL,myGame.screen,&position);
    SDL_FreeSurface(header_surface);


    //icone header ############## a changer ###############"#"
    header_surface = SDL_LoadBMP("img/star_jaune.bmp");
    SDL_SetColorKey(header_surface, SDL_SRCCOLORKEY, SDL_MapRGB(header_surface->format, 0, 0, 0));
    position.x = 5;
    position.y = 6;
    SDL_BlitSurface(header_surface, NULL, myGame.screen,&position);
    SDL_FreeSurface(header_surface);
    header_surface = SDL_LoadBMP("img/time_user.bmp");
    SDL_SetColorKey(header_surface, SDL_SRCCOLORKEY, SDL_MapRGB(header_surface->format, 255,255,255));
    position.x=175;
    position.y=8;
    SDL_BlitSurface(header_surface, NULL, myGame.screen,&position);
    SDL_FreeSurface(header_surface);
     header_surface = SDL_LoadBMP("img/carre.bmp");
    SDL_SetColorKey(header_surface, SDL_SRCCOLORKEY, SDL_MapRGB(header_surface->format, 255,255,255));
    position.x=WIDTH_SCREEN-45;
    position.y=0;
    SDL_BlitSurface(header_surface, NULL, myGame.screen,&position);
    SDL_FreeSurface(header_surface);


    //navigation time
    myGame.police = initialisation_ttf("fonts/neuropol x rg.ttf",22);
    header_surface = TTF_RenderText_Blended(myGame.police,"Time :",couleurBlanche);
    TTF_CloseFont(myGame.police);
    position.x=40;
    position.y=100;
    SDL_BlitSurface(header_surface,NULL,myGame.screen,&position);
    SDL_FreeSurface(header_surface);
    //affichage des boutons de navigation
     fleche_gauche = SDL_LoadBMP("img/left.bmp");
     SDL_SetColorKey(fleche_gauche, SDL_SRCCOLORKEY, SDL_MapRGB(fleche_gauche->format, 255, 255, 255));
     positionArrow_left_time.x=myGame.screen->w/2-fleche_gauche->w;
     positionArrow_left_time.y=92;
     SDL_BlitSurface(fleche_gauche, NULL, myGame.screen,&positionArrow_left_time);
     SDL_FreeSurface(fleche_gauche);
     fleche_droite = SDL_LoadBMP("img/right.bmp");
     SDL_SetColorKey(fleche_droite, SDL_SRCCOLORKEY, SDL_MapRGB(fleche_droite->format, 255, 255, 255));
     positionArrow_right_time.x=myGame.screen->w/2+fleche_droite->w*2.2;
     positionArrow_right_time.y=92;
     SDL_BlitSurface(fleche_droite, NULL, myGame.screen,&positionArrow_right_time);
     SDL_FreeSurface(fleche_droite);
     //logo time
    header_surface = SDL_LoadBMP("img/time.bmp");
    SDL_SetColorKey(header_surface, SDL_SRCCOLORKEY, SDL_MapRGB(header_surface->format, 255, 255, 255));
    position.x=positionArrow_left_time.x+30;
    position.y=80;
    SDL_BlitSurface(header_surface, NULL, myGame.screen,&position);
    SDL_FreeSurface(header_surface);
    //time value
    myGame.police = initialisation_ttf("fonts/neuropol x rg.ttf",22);
    Nombre_time= TTF_RenderText_Blended(myGame.police,get_time(nbr_secondes*1000),couleurBlanche);
    TTF_CloseFont(myGame.police);
    position.x=positionArrow_left_time.x+90;
    position.y=100;
    SDL_BlitSurface(Nombre_time,NULL,myGame.screen,&position);
    SDL_FreeSurface(Nombre_time);


    //formatage des scores
    myGame.police = initialisation_ttf("fonts/ComicNeueSansID.ttf",15);
    records = best_score_by_time(start,8,nbr_secondes);

    position.y = 180;
    for(i=0 ; i < records.nb_records ; i++)
    {
        //TTF_SetFontStyle(myGame.police,TTF_STYLE_NORMAL);

        position.x = 15;

        //affichage nom
        strcpy(formatage_des_scores[i],records.records[i].nom_joueur);
        record_surface = TTF_RenderText_Blended(myGame.police,formatage_des_scores[i],couleurJaune);
        SDL_BlitSurface(record_surface,NULL,myGame.screen,&position);
        SDL_FreeSurface(record_surface);

        position.x+=120;
        //affichage score
        sprintf(formatage_des_scores[i],"%d",records.records[i].score.nb_points);
        record_surface = TTF_RenderText_Blended(myGame.police,formatage_des_scores[i],couleurJaune);
        SDL_BlitSurface(record_surface,NULL,myGame.screen,&position);
        SDL_FreeSurface(record_surface);

         position.x+=50;
         //affichage vs
        strcpy(formatage_des_scores[i],records.records[i].vs);
        record_surface = TTF_RenderText_Blended(myGame.police,formatage_des_scores[i],couleurJaune);
        SDL_BlitSurface(record_surface,NULL,myGame.screen,&position);
        SDL_FreeSurface(record_surface);

        position.x+=120;
        //affichage voir
        strcpy(formatage_des_scores[i],"voir");


        record_surface = TTF_RenderText_Blended(myGame.police,formatage_des_scores[i],couleurJaune);
        width_voir = record_surface->w;
        height_voir = record_surface->h;
        SDL_BlitSurface(record_surface,NULL,myGame.screen,&position);
        SDL_FreeSurface(record_surface);


        position.y+=40;
    }

    /*//affichage des scores
    position.x = 40;
    position.y = 200;
    myGame.police = initialisation_ttf("fonts/neuropol x rg.ttf",16);
    //TTF_SetFontStyle(myGame.police,TTF_STYLE_UNDERLINE);
    for(i=0 ;i < records.nb_records ; i++)
    {
        record_surface = TTF_RenderText_Blended(myGame.police,formatage_des_scores[i],couleurJaune);
        SDL_BlitSurface(record_surface,NULL,myGame.screen,&position);
        SDL_FreeSurface(record_surface);

        position.y+=50;
    }*/
    TTF_CloseFont(myGame.police);


    //affichage des boutons de navigation
     fleche_gauche = SDL_LoadBMP("img/left.bmp");
     SDL_SetColorKey(fleche_gauche, SDL_SRCCOLORKEY, SDL_MapRGB(fleche_gauche->format, 255, 255, 255));
     positionArrow_left.x=myGame.screen->w/2-fleche_gauche->w*2;
     positionArrow_left.y=myGame.screen->h-TAILLE_CASE;
     SDL_BlitSurface(fleche_gauche, NULL, myGame.screen,&positionArrow_left);
     //SDL_FreeSurface(fleche_gauche);
     fleche_droite = SDL_LoadBMP("img/right.bmp");
     SDL_SetColorKey(fleche_droite, SDL_SRCCOLORKEY, SDL_MapRGB(fleche_droite->format, 255, 255, 255));
     positionArrow_right.x=myGame.screen->w/2+fleche_droite->w;
     positionArrow_right.y=myGame.screen->h-TAILLE_CASE;
     SDL_BlitSurface(fleche_droite, NULL, myGame.screen,&positionArrow_right);
     //SDL_FreeSurface(fleche_droite);


    SDL_Flip(myGame.screen);
    while(!myGame.quitter)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                myGame.quitter=true;
            break;

            case SDL_MOUSEBUTTONDOWN:

                //clique sur navigation droite
                if( (event.button.x >= positionArrow_right.x) && (event.button.x<= positionArrow_right.x+fleche_droite->w)
                   && (event.button.y >= positionArrow_right.y) && (event.button.y<= positionArrow_right.y+fleche_droite->h)
                   ){

                      //liberation
                       SDL_FreeSurface(fleche_droite);
                       SDL_FreeSurface(fleche_gauche);
                       afficher_score_board_graphics(start+5,nbr_secondes,indice_hover);


                   }
                   //clique sur navigation gauche
                if( (event.button.x >= positionArrow_left.x) && (event.button.x<= positionArrow_left.x+fleche_gauche->w)
                   && (event.button.y >= positionArrow_left.y) && (event.button.y<= positionArrow_left.y+fleche_gauche->h)
                   ){
                       if(start-5>0)
                       {

                           //liberation
                           SDL_FreeSurface(fleche_droite);
                           SDL_FreeSurface(fleche_gauche);
                           afficher_score_board_graphics(start-5,nbr_secondes,-1);

                   }
               }
                       //fleche gauche time
                    if( (event.button.x >= positionArrow_left_time.x) && (event.button.x<= positionArrow_left_time.x+fleche_gauche->w)
                    && (event.button.y >= positionArrow_left_time.y) && (event.button.y<= positionArrow_left_time.y+fleche_gauche->h)
                   ){

                        if(nbr_secondes>60)
                        {

                           //liberation
                           SDL_FreeSurface(fleche_droite);
                           SDL_FreeSurface(fleche_gauche);
                           afficher_score_board_graphics(1,nbr_secondes-60,-1);

                        }

                   }
                   //fleche gauche time
                    if( (event.button.x >= positionArrow_right_time.x) && (event.button.x<= positionArrow_right_time.x+fleche_gauche->w)
                    && (event.button.y >= positionArrow_right_time.y) && (event.button.y<= positionArrow_right_time.y+fleche_gauche->h)
                   ){

                        if(nbr_secondes<5*60)
                        {

                           //liberation
                           SDL_FreeSurface(fleche_droite);
                           SDL_FreeSurface(fleche_gauche);
                            afficher_score_board_graphics(1,nbr_secondes+60,-1);

                        }

                   }


                for(i=0 ; i< records.nb_records ;i++)
                {
                    if((event.motion.x >= 305 && event.motion.x <= 305 + width_voir)
                        && (event.motion.y >= 180 + i*40 && event.motion.y <= 180 + i*40+ height_voir))
                        {
                                //liberation
                           SDL_FreeSurface(fleche_droite);
                           SDL_FreeSurface(fleche_gauche);
                           afficher_resultat(records.records[i]);

                        }
                }


             break;

            default:
            break;
        }


        SDL_Delay(1);
        SDL_Flip(myGame.screen);
    }
    //liberation
}
void affiche_help(){


    SDL_Surface *help_surface,*help;
    SDL_Rect position;
    SDL_Event event;
    int page = 1;

    SDL_FillRect(myGame.screen,NULL,SDL_MapRGB(myGame.screen->format,255,255,255));




    help=SDL_LoadBMP("img/help1.bmp");
    position.x=10;
    position.y=5;

    SDL_BlitSurface(help, NULL, myGame.screen,&position);
    SDL_Flip(myGame.screen);
    SDL_FreeSurface(help);

    while(myGame.quitter!=true)
    {
       SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                myGame.quitter=true;

            break;
             case SDL_MOUSEBUTTONDOWN:

                //clique sur bouton jouer
                if( (event.button.x >= WIDTH_SCREEN-60) && (event.button.x<= WIDTH_SCREEN)
                   && (event.button.y >= 0) && (event.button.y<= 40)
                   ){

                    if(page==1){
                        SDL_FillRect(myGame.screen,NULL,SDL_MapRGB(myGame.screen->format,255,255,255));
                        help=SDL_LoadBMP("img/help2.bmp");
                        position.x=10;
                        position.y=5;
                        SDL_BlitSurface(help, NULL, myGame.screen,&position);
                        SDL_Flip(myGame.screen);
                        SDL_FreeSurface(help);
                        page=2;
                    }else{
                        SDL_FillRect(myGame.screen,NULL,SDL_MapRGB(myGame.screen->format,255,255,255));
                        help=SDL_LoadBMP("img/help1.bmp");
                        position.x=10;
                        position.y=5;
                        SDL_BlitSurface(help, NULL, myGame.screen,&position);
                        SDL_Flip(myGame.screen);
                        SDL_FreeSurface(help);
                        page=1;


                    }

                }
                if( (event.button.x >= 0) && (event.button.x<= 100)
                   && (event.button.y >= 0) && (event.button.y<= 40)
                   ){


                    menu_game();
                }

                break;

        }


    }

}
//fonction menu
void menu_game()
{


    //ab_Score_board rec =get_all_scores();
    //afficher_score_board(rec);

    SDL_Surface *menu=NULL,*surface_Nombre_joueur = NULL,*surface_Nombre_time = NULL;
    SDL_Surface *menu_surface=NULL,*Nombre_joueur=NULL,*Nombre_time=NULL;

    SDL_Color couleurBlanche={255,255,255};
    SDL_Color couleurJaune = {255, 255, 0};
    SDL_Color couleur={0,255,255};
    SDL_Rect position,position_jouer,positionArrow;
    SDL_Event event;
    int i,j,cpt=1,id_courant;


    //initialisation SDl
    myGame.screen = initialisation_sdl();
    //background interface
    menu = SDL_LoadBMP("img/background_menu.bmp");
    position.x = 0;
    position.y = 0;
    SDL_BlitSurface(menu, NULL, myGame.screen,&position);
    SDL_FreeSurface(menu);

    menu = SDL_LoadBMP("img/boggle.bmp");
    SDL_SetColorKey(menu, SDL_SRCCOLORKEY, SDL_MapRGB(menu->format, 0, 0, 0));
    position.x = 60;
    position.y = 100;
    SDL_BlitSurface(menu,NULL,myGame.screen,&position);
    SDL_FreeSurface(menu);


    //initialisation du bouton score and help

    menu_surface = SDL_CreateRGBSurface(SDL_HWSURFACE,355/3,45,32,0,0,0,0);
    SDL_FillRect(menu_surface,NULL,SDL_MapRGB(myGame.screen->format, 39, 62, 255));
    SDL_SetAlpha(menu_surface, SDL_SRCALPHA, 170);
    menu = SDL_LoadBMP("img/star.bmp");
    SDL_SetColorKey(menu, SDL_SRCCOLORKEY, SDL_MapRGB(menu->format, 0, 0, 0));
    position.x = 5;
    position.y = 6;
    SDL_BlitSurface(menu,NULL,menu_surface,&position);
    SDL_FreeSurface(menu);
    myGame.police = initialisation_ttf("fonts/neuropol x rg.ttf",12);
    menu = TTF_RenderText_Blended(myGame.police,"high score",couleurBlanche);
    TTF_CloseFont(myGame.police);
    position.x = 32;
    position.y = 12;
    SDL_BlitSurface(menu,NULL,menu_surface,&position);
    SDL_FreeSurface(menu);


    position.x = 0;
    position.y = 0;
    SDL_BlitSurface(menu_surface,NULL,myGame.screen,&position);
    SDL_FreeSurface(menu_surface);

    menu_surface = SDL_CreateRGBSurface(SDL_HWSURFACE,355/3,45,32,0,0,0,0);
    SDL_FillRect(menu_surface,NULL,SDL_MapRGB(myGame.screen->format, 23, 97, 232));
    SDL_SetAlpha(menu_surface, SDL_SRCALPHA, 170);
    position.x = 355/3;
    position.y = 0;
    SDL_BlitSurface(menu_surface,NULL,myGame.screen,&position);
    SDL_FreeSurface(menu_surface);

    menu_surface = SDL_CreateRGBSurface(SDL_HWSURFACE,355/3,45,32,0,0,0,0);
    SDL_FillRect(menu_surface,NULL,SDL_MapRGB(myGame.screen->format, 26, 163, 255));
    SDL_SetAlpha(menu_surface, SDL_SRCALPHA, 170);
    menu = SDL_LoadBMP("img/help.bmp");
    SDL_SetColorKey(menu, SDL_SRCCOLORKEY, SDL_MapRGB(menu->format, 0, 0, 0));
    position.x = 5;
    position.y = 6;
    SDL_BlitSurface(menu,NULL,menu_surface,&position);
    SDL_FreeSurface(menu);
    myGame.police = initialisation_ttf("fonts/neuropol x rg.ttf",12);
    menu = TTF_RenderText_Blended(myGame.police,"help",couleurBlanche);
    TTF_CloseFont(myGame.police);
    position.x = 42;
    position.y = 12;
    SDL_BlitSurface(menu,NULL,menu_surface,&position);
    SDL_FreeSurface(menu);
    position.x = 2*355/3;
    position.y = 0;
    SDL_BlitSurface(menu_surface,NULL,myGame.screen,&position);
    SDL_FreeSurface(menu_surface);




     //initialisation du bouton jouer
    myGame.police = initialisation_ttf("fonts/neuropol x rg.ttf",18);
    menu = TTF_RenderText_Blended(myGame.police,"PLAY",couleurBlanche);
    TTF_CloseFont(myGame.police);

    menu_surface=SDL_LoadBMP("img/play_bleu.bmp");


        //position du text dans play_surface
    position_jouer.x = (menu_surface->w-menu->w)/2;
    position_jouer.y = (menu_surface->h-menu->h)/2;
    SDL_BlitSurface(menu,NULL,menu_surface,&position_jouer);
    SDL_FreeSurface(menu);
        //position du play dans screen
    position_jouer.x = (myGame.screen->w-menu_surface->w)/2;
    position_jouer.y = myGame.screen->h-menu_surface->h-50;
    SDL_BlitSurface(menu_surface,NULL,myGame.screen,&position_jouer);
    SDL_FreeSurface(menu_surface);


        //players and times

    position.x=15;
    position.y=300;
    myGame.police = initialisation_ttf("fonts/neuropol x rg.ttf",22);
    menu = TTF_RenderText_Blended(myGame.police,"Players :",couleurBlanche);
    SDL_BlitSurface(menu,NULL,myGame.screen,&position);
    SDL_FreeSurface(menu);
    menu = TTF_RenderText_Blended(myGame.police,"Time :",couleurBlanche);
    position.y+=TAILLE_CASE;
    SDL_BlitSurface(menu,NULL,myGame.screen,&position);
    SDL_FreeSurface(menu);

        //position Arrow et interface

    positionArrow.x=TAILLE_CASE*2;
    positionArrow.y=290;

     menu = SDL_LoadBMP("img/left.bmp");
     SDL_SetColorKey(menu, SDL_SRCCOLORKEY, SDL_MapRGB(menu->format, 255, 255, 255));

    for(i=0;i<2;i++){

     SDL_BlitSurface(menu, NULL, myGame.screen,&positionArrow);
     positionArrow.y+=TAILLE_CASE;

    }
    SDL_FreeSurface(menu);
    positionArrow.x=TAILLE_CASE*4;
    positionArrow.y=290;


     menu = SDL_LoadBMP("img/right.bmp");
     SDL_SetColorKey(menu, SDL_SRCCOLORKEY, SDL_MapRGB(menu->format, 255, 255, 255));
    // menu_surface=SDL_CreateRGBSurface(SDL_HWSURFACE,TAILLE_CASE-35,TAILLE_CASE-35,32,0,0,0,0);
    for(i=0;i<2;i++){
     SDL_BlitSurface(menu, NULL, myGame.screen,&positionArrow);
     positionArrow.y+=TAILLE_CASE;

    }
    SDL_FreeSurface(menu);
    positionArrow.x=TAILLE_CASE*2;
    positionArrow.y=290;


    menu= SDL_LoadBMP("img/groups.bmp");
    SDL_SetColorKey(menu, SDL_SRCCOLORKEY, SDL_MapRGB(menu->format, 255, 255, 255));
    position.x=TAILLE_CASE*3-30;
    position.y=290;
    SDL_BlitSurface(menu, NULL, myGame.screen,&position);
    SDL_FreeSurface(menu);

    surface_Nombre_joueur=SDL_LoadBMP("img/surface_nombre.bmp");
    Nombre_joueur = TTF_RenderText_Blended(myGame.police,"1",couleur);

    position.x = (surface_Nombre_joueur->w-Nombre_joueur->w)/2;
    position.y = (surface_Nombre_joueur->h-Nombre_joueur->h)/2;
    SDL_BlitSurface(Nombre_joueur,NULL,surface_Nombre_joueur,&position);
    SDL_FreeSurface(Nombre_joueur);
    position.x=TAILLE_CASE*3+38;
    position.y=290;
    SDL_BlitSurface(surface_Nombre_joueur,NULL,myGame.screen,&position);
    SDL_FreeSurface(surface_Nombre_joueur);
    myGame.nbr_joueur=1;





    menu = SDL_LoadBMP("img/time.bmp");
    SDL_SetColorKey(menu, SDL_SRCCOLORKEY, SDL_MapRGB(menu->format, 255, 255, 255));
    position.x=TAILLE_CASE*3-40;
    position.y=290+TAILLE_CASE-10;
    SDL_BlitSurface(menu, NULL, myGame.screen,&position);
    SDL_FreeSurface(menu);


    surface_Nombre_time=SDL_LoadBMP("img/surface_time.bmp");
    Nombre_time= TTF_RenderText_Blended(myGame.police,"1:00",couleur);
    TTF_CloseFont(myGame.police);
    position.x = (surface_Nombre_time->w-Nombre_time->w)/2;
    position.y = (surface_Nombre_time->h-Nombre_time->h)/2+8;
    SDL_BlitSurface(Nombre_time,NULL,surface_Nombre_time,&position);
    SDL_FreeSurface(Nombre_time);
    position.x=TAILLE_CASE*3+4;
    position.y=300+TAILLE_CASE-8;
    SDL_BlitSurface(surface_Nombre_time,NULL,myGame.screen,&position);
    SDL_FreeSurface(surface_Nombre_time);
    myGame.nbr_seconde=60;



    SDL_EnableUNICODE(1);

    //boucle principale
    myGame.quitter=false;
    while(!myGame.quitter)
    {

        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                myGame.quitter=true;
            break;

            break;
            case SDL_MOUSEMOTION:

                //si on passe la souris sur high score
                if( (event.button.x >= 32) && (event.button.x<= 32+95)
                   && (event.button.y >= 12) && (event.button.y<= 12+15)
                   ){


                        myGame.police = initialisation_ttf("fonts/neuropol x rg.ttf",12);
                        menu = TTF_RenderText_Blended(myGame.police,"high score",couleurJaune);
                        TTF_CloseFont(myGame.police);
                        position.x = 32;
                        position.y = 12;
                        SDL_BlitSurface(menu,NULL,myGame.screen,&position);
                        SDL_FreeSurface(menu);

                   }
                   else
                   {
                        myGame.police = initialisation_ttf("fonts/neuropol x rg.ttf",12);
                        menu = TTF_RenderText_Blended(myGame.police,"high score",couleurBlanche);
                        TTF_CloseFont(myGame.police);
                        position.x = 32;
                        position.y = 12;
                        SDL_BlitSurface(menu,NULL,myGame.screen,&position);
                        SDL_FreeSurface(menu);
                   }

                if( (event.button.x >= position_jouer.x) && (event.button.x<= position_jouer.x+4*TAILLE_CASE-30)
                   && (event.button.y >= position_jouer.y) && (event.button.y<= position_jouer.y+TAILLE_CASE-10)
                   ){

                         myGame.police = initialisation_ttf("fonts/neuropol x rg.ttf",18);
                         menu = TTF_RenderText_Blended(myGame.police,"PLAY",couleurBlanche);
                         TTF_CloseFont(myGame.police);
                         menu_surface=SDL_LoadBMP("img/play_orange.bmp");

                             //position du text dans play_surface
                         position.x = (menu_surface->w-menu->w)/2;
                         position.y = (menu_surface->h-menu->h)/2;
                         SDL_BlitSurface(menu,NULL,menu_surface,&position);
                         SDL_FreeSurface(menu);
                             //position du play dans screen
                         position.x = (myGame.screen->w-menu_surface->w)/2;
                         position.y = myGame.screen->h-menu_surface->h-50;
                         SDL_BlitSurface(menu_surface,NULL,myGame.screen,&position);
                         SDL_FreeSurface(menu_surface);


                    }else{

                        myGame.police = initialisation_ttf("fonts/neuropol x rg.ttf",18);
                        menu = TTF_RenderText_Blended(myGame.police,"PLAY",couleurBlanche);
                        TTF_CloseFont(myGame.police);
                        menu_surface=SDL_LoadBMP("img/play_bleu.bmp");


                            //position du text dans play_surface
                        position.x = (menu_surface->w-menu->w)/2;
                        position.y = (menu_surface->h-menu->h)/2;
                        SDL_BlitSurface(menu,NULL,menu_surface,&position);
                        SDL_FreeSurface(menu);
                            //position du play dans screen
                        position.x = (myGame.screen->w-menu_surface->w)/2;
                        position.y = myGame.screen->h-menu_surface->h-50;
                        SDL_BlitSurface(menu_surface,NULL,myGame.screen,&position);
                        SDL_FreeSurface(menu_surface);
                    }

                    break;
            case SDL_MOUSEBUTTONDOWN:
                //si on clique sur high score
                if( (event.button.x >= 32) && (event.button.x<= 32+95)
                   && (event.button.y >= 12) && (event.button.y<= 12+15)
                   ){
                       afficher_score_board_graphics(1,1,-1);
                   }
                //clique sur bouton jouer
                if( (event.button.x >= position_jouer.x) && (event.button.x<= position_jouer.x+4*TAILLE_CASE-30)
                   && (event.button.y >= position_jouer.y) && (event.button.y<= position_jouer.y+TAILLE_CASE-10)
                   ){

                        id_courant = recuperer_id_courant();
                        id_courant++;

                        //font of the game
                        myGame.police = initialisation_ttf("fonts/neuropol x rg.ttf",18);
                        initialisation_game();
                        TTF_CloseFont(myGame.police);

            myGame.nbr_seconde=1;
                        //allocation nombre de joueur soit 1 seul joueur ou 2 joueur
                        myGame.joueur = Malloc(myGame.nbr_joueur,Joueur*);
                        for(i=0 ; i < myGame.nbr_joueur ; i++)
                            myGame.joueur[i] = Malloc(1,Joueur);

                        //boucler sur le nombre de joueur
                        for(i=0 ; i < myGame.nbr_joueur && myGame.quitter==false ;i++)
                        {


                            //initialisation de joueur return le nom de joueur
                            myGame.joueur[i]=initialisation_joueur();


                            myGame.police = initialisation_ttf("fonts/neuropol x rg.ttf",18);

                            //si le joueur n'a pas quitter le jeu
                            if(myGame.quitter==false)
                             play_game(i);

                            //actualiser mot courant
                              while(!pile_vide(myGame.plateau->coordonnee_lettre))
                                     depiler(&myGame.plateau->coordonnee_lettre);

                           TTF_CloseFont(myGame.police);
                        }

                            if(myGame.quitter==false)
                            {
                                if(myGame.nbr_joueur==1)
                                    sauvegarder_score_trier(myGame.joueur[0]->nom,"solo",myGame.joueur[0]->score,myGame.nbr_seconde,id_courant);
                                else
                                {

                                    sauvegarder_score_trier(myGame.joueur[0]->nom,myGame.joueur[1]->nom,myGame.joueur[0]->score,myGame.nbr_seconde,id_courant);
                                    sauvegarder_score_trier(myGame.joueur[1]->nom,myGame.joueur[0]->nom,myGame.joueur[1]->score,myGame.nbr_seconde,id_courant);

                                }

                            }

                             if(myGame.nbr_joueur)
                            {
                                Score_board record;
                                record.id_partie = id_courant;
                                strcpy(record.nom_joueur,myGame.joueur[0]->nom);
                                strcpy(record.vs,"solo");
                                record.score.nb_mots=0;
                                record.score.nb_points=0;
                                record.score.teteMots=NULL;
                                Mot *mot_temp = NULL;

                                mot_temp = myGame.joueur[0]->score->teteMots;

                                while(mot_temp!=NULL)
                                {
                                    inserer_mot(&record.score,mot_temp->info->mot);
                                    mot_temp=mot_temp->suivant;
                                }
                                
                                afficher_resultat(record);
                            }

                   }





                   if( (event.button.x >= positionArrow.x) && (event.button.x<= positionArrow.x+TAILLE_CASE-35)
                   && (event.button.y >= positionArrow.y) && (event.button.y<= positionArrow.y+TAILLE_CASE-35)
                   ){



                            myGame.police = initialisation_ttf("fonts/neuropol x rg.ttf",22);
                            surface_Nombre_joueur=SDL_LoadBMP("img/surface_nombre.bmp");
                            Nombre_joueur = TTF_RenderText_Blended(myGame.police,"1",couleur);
                            TTF_CloseFont(myGame.police);
                            position.x = (surface_Nombre_joueur->w-Nombre_joueur->w)/2;
                            position.y = (surface_Nombre_joueur->h-Nombre_joueur->h)/2;
                            SDL_BlitSurface(Nombre_joueur,NULL,surface_Nombre_joueur,&position);
                            SDL_FreeSurface(Nombre_joueur);
                            position.x=TAILLE_CASE*3+38;
                            position.y=290;
                            SDL_BlitSurface(surface_Nombre_joueur,NULL,myGame.screen,&position);
                            SDL_FreeSurface(surface_Nombre_joueur);
                            myGame.nbr_joueur=1;



                   }

                   if( (event.button.x >= positionArrow.x+2*TAILLE_CASE) && (event.button.x<= positionArrow.x+3*TAILLE_CASE-35)
                   && (event.button.y >= positionArrow.y) && (event.button.y<= positionArrow.y+TAILLE_CASE-35)
                   ){

                            myGame.police = initialisation_ttf("fonts/neuropol x rg.ttf",22);
                            surface_Nombre_joueur=SDL_LoadBMP("img/surface_nombre.bmp");
                            Nombre_joueur = TTF_RenderText_Blended(myGame.police,"2",couleur);
                            TTF_CloseFont(myGame.police);
                            position.x = (surface_Nombre_joueur->w-Nombre_joueur->w)/2;
                            position.y = (surface_Nombre_joueur->h-Nombre_joueur->h)/2;
                            SDL_BlitSurface(Nombre_joueur,NULL,surface_Nombre_joueur,&position);
                            SDL_FreeSurface(Nombre_joueur);
                            position.x=TAILLE_CASE*3+38;
                            position.y=290;
                            SDL_BlitSurface(surface_Nombre_joueur,NULL,myGame.screen,&position);
                            SDL_FreeSurface(surface_Nombre_joueur);
                            myGame.nbr_joueur=2;

                   }



                    if( (event.button.x >= positionArrow.x) && (event.button.x<= positionArrow.x+TAILLE_CASE-35)
                   && (event.button.y >= positionArrow.y+TAILLE_CASE) && (event.button.y<= positionArrow.y+2*TAILLE_CASE-35)
                   ){

                       if(cpt>1) cpt--;
                        myGame.nbr_seconde=60*cpt;

                        myGame.police = initialisation_ttf("fonts/neuropol x rg.ttf",22);
                        Nombre_time= TTF_RenderText_Blended(myGame.police,get_time(myGame.nbr_seconde*1000),couleur);
                        TTF_CloseFont(myGame.police);
                         surface_Nombre_time=SDL_LoadBMP("img/surface_time.bmp");
                        position.x = (surface_Nombre_time->w-Nombre_time->w)/2;
                        position.y = (surface_Nombre_time->h-Nombre_time->h)/2+8;
                        SDL_BlitSurface(Nombre_time,NULL,surface_Nombre_time,&position);
                        SDL_FreeSurface(Nombre_time);
                        position.x=TAILLE_CASE*3+4;
                        position.y=300+TAILLE_CASE-8;
                        SDL_BlitSurface(surface_Nombre_time,NULL,myGame.screen,&position);
                        SDL_FreeSurface(surface_Nombre_time);


                    }
                      if( (event.button.x >= positionArrow.x+2*TAILLE_CASE) && (event.button.x<= positionArrow.x+3*TAILLE_CASE-35)
                   && (event.button.y >= positionArrow.y+TAILLE_CASE) && (event.button.y<= positionArrow.y+2*TAILLE_CASE-35)
                   ){

                       if(cpt<5) cpt++;
                        myGame.nbr_seconde=60*cpt;


                        myGame.police = initialisation_ttf("fonts/neuropol x rg.ttf",22);
                        Nombre_time= TTF_RenderText_Blended(myGame.police,get_time(myGame.nbr_seconde*1000),couleur);
                        TTF_CloseFont(myGame.police);
                         surface_Nombre_time=SDL_LoadBMP("img/surface_time.bmp");

                        position.x = (surface_Nombre_time->w-Nombre_time->w)/2;
                        position.y = (surface_Nombre_time->h-Nombre_time->h)/2+8;
                        SDL_BlitSurface(Nombre_time,NULL,surface_Nombre_time,&position);
                        SDL_FreeSurface(Nombre_time);
                        position.x=TAILLE_CASE*3+4;
                        position.y=300+TAILLE_CASE-8;
                        SDL_BlitSurface(surface_Nombre_time,NULL,myGame.screen,&position);
                        SDL_FreeSurface(surface_Nombre_time);

                   }
                    if(event.button.x >= 2*355/3  && event.button.y >= 0 && event.button.x <= 2*355/3+355/3 && event.button.y <=45){

                        affiche_help();
                   }

            break;

            default:
            break;
        }

        SDL_Flip(myGame.screen);
    }

    //liberation
    SDL_FreeSurface(myGame.screen);
    //TTF_CloseFont(myGame.police);
    TTF_Quit();
    SDL_Quit();
}

void afficher_resultat(Score_board record)
{


    Joueur *nvJoueur = NULL;
    SDL_Event event;
    SDL_Surface *input_text,*nom_text;
    SDL_Surface *input_box;
    SDL_Surface *play=NULL;
    SDL_Surface *play_surface=NULL;
    SDL_Rect position,position_relatif,position_nom_text;
    SDL_Rect position_input_box;
    SDL_Rect position_play;
    SDL_Rect position_temp;


    //requires for score board
    int i;
    char formatage_des_scores[10][500],time[10];
    Tab_Score_board records;
    SDL_Surface *record_surface,*fleche_droite,*fleche_gauche,*surface_Nombre_time,*Nombre_time;
    SDL_Color couleurBlanche = {255, 255, 255};
    SDL_Color couleurJaune = {255, 255, 0};
    SDL_Rect positionArrow_left;
    SDL_Rect positionArrow_right;
    SDL_Rect positionArrow_left_time,positionArrow_right_time;
    char header_mot[50];
    SDL_Surface *header=NULL,*header_surface=NULL,*image_background=NULL;
    SDL_Color couleurNoire = {0,0,0};
    SDL_Color couleurBleu = {65,105,225};
    int width_voir,height_voir;
    SDL_Surface *chaine=NULL;

    //1 background
    image_background = SDL_LoadBMP("img/background_joueur1.bmp");
    position.x=0;
    position.y=0;
    SDL_BlitSurface(image_background, NULL, myGame.screen,&position);
    SDL_FreeSurface(image_background);
    //1 background avec setAlpha
    image_background=SDL_LoadBMP("img/background_score.bmp");
    position.x=0;
    position.y=0;
    SDL_SetAlpha(image_background, SDL_SRCALPHA, 200);
    SDL_BlitSurface(image_background, NULL, myGame.screen,&position);
    SDL_FreeSurface(image_background);


    myGame.police = initialisation_ttf("fonts/ComicNeueSansID.ttf",15);
    header_surface=SDL_CreateRGBSurface(SDL_HWSURFACE,WIDTH_SCREEN,50,32,0,0,0,0);
    SDL_SetAlpha(header_surface, SDL_SRCALPHA, 180);
    SDL_FillRect(header_surface,NULL,SDL_MapRGB(myGame.screen->format, 255, 255, 255));
    //concatenation de header
    strcpy(header_mot,"       Score                ");
    strcat(header_mot,"      time ");
    strcat(header_mot,get_time(record.nb_secondes*1000));
    strcat(header_mot,"  4x4");

    header = TTF_RenderText_Blended(myGame.police,header_mot,couleurNoire);
    TTF_CloseFont(myGame.police);
    position.x=15;
    position.y=12;
    SDL_BlitSurface(header,NULL,header_surface,&position);
    SDL_FreeSurface(header);

    position.x=0;
    position.y=0;
    SDL_BlitSurface(header_surface,NULL,myGame.screen,&position);
    SDL_FreeSurface(header_surface);


    //icone header ############## a changer ###############"#"
    header_surface = SDL_LoadBMP("img/star_jaune.bmp");
    SDL_SetColorKey(header_surface, SDL_SRCCOLORKEY, SDL_MapRGB(header_surface->format, 0, 0, 0));
    position.x = 5;
    position.y = 6;
    SDL_BlitSurface(header_surface, NULL, myGame.screen,&position);
    SDL_FreeSurface(header_surface);
    header_surface = SDL_LoadBMP("img/time_user.bmp");
    SDL_SetColorKey(header_surface, SDL_SRCCOLORKEY, SDL_MapRGB(header_surface->format, 255,255,255));
    position.x=175;
    position.y=8;
    SDL_BlitSurface(header_surface, NULL, myGame.screen,&position);
    SDL_FreeSurface(header_surface);
     header_surface = SDL_LoadBMP("img/carre.bmp");
    SDL_SetColorKey(header_surface, SDL_SRCCOLORKEY, SDL_MapRGB(header_surface->format, 255,255,255));
    position.x=WIDTH_SCREEN-45;
    position.y=0;
    SDL_BlitSurface(header_surface, NULL, myGame.screen,&position);
    SDL_FreeSurface(header_surface);

    //si un seul joueur
    if(strcmp(record.vs,"solo")==0)
    {
        myGame.police = initialisation_ttf("fonts/ComicNeueSansID.ttf",15);
        //nom
        strcpy(header_mot,"player: ");
        strcat(header_mot,record.nom_joueur);
        chaine = TTF_RenderText_Blended(myGame.police,header_mot,couleurJaune);
        position.x = (myGame.screen->w-chaine->w)/2;
        position.y = 200;
        SDL_BlitSurface(chaine,NULL,myGame.screen,&position);
        SDL_FreeSurface(chaine);

        //score
        sprintf(header_mot,"score: %d",record.score.nb_points);
        chaine = TTF_RenderText_Blended(myGame.police,header_mot,couleurJaune);
        position.x = (myGame.screen->w-chaine->w)/2;
        position.y = 250;
        SDL_BlitSurface(chaine,NULL,myGame.screen,&position);
        SDL_FreeSurface(chaine);


        TTF_CloseFont(myGame.police);
    }
    else //si deux joueur
    {

    }

    SDL_Flip(myGame.screen);
    while(!myGame.quitter)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                myGame.quitter=true;
            break;

            case SDL_MOUSEBUTTONDOWN:


             break;

            default:
            break;
        }


        SDL_Delay(1);
        SDL_Flip(myGame.screen);
    }
    //liberation
}
