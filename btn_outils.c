#include <stdio.h>
#include <stdlib.h>

#include "btn_outils.h"

Btn_outils* initialisation_btn_outils(SDL_Rect position_plateau ,TTF_Font police)
{
    Btn_outils *btn_outils=NULL;
    SDL_Rect position;
    position.x = position_plateau.x;
    position.y = position_plateau.y+10;

    btn_outils = Malloc(1,Btn_outils);

    btn_outils.valider = TTF_RenderText_Blended(police, "Valider" , couleurBlanche);

    SDL_BlitSurface(btn_outils.valider,NULL,myGame.screen,&position);

    return Btn_outils;
}
