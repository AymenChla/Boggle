#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"

int main(int argc, char *argv[])
{


    //initialisation SDl
    myGame.screen = initialisation_sdl();
    if(TTF_Init()== -1){

        fprintf(stderr,"%s: %s\n","impossible d'initialiser la ttf: s\n",TTF_GetError());
        exit(EXIT_FAILURE);
     }
     
    myGame.quitter=false;
    
    menu_game();

    return EXIT_SUCCESS;

}
