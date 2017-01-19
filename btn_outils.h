
#ifndef BTN_OUTILS
#define BTN_OUTILS

#include <SDL_ttf.h>
#include <SDL.h>
#include "generale.h"


typedef struct
{
    SDL_Surface *valider;

} Btn_outils;

Btn_outils* initialisation_btn_outils(SDL_Rect position_plateau ,TTF_Font);

#endif // BTN_OUTILS
