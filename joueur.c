#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "joueur.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include "game.h"

void liberation_joueur(Joueur* j)
{
    liberation_score(j->score);
    free(j);
}


Joueur* initialisation_joueur()
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
    SDL_Color couleurNoire = {0,0,0};
    SDL_Color couleurBlanche = {255, 255, 255};
    SDL_Color couleurBleu = {65,105,225};
    int max_taille_input=10;
    int taille_input_courant=0;
    int continuer=true;
    SDL_Surface *header=NULL,*header_surface=NULL,*image_background=NULL;
    char header_mot[20];
    char nbr_joueur[4];

    //1 background
    image_background = SDL_LoadBMP("img/background_joueur1.bmp");
    position.x=0;
    position.y=0;
    SDL_BlitSurface(image_background, NULL, myGame.screen,&position);
    SDL_FreeSurface(image_background);
    //1 background avec setAlpha
    image_background=SDL_LoadBMP("img/background_joueur2.bmp");
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
    sprintf(nbr_joueur,"%d",myGame.nbr_joueur);
    strcpy(header_mot,"      Nombre players ");
    strcat(header_mot,nbr_joueur);
    strcat(header_mot,"      time ");
    strcat(header_mot,get_time(myGame.nbr_seconde*1000));
    strcat(header_mot,"  4x4");

    header = TTF_RenderText_Blended(myGame.police,header_mot,couleurBlanche);
    TTF_CloseFont(myGame.police);
    position.x=15;
    position.y=12;
    SDL_BlitSurface(header,NULL,header_surface,&position);
    SDL_FreeSurface(header);

    position.x=0;
    position.y=0;
    SDL_BlitSurface(header_surface,NULL,myGame.screen,&position);
    SDL_FreeSurface(header_surface);


    //icone header
    header_surface = SDL_LoadBMP("img/user.bmp");
    SDL_SetColorKey(header_surface, SDL_SRCCOLORKEY, SDL_MapRGB(header_surface->format, 255,255,255));
    position.x=0;
    position.y=4;
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



    nvJoueur = Malloc(1,Joueur);
    nvJoueur->score = initialisation_score();
    nvJoueur->nom[0]='\0';

    //initialisation input box
    input_box = SDL_CreateRGBSurface(SDL_HWSURFACE,200,30,32,0,0,0,0);
    position_input_box.x = 100;
    position_input_box.y = 240;
    position_relatif.x = 0;
    position_relatif.y = 0;
    myGame.police = initialisation_ttf("fonts/neuropol x rg.ttf",18);
    //initialisation message nom joueur
    nom_text = TTF_RenderText_Blended(myGame.police,"  Name of player : ",couleurBlanche);
    position_nom_text.x = 0;
    position_nom_text.y = 210;



    //initialisation du bouton start
    strcpy(nbr_joueur,"START");

    play = TTF_RenderText_Blended(myGame.police,nbr_joueur,couleurBlanche);

    play_surface=SDL_LoadBMP("img/play_blanche.bmp");
    SDL_SetColorKey(play_surface, SDL_SRCCOLORKEY, SDL_MapRGB(myGame.screen->format, 255,255,255));



    //make screen clean for player input menu
   // SDL_FillRect(myGame.screen,NULL,SDL_MapRGB(myGame.screen->format,45,75,20));
        //position du text dans play_surface
    position_temp.x = (play_surface->w-play->w)/2;
    position_temp.y = (play_surface->h-play->h)/2;
    SDL_BlitSurface(play,NULL,play_surface,&position_temp);
    SDL_FreeSurface(play);
        //position du play dans screen
    position_temp.x = (myGame.screen->w-play_surface->w)/2;
    position_temp.y = (myGame.screen->h-play_surface->h)/2+80;
    SDL_BlitSurface(play_surface,NULL,myGame.screen,&position_temp);
    SDL_FreeSurface(play_surface);
	SDL_FillRect(input_box,NULL,SDL_MapRGB(myGame.screen->format,224,224,224));
    SDL_BlitSurface(nom_text,NULL,myGame.screen,&position_nom_text);
    SDL_FreeSurface(nom_text);
    SDL_BlitSurface(input_box,NULL,myGame.screen,&position_input_box);
    SDL_Flip(myGame.screen);

    SDL_EnableKeyRepeat(100,100);

    while(!myGame.quitter && continuer)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                myGame.quitter=true;
            break;

            case SDL_KEYDOWN:
                if(event.key.keysym.sym == SDLK_RETURN)
                {
                    if(strlen(nvJoueur->nom)>=3)
                        continuer = false;
                }

            	if(taille_input_courant < max_taille_input && event.key.keysym.unicode <= 0x7A && event.key.keysym.unicode >= 0x41 )
            	{
	            	strcat(nvJoueur->nom,get_input(&event.key));
	                input_text = TTF_RenderText_Blended(myGame.police,nvJoueur->nom,couleurNoire);
	                SDL_FillRect(input_box,NULL,SDL_MapRGB(myGame.screen->format,255,255,255));
	                SDL_BlitSurface(input_text,NULL,input_box,&position_relatif);
                    SDL_FreeSurface(input_text);
	                SDL_BlitSurface(input_box,NULL,myGame.screen,&position_input_box);

	                taille_input_courant++;
            	}
            	else if(taille_input_courant > 0 && event.key.keysym.sym == SDLK_BACKSPACE)
                {
                    nvJoueur->nom[--taille_input_courant]='\0';
                    input_text = TTF_RenderText_Blended(myGame.police,nvJoueur->nom,couleurNoire);
	                SDL_FillRect(input_box,NULL,SDL_MapRGB(myGame.screen->format,224,224,224));
	                SDL_BlitSurface(input_text,NULL,input_box,&position_relatif);
                    SDL_FreeSurface(input_text);
	                SDL_BlitSurface(input_box,NULL,myGame.screen,&position_input_box);
                }

            break;

            case SDL_MOUSEBUTTONDOWN:

                //clique sur bouton jouer
                if( (event.button.x >= position_temp.x) && (event.button.x<= position_temp.x+play_surface->w)
                   && (event.button.y >= position_temp.y) && (event.button.y<= position_temp.y+play_surface->h)
                   ){
                        if(strlen(nvJoueur->nom)>=3)
                            continuer=false;
                    }

             break;

            default:
            break;
        }


        SDL_Delay(1);
        SDL_Flip(myGame.screen);
    }
    //liberation
    SDL_FreeSurface(input_box);
    TTF_CloseFont(myGame.police);

    return nvJoueur;
}
