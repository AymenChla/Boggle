#ifndef SCORE_H
#define SCORE_H

    #include "generale.h"

    typedef struct{
        char mot[9];
        int points;
    } DonneesMot;

    typedef struct Mot{
        DonneesMot *info;
        struct Mot *suivant;
    } Mot;

    typedef struct{
        int nb_points;
        int nb_mots;
        //tete de la liste chainée
        Mot *teteMots;
    } Score;

    //pour l'affichage du score
    typedef struct{
        char nom_joueur[20];
        char vs[20];
        Score score;
        int nb_secondes;
    }Score_board;

    typedef struct{
        Score_board *records;
        int nb_records;
    }Tab_Score_board;

    int nb_points(char *);
    Mot *creer_mot(char*);
    Score* initialisation_score();
    void inserer_mot(Score *score,char *mot);
    boolean rechercher_mot(Score *score,char *mot);
    void afficher_score(Score* );
    void liberation_score(Score *score);
    void sauvegarder_score(char *nom_joueur,char *vs,Score* score,int nb_secondes);
    void sauvegarder_score_trier(char *nom_joueur,char *vs,Score* score,int nb_secondes);
    void affichage_score_board();
    Tab_Score_board best_score_by_time(int start,int nb_records,int nb_secondes);
    Tab_Score_board best_score_by_player_by_time(int start,int nb_records,int nb_secondes);
    void recuperer();
    void afficher_score_board(Tab_Score_board tab_scores);
    Tab_Score_board get_all_scores();
    void save_record(Score_board record,FILE *fichier);
    void afficher_cinq_mot(int j,int partie);


#endif // SCORE_H
