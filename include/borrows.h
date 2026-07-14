#ifndef BORROWS_H
#define BORROWS_H

#include "utils.h"

typedef struct {
    int  id;
    char numeroEmprunt[20];
    int  idUtilisateur;
    int  idLivre;
    char dateEmprunt[20];
    char datePrevueRetour[20];
    EtatEmprunt etat;
} Emprunt;

void genererNumeroEmprunt(char *sortie);
int  compterEmpruntsActifsParUtilisateur(int idUtilisateur);
int  compterEmpruntsActifsParLivre(int idLivre);
int  creerEmprunt(int idUtilisateur, int idLivre, Emprunt *nouvelEmprunt);
int  trouverEmpruntParId(int id, Emprunt *resultat);
int  trouverEmpruntParNumero(const char *numeroEmprunt, Emprunt *resultat);
int  listerEmpruntsActifsParUtilisateur(int idUtilisateur, Emprunt tableau[], int tailleMax, int *nombreTrouve);
int  mettreAJourEmpruntsEnRetard(void);

#endif