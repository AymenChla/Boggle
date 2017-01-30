#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "score.h"

int recuperer_id_courant()
{
    int id;
    FILE* fichier=NULL;
    fichier = fopen("scores.dta","rb");
    if(fichier!=NULL)
    {

        fread(&id,sizeof(int),1,fichier);
        fclose(fichier);
        return id;

    }
    else
        fprintf(stderr,"can't open the file recuperer courant id\n");

    return 0;
}

void afficher_score(Score* score)
{
    Mot *cour = NULL;
    cour = score->teteMots;


    fprintf(stdout,"score: %d\n",score->nb_points);

    while(cour != NULL)
    {
        fprintf(stdout,"%s\t%d\n",cour->info->mot,cour->info->points);
        cour = cour->suivant;
    }
}

int nb_points(char* mot)
{
        switch(strlen(mot))
        {
            case 3:
            case 4:
                return 1;
            break;

            case 5:
                return 2;
            break;

            case 6:
                return 3;
            break;

            case 7:
                return 5;
            break;

            default:
                return 11;
            break;
        }

}

Mot *creer_mot(char *mot)
{
        Mot *nvMot = Malloc(1,Mot);
        nvMot->info = Malloc(1,DonneesMot);

        strcpy(nvMot->info->mot,mot);
        nvMot->info->points = nb_points(mot);

        nvMot->suivant = NULL;

        return nvMot;
}

Score* initialisation_score()
{
    Score *score = Malloc(1,Score);
    score->nb_points = 0;
    score->nb_mots=0;
    score->teteMots = NULL;
    return score;
}


void inserer_mot(Score *score,char *mot)
{

    if(rechercher_mot(score,mot) == false)
    {
        Mot *nvMot = creer_mot(mot);

        score->nb_points += nvMot->info->points;
        score->nb_mots++;

        nvMot->suivant = score->teteMots;
        score->teteMots = nvMot;

    }

}


boolean rechercher_mot(Score *score,char *mot)
{
    Mot *cour = NULL;

    cour = score->teteMots;

    while(cour != NULL && strcmp(mot,cour->info->mot)!= 0)
        cour = cour->suivant;

    if(cour != NULL)
        return true;

    return false;
}

void liberation_score(Score *score)
{
    Mot *cour = score->teteMots;
    Mot *temp;
    while(cour != NULL)
    {
        temp = cour;
        free(cour->info);
        cour = cour->suivant;
        free(temp);
    }
    free(score);
}

/*void recuperer()
{
    Score_board record;
    FILE* fichier=NULL;
    DonneesMot *infos=NULL;
    int i=0,nb_mots=0;


    fichier = fopen("scores.dta","rb");
    if(fichier!=NULL)
    {
        //recuperer nb_secondes et test si EOF
        while(fread(&record.nb_secondes,sizeof(int),1,fichier)==1)
        {



            //recuperer nom joueur
            fread(record.nom_joueur,sizeof(char),20,fichier);

            //recuperer vs
            fread(record.vs,sizeof(char),20,fichier);

            //recuperer  nb mot
            fread(&nb_mots,sizeof(int),1,fichier);

            //recuperer les mots


            infos=Malloc(1,DonneesMot);
            record.score.teteMots=NULL;
            record.score.nb_points=0;
            record.score.nb_mots=0;
            for(i=0 ; i < nb_mots ;i++){
                fread(infos,sizeof(DonneesMot),1,fichier);
                inserer_mot(&record.score,infos->mot);
            }


            fprintf(stdout,"%d\n",record.nb_secondes);
            fprintf(stdout,"%s\n",record.nom_joueur);
            fprintf(stdout,"%s\n",record.vs);
            fprintf(stdout,"nbpoint:%d\nnbmot:%d\n",record.score.nb_points,record.score.nb_mots);
            afficher_score(&record.score);

            free(infos);

        }



        fclose(fichier);
    }
    else
    {
        fprintf(stderr,"can't open the file\n");
    }

}*/

/*void sauvegarder_score(char *nom_joueur, char *vs,Score *score,int nb_secondes)
{
    FILE* fichier=NULL;
    Mot *temp;


    fichier = fopen("scores.dta","ab");
    if(fichier!=NULL)
    {
        //sauvegarde nb_secondes
        fwrite(&nb_secondes,sizeof(int),1,fichier);

        //sauvegarde nom joueur
        fwrite(nom_joueur,sizeof(char),20,fichier);

        //sauvegarde vs
        fwrite(vs,sizeof(char),20,fichier);

        //sauvegarde de nb mot
        fwrite(&score->nb_mots,sizeof(int),1,fichier);

        //sauvegarde des mots
        temp = score->teteMots;
        while(temp != NULL)
        {
            fwrite(temp->info,sizeof(DonneesMot),1,fichier);
            temp = temp->suivant;
        }


        fclose(fichier);
    }
    else
    {
        fprintf(stderr,"can't open the file\n");
    }
}*/

Tab_Score_board best_score_by_time(int start,int nb_records,int nb_secondes)
{
    Tab_Score_board all_records;
    Tab_Score_board records_i_want;
    int i,cmpt_start=1;
    Mot *temp;

    all_records = get_all_scores();
    records_i_want.nb_records=0;
    records_i_want.records=NULL;
    for(i=0 ; i < all_records.nb_records && records_i_want.nb_records != nb_records ;i++)
    {

        if(all_records.records[i].nb_secondes == nb_secondes)
        {
            //pour commencer a partir du ieme score selon start
            if(cmpt_start!=start)
                cmpt_start++;
            else
            {
                records_i_want.nb_records++;
                //inserer nv score
                records_i_want.records = (Score_board*) realloc(records_i_want.records,records_i_want.nb_records*sizeof(Score_board));
                records_i_want.records[records_i_want.nb_records-1].nb_secondes = nb_secondes;
                records_i_want.records[records_i_want.nb_records-1].id_partie=all_records.records[i].id_partie;
                strcpy(records_i_want.records[records_i_want.nb_records-1].nom_joueur,all_records.records[i].nom_joueur);
                strcpy(records_i_want.records[records_i_want.nb_records-1].vs,all_records.records[i].vs);
                records_i_want.records[records_i_want.nb_records-1].score.nb_mots=0;
                records_i_want.records[records_i_want.nb_records-1].score.nb_points=0;
                records_i_want.records[records_i_want.nb_records-1].score.teteMots=NULL;
                temp = all_records.records[i].score.teteMots;
                while(temp!=NULL)
                {
                    inserer_mot(&records_i_want.records[records_i_want.nb_records-1].score,temp->info->mot);
                    temp = temp->suivant;
                }

            }
        }

    }

    return records_i_want;
}

Tab_Score_board get_all_scores()
{
    Tab_Score_board tab_scores;
    FILE* fichier=NULL;
    DonneesMot *infos=NULL;
    int i=0,nb_mots=0,nb_secondes=0,id_courant=0;

    tab_scores.records=NULL;
    tab_scores.nb_records=0;

    fichier = fopen("scores.dta","rb");
    if(fichier!=NULL)
    {
        //recuperer id courant
        fread(&id_courant,sizeof(int),1,fichier);

        //recuperer nb_secondes et test si EOF
        while(fread(&nb_secondes,sizeof(int),1,fichier)==1)
        {


            //ajout un nv score dans records
            tab_scores.nb_records++;
            tab_scores.records =(Score_board*) realloc(tab_scores.records,tab_scores.nb_records*sizeof(Score_board));

            //affectation nb_secondes
            tab_scores.records[tab_scores.nb_records-1].nb_secondes = nb_secondes;

            //recuperer id partie
            fread(&tab_scores.records[tab_scores.nb_records-1].id_partie,sizeof(int),1,fichier);

            //recuperer nom joueur
            fread(tab_scores.records[tab_scores.nb_records-1].nom_joueur,sizeof(char),20,fichier);

            //recuperer vs
            fread(tab_scores.records[tab_scores.nb_records-1].vs,sizeof(char),20,fichier);

            //recuperer  nb mot
            fread(&nb_mots,sizeof(int),1,fichier);

            //recuperer les mots
            infos=Malloc(1,DonneesMot);
            tab_scores.records[tab_scores.nb_records-1].score.teteMots=NULL;
            tab_scores.records[tab_scores.nb_records-1].score.nb_points=0;
            tab_scores.records[tab_scores.nb_records-1].score.nb_mots=0;
            for(i=0 ; i < nb_mots ;i++){
                fread(infos,sizeof(DonneesMot),1,fichier);
                inserer_mot(&tab_scores.records[tab_scores.nb_records-1].score,infos->mot);
            }


            free(infos);
        }



        fclose(fichier);
    }
    else
    {
        fprintf(stderr,"can't open the file\n");
    }

    return tab_scores;
}

void afficher_score_board(Tab_Score_board tab_scores)
{
    int i;

    for(i=0 ;i < tab_scores.nb_records ; i++)
    {
        fprintf(stdout,"%d\n",tab_scores.records[i].id_partie);
        fprintf(stdout,"%d\n",tab_scores.records[i].nb_secondes);
        fprintf(stdout,"%s\n",tab_scores.records[i].nom_joueur);
        fprintf(stdout,"%s\n",tab_scores.records[i].vs);
        fprintf(stdout,"nbpoint:%d\nnbmot:%d\n",tab_scores.records[i].score.nb_points,tab_scores.records[i].score.nb_mots);
        afficher_score(&tab_scores.records[i].score);
    }
}

/*void save_record(Score_board record,FILE *fichier)
{

        Mot *temp=NULL;


        //sauvegarde nb_secondes
        fwrite(&record.nb_secondes,sizeof(int),1,fichier);

        //sauvegarde nom joueur
        fwrite(record.nom_joueur,sizeof(char),20,fichier);

        //sauvegarde vs
        fwrite(record.vs,sizeof(char),20,fichier);

        //sauvegarde de nb mot
        fwrite(&record.score.nb_mots,sizeof(int),1,fichier);

        //sauvegarde des mots
        temp = record.score.teteMots;
        while(temp != NULL)
        {
            fwrite(temp->info,sizeof(DonneesMot),1,fichier);
            temp = temp->suivant;
        }

}*/

void sauvegarder_score_trier(char *nom_joueur, char *vs,Score *score,int nb_secondes,int id_courant)
{
    Tab_Score_board tab_scores;
    FILE* fichier=NULL;
    Mot *temp=NULL;
    int i;

    //recuperer tout les scores
    tab_scores = get_all_scores();
    tab_scores.nb_records++;

    fichier = fopen("scores.dta","wb");
    if(fichier!=NULL)
    {

        //inserer nv score
        tab_scores.records = (Score_board*) realloc(tab_scores.records,tab_scores.nb_records*sizeof(Score_board));
        tab_scores.records[tab_scores.nb_records-1].id_partie = id_courant;
        tab_scores.records[tab_scores.nb_records-1].nb_secondes = nb_secondes;
        strcpy(tab_scores.records[tab_scores.nb_records-1].nom_joueur,nom_joueur);
        strcpy(tab_scores.records[tab_scores.nb_records-1].vs,vs);
        tab_scores.records[tab_scores.nb_records-1].score.nb_mots=0;
        tab_scores.records[tab_scores.nb_records-1].score.nb_points=0;
        tab_scores.records[tab_scores.nb_records-1].score.teteMots=NULL;
        temp = score->teteMots;

        while(temp!=NULL)
        {
            inserer_mot(&tab_scores.records[tab_scores.nb_records-1].score,temp->info->mot);
            temp = temp->suivant;
        }


        //trier scores
        trier_score_board(tab_scores);

        //sauvegarder id courant
        fwrite(&id_courant,sizeof(int),1,fichier);

        //sauvegarder les scores
        for(i=0 ; i < tab_scores.nb_records ;i++)
        {

             //sauvegarde nb_secondes
            fwrite(&tab_scores.records[i].nb_secondes,sizeof(int),1,fichier);


            //sauvegarder id partie
            fwrite(&tab_scores.records[i].id_partie,sizeof(int),1,fichier);

            //sauvegarde nom joueur
            fwrite(tab_scores.records[i].nom_joueur,sizeof(char),20,fichier);

            //sauvegarde vs
            fwrite(tab_scores.records[i].vs,sizeof(char),20,fichier);

            //sauvegarde de nb mot
            fwrite(&tab_scores.records[i].score.nb_mots,sizeof(int),1,fichier);

            //sauvegarde des mots
            temp = tab_scores.records[i].score.teteMots;
            while(temp != NULL)
            {
                fwrite(temp->info,sizeof(DonneesMot),1,fichier);
                temp = temp->suivant;
            }

        }
            //save_record(tab_scores.records[i],fichier);

        fclose(fichier);
    }
    else fprintf(stderr,"can't open the file\n");


}

void trier_score_board(Tab_Score_board tab_scores)
{
    int i,j;
    Score_board temp;
    Mot *mot_temp=NULL;

    for(i=0 ; i < tab_scores.nb_records-1;i++)
    {
        for(j = i+1 ; j < tab_scores.nb_records ; j++)
        {
            if(tab_scores.records[j].score.nb_points > tab_scores.records[i].score.nb_points)
            {
                temp.id_partie = tab_scores.records[j].id_partie;
                temp.nb_secondes = tab_scores.records[j].nb_secondes;
                strcpy(temp.nom_joueur,tab_scores.records[j].nom_joueur);
                strcpy(temp.vs,tab_scores.records[j].vs);
                temp.score.nb_mots=0;
                temp.score.nb_points=0;
                temp.score.teteMots=NULL;
                mot_temp = tab_scores.records[j].score.teteMots;
                while(mot_temp!=NULL)
                {
                    inserer_mot(&temp.score,mot_temp->info->mot);
                    mot_temp=mot_temp->suivant;
                }


                tab_scores.records[j].id_partie = tab_scores.records[i].id_partie;
                tab_scores.records[j].nb_secondes = tab_scores.records[i].nb_secondes;
                strcpy(tab_scores.records[j].nom_joueur,tab_scores.records[i].nom_joueur);
                strcpy(tab_scores.records[j].vs,tab_scores.records[i].vs);
                tab_scores.records[j].score.nb_mots=0;
                tab_scores.records[j].score.nb_points=0;
                tab_scores.records[j].score.teteMots=NULL;
                mot_temp = tab_scores.records[i].score.teteMots;
                while(mot_temp!=NULL)
                {
                    inserer_mot(&tab_scores.records[j].score,mot_temp->info->mot);
                    mot_temp=mot_temp->suivant;
                }

                tab_scores.records[i].id_partie = temp.id_partie;
                tab_scores.records[i].nb_secondes = temp.nb_secondes;
                strcpy(tab_scores.records[i].nom_joueur,temp.nom_joueur);
                strcpy(tab_scores.records[i].vs,temp.vs);
                tab_scores.records[i].score.nb_mots=0;
                tab_scores.records[i].score.nb_points=0;
                tab_scores.records[i].score.teteMots=NULL;
                mot_temp = temp.score.teteMots;
                while(mot_temp!=NULL)
                {
                    inserer_mot(&tab_scores.records[i].score,mot_temp->info->mot);
                    mot_temp=mot_temp->suivant;
                }

            }
        }
    }
}


Score_board information_adversaire(char *nom_adversaire,int id_partie)
{
    Tab_Score_board all_records;
    int i=0;

    all_records = get_all_scores();
    while(i < all_records.nb_records
          && strcmp(nom_adversaire,all_records.records[i].nom_joueur)!=0
          && id_partie!=all_records.records[i].id_partie)
        i++;

    if(i < all_records.nb_records)
        return all_records.records[i];
}
