
#ifndef GAME_H
#define GAME_H

    #include <SDL_ttf.h>

    #include "plateau.h"
    #include "joueur.h"


    typedef struct
    {
        SDL_Surface *valider;
        SDL_Rect position_valider;

        SDL_Surface *score;
        SDL_Rect position_score;

        SDL_Surface *surface_time;
        SDL_Surface *time;
        SDL_Rect position_time;

        //surface du mot courant
        SDL_Surface *mot_courant;
        SDL_Surface *surface_mot_courant;
        SDL_Rect position_mot_courant;

    } Btn_outils;

    typedef struct {

        Btn_outils *btn_outils;
        SDL_Surface *screen;
        TTF_Font *police;
        Plateau *plateau;
        Joueur *joueur[2];
    } Game;

    Game myGame;

    void initialisation_btn_outils();
    boolean is_last_click(int,int);
    boolean can_be_hovered(int,int);
    void initialisation_game();
    void saisir_mot(char*);
    boolean validation_sur_dictionnaire(char *nom_fichier,char *mot);
    void play_game(Game);
    void end_game(Game);
    SDL_Surface *initialisation_sdl();
    void error(char *msg);
    TTF_Font* initialisation_ttf(char *);
    boolean valider_mot();
    char* get_time(int);
    void menu_game();
    void preparation_joueurs();


#endif // GAME_H
