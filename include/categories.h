#ifndef CATEGORIES_H
#define CATEGORIES_H

typedef struct {
    int  id;
    char libelle[50];
    char description[200];
    char dateCreation[20];
} Categorie;

int creerCategorie(const Categorie *donnees, Categorie *nouvelleCategorie);
int trouverCategorieParId(int id, Categorie *resultat);
int modifierCategorie(int id, const Categorie *nouvellesDonnees);
int supprimerCategorie(int id);
int listerCategories(Categorie tableau[], int tailleMax, int *nombreTrouve);

#endif