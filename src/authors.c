#include "../include/authors.h"
#include "../include/books.h"
#include <stdio.h>
#include <string.h>

#define FICHIER "DATABASE/AUTHORS.dat"

/* Calcule le prochain id */
static int prochainId(void) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) return 1;
    fseek(f, 0, SEEK_END);
    int id = (int)(ftell(f) / sizeof(Auteur)) + 1;
    fclose(f);
    return id;
}

/* Trouve un auteur par id. Retourne le rang ou -1 */
static int chercherAuteur(int id, Auteur *res) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) return -1;

    Auteur a;
    int rang = 0;
    while (fread(&a, sizeof(Auteur), 1, f) == 1) {
        if (a.id == id) {
            *res = a;
            fclose(f);
            return rang;
        }
        rang++;
    }
    fclose(f);
    return -1;
}

/* Crée un auteur. 0=succès */
int creerAuteur(const Auteur *donnees, Auteur *nouveau) {
    *nouveau = *donnees;
    nouveau->id = prochainId();

    FILE *f = fopen(FICHIER, "ab");
    if (f == NULL) return -3;
    fwrite(nouveau, sizeof(Auteur), 1, f);
    fclose(f);
    return 0;
}

/* Trouve par id. 1=trouvé, 0=pas trouvé */
int trouverAuteurParId(int id, Auteur *res) {
    return (chercherAuteur(id, res) >= 0) ? 1 : 0;
}

/* Modifie un auteur. 0=succès, -1=introuvable */
int modifierAuteur(int id, const Auteur *nouvellesDonnees) {
    FILE *f = fopen(FICHIER, "rb+");
    if (f == NULL) return -3;

    Auteur a;
    int rang = 0;
    while (fread(&a, sizeof(Auteur), 1, f) == 1) {
        if (a.id == id) {
            Auteur modifie = *nouvellesDonnees;
            modifie.id = id;
            fseek(f, rang * sizeof(Auteur), SEEK_SET);
            fwrite(&modifie, sizeof(Auteur), 1, f);
            fclose(f);
            return 0;
        }
        rang++;
    }
    fclose(f);
    return -1;
}

/* Supprime un auteur.
   Refuse si des livres sont associés.
   0=succès, -1=introuvable, -2=livres associés */
int supprimerAuteur(int id) {
    if (compterLivresParAuteur(id) > 0)
        return -2;

    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) return -3;

    Auteur liste[500];
    int total = 0, trouve = 0;
    Auteur a;

    while (fread(&a, sizeof(Auteur), 1, f) == 1) {
        if (a.id != id) {
            liste[total] = a;
            total++;
        } else {
            trouve = 1;
        }
    }
    fclose(f);

    if (!trouve) return -1;

    f = fopen(FICHIER, "wb");
    if (f == NULL) return -3;
    fwrite(liste, sizeof(Auteur), total, f);
    fclose(f);
    return 0;
}

/* Liste tous les auteurs */
int listerAuteurs(Auteur tableau[], int tailleMax, int *nb) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) { *nb = 0; return -3; }

    *nb = 0;
    Auteur a;
    while (fread(&a, sizeof(Auteur), 1, f) == 1) {
        if (*nb < tailleMax) {
            tableau[*nb] = a;
            (*nb)++;
        }
    }
    fclose(f);
    return 0;
}
