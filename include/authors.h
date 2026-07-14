#ifndef AUTHORS_H
#define AUTHORS_H

typedef struct {
    int  id;
    char nomComplet[100];
    char nationalite[50];
    char dateNaissance[11];
    char biographie[500];
    int  nombreLivresPublies;
} Auteur;

int creerAuteur(const Auteur *donnees, Auteur *nouvelAuteur);
int trouverAuteurParId(int id, Auteur *resultat);
int modifierAuteur(int id, const Auteur *nouvellesDonnees);
int supprimerAuteur(int id);
int listerAuteurs(Auteur tableau[], int tailleMax, int *nombreTrouve);

#endif