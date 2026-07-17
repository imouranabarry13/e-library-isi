#include "../include/categories.h"
#include "../include/books.h"
#include "../include/utils.h"
#include <stdio.h>
#include <string.h>

#define FICHIER "DATABASE/CATEGORIES.dat"

/* Calcule le prochain id */
static int prochainId(void) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) return 1;
    fseek(f, 0, SEEK_END);
    int id = (int)(ftell(f) / sizeof(Categorie)) + 1;
    fclose(f);
    return id;
}

/* Crée une catégorie. 0=succès */
int creerCategorie(const Categorie *donnees, Categorie *nouvelle) {
    *nouvelle = *donnees;
    nouvelle->id = prochainId();
    obtenirDateHeureActuelle(nouvelle->dateCreation, 20);

    FILE *f = fopen(FICHIER, "ab");
    if (f == NULL) return -3;
    fwrite(nouvelle, sizeof(Categorie), 1, f);
    fclose(f);
    return 0;
}

/* Trouve par id. 1=trouvé, 0=pas trouvé */
int trouverCategorieParId(int id, Categorie *res) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) return 0;

    Categorie c;
    while (fread(&c, sizeof(Categorie), 1, f) == 1) {
        if (c.id == id) {
            *res = c;
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

/* Modifie une catégorie. 0=succès, -1=introuvable */
int modifierCategorie(int id, const Categorie *nouvellesDonnees) {
    FILE *f = fopen(FICHIER, "rb+");
    if (f == NULL) return -3;

    Categorie c;
    int rang = 0;
    while (fread(&c, sizeof(Categorie), 1, f) == 1) {
        if (c.id == id) {
            Categorie modifie = *nouvellesDonnees;
            modifie.id = id;
            fseek(f, rang * sizeof(Categorie), SEEK_SET);
            fwrite(&modifie, sizeof(Categorie), 1, f);
            fclose(f);
            return 0;
        }
        rang++;
    }
    fclose(f);
    return -1;
}

/* Supprime une catégorie.
   Refuse si des livres l'utilisent.
   0=succès, -1=introuvable, -2=livres associés */
int supprimerCategorie(int id) {
    if (compterLivresParCategorie(id) > 0)
        return -2;

    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) return -3;

    Categorie liste[500];
    int total = 0, trouve = 0;
    Categorie c;

    while (fread(&c, sizeof(Categorie), 1, f) == 1) {
        if (c.id != id) {
            liste[total] = c;
            total++;
        } else {
            trouve = 1;
        }
    }
    fclose(f);

    if (!trouve) return -1;

    f = fopen(FICHIER, "wb");
    if (f == NULL) return -3;
    fwrite(liste, sizeof(Categorie), total, f);
    fclose(f);
    return 0;
}

/* Liste toutes les catégories */
int listerCategories(Categorie tableau[], int tailleMax, int *nb) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) { *nb = 0; return -3; }

    *nb = 0;
    Categorie c;
    while (fread(&c, sizeof(Categorie), 1, f) == 1) {
        if (*nb < tailleMax) {
            tableau[*nb] = c;
            (*nb)++;
        }
    }
    fclose(f);
    return 0;
}
