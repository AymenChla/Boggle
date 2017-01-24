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
    boolean continuer = true;
    SDL_Event event;
    SDL_Surface *input_text,* nom_text;
    SDL_Surface *input_box;
   	SDL_Surface *play=NULL;
   	SDL_Surface *play_surface=NULL;
    SDL_Rect position_relatif,position_nom_text;
    SDL_Rect position_input_box;
    SDL_Rect position_play;
    SDL_Rect position_temp;
    SDL_Color couleurNoire = {0,0,0};
    SDL_Color couleurBlanche = {255,255,255};
    int max_taille_input=10;
    int taille_input_courant=0;


    nvJoueur = Malloc(1,Joueur);
    nvJoueur->score = initialisation_score();
    nvJoueur->nom[0]='\0';

    //initialisation input box
    input_box = SDL_CreateRGBSurface(SDL_HWSURFACE,150,30,32,0,0,0,0);
    position_input_box.x = 150;
    position_input_box.y = 150;
    position_relatif.x = 0;
    position_relatif.y = 0;

    //initialisation message nom joueur
    nom_text = TTF_RenderText_Blended(myGame.police,"nom: ",couleurBlanche);
    position_nom_text.x = 80;
    position_nom_text.y = 150;

    
    //initialisation du bouton jouer
    play = TTF_RenderText_Blended(myGame.police,"Jouer",couleurBlanche);
    play_surface=SDL_CreateRGBSurface(SDL_HWSURFACE,3*TAILLE_CASE,TAILLE_CASE,32,0,0,0,0);
    SDL_FillRect(play_surface,NULL,SDL_MapRGB(myGame.screen->format,255,55,255));



    //make screen clean for player input menu
    SDL_FillRect(myGame.screen,NULL,SDL_MapRGB(myGame.screen->format,45,75,20));
        //position du text dans play_surface
    position_temp.x = (play_surface->w-play->w)/2;
    position_temp.y = (play_surface->h-play->h)/2;
    SDL_BlitSurface(play,NULL,play_surface,&position_temp);
        //position du play dans screen
    position_temp.x = (myGame.screen->w-play_surface->w)/2;
    position_temp.y = (myGame.screen->h-play_surface->h)/2;
    SDL_BlitSurface(play_surface,NULL,myGame.screen,&position_temp);
	SDL_FillRect(input_box,NULL,SDL_MapRGB(myGame.screen->format,255,255,255));
    SDL_BlitSurface(nom_text,NULL,myGame.screen,&position_nom_text);
    SDL_BlitSurface(input_box,NULL,myGame.screen,&position_input_box);
    SDL_Flip(myGame.screen);

    SDL_EnableKeyRepeat(100,100);

    while(continuer)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                continuer = false;
            break;

            case SDL_KEYDOWN:

            	if(taille_input_courant < max_taille_input && event.key.keysym.unicode <= 0x7A && event.key.keysym.unicode >= 0x41 )
            	{
	            	strcat(nvJoueur->nom,get_input(&event.key));
	                input_text = TTF_RenderText_Blended(myGame.police,nvJoueur->nom,couleurNoire);
	                SDL_FillRect(input_box,NULL,SDL_MapRGB(myGame.screen->format,255,255,255));
	                SDL_BlitSurface(input_text,NULL,input_box,&position_relatif);
	                SDL_BlitSurface(input_box,NULL,myGame.screen,&position_input_box);

	                taille_input_courant++;
            	}
            	else if(taille_input_courant > 0 && event.key.keysym.sym == SDLK_BACKSPACE)
                {
                    nvJoueur->nom[--taille_input_courant]='\0';
                    input_text = TTF_RenderText_Blended(myGame.police,nvJoueur->nom,couleurNoire);
	                SDL_FillRect(input_box,NULL,SDL_MapRGB(myGame.screen->format,255,255,255));
	                SDL_BlitSurface(input_text,NULL,input_box,&position_relatif);
	                SDL_BlitSurface(input_box,NULL,myGame.screen,&position_input_box);
                }

            break;

            case SDL_MOUSEBUTTONDOWN:

                //clique sur bouton jouer
                if( (event.button.x >= position_temp.x) && (event.button.x<= position_temp.x+play_surface->w)
                   && (event.button.y >= position_temp.y) && (event.button.y<= position_temp.y+play_surface->h)
                   )
                   continuer = false;
             break;
             
            default:
            break;
        }


        SDL_Delay(1);
        SDL_Flip(myGame.screen);
    }

    return nvJoueur;
}
