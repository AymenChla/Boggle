
#ifndef GENERALE_H
#define GENERALE_H

    #define NB_LIGNE 4
    #define NB_COLONNE 4
    #define TAILLE_CASE 70
    #define DECALAGE 5
    #define TAILLE_PLATEAU NB_COLONNE*TAILLE_CASE+5*DECALAGE
    #define WIDTH_SCREEN TAILLE_PLATEAU+50
    #define HEIGHT_SCREEN TAILLE_PLATEAU+300


    #define Malloc(nb,type) (type *)malloc(nb*sizeof(type))

    typedef enum{false, true} boolean;


#endif // GENERALE_H
