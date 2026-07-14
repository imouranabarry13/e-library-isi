#ifndef PENALTIES_H
#define PENALTIES_H

typedef struct {
    int   id;
    int   idUtilisateur;
    int   idEmprunt;
    int   joursRetard;
    float montant;
    char  date[20];
} Penalite;

int   creerPenalite(int idUtilisateur, int idEmprunt, int joursRetard, float montant, Penalite *nouvellePenalite);
int   listerPenalitesParUtilisateur(int idUtilisateur, Penalite tableau[], int tailleMax, int *nombreTrouve);
float getTotalPenalitesMontant(int idUtilisateur);
float getTotalPenalitesMontantJour(void);

#endif