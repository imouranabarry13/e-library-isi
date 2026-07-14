#ifndef RETURNS_H
#define RETURNS_H

typedef struct {
    int   idRetour;
    int   idEmprunt;
    char  dateRetour[20];
    int   joursRetard;
    float montantPenalite;
} Retour;

int calculerJoursRetard(const char *datePrevueRetour, const char *dateRetourReelle);
int creerRetour(int idEmprunt, Retour *nouveauRetour);
int listerRetours(Retour tableau[], int tailleMax, int *nombreTrouve);
int listerRetoursParUtilisateur(int idUtilisateur, Retour tableau[], int tailleMax, int *nombreTrouve);

#endif