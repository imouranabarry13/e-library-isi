#include "../include/books.h"
#include "../include/authors.h"
#include "../include/categories.h"
#include "../include/borrows.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define FICHIER "DATABASE/BOOKS.dat"

/* Convertit une chaîne en minuscules */
static void minuscules(const char *src, char *dest, int taille) {
    int i;
    for (i = 0; i < taille - 1 && src[i] != '\0'; i++)
        dest[i] = tolower((unsigned char)src[i]);
    dest[i] = '\0';
}

/* Calcule le prochain id */
static int prochainId(void) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) return 1;
    fseek(f, 0, SEEK_END);
    int id = (int)(ftell(f) / sizeof(Livre)) + 1;
    fclose(f);
    return id;
}

/* Crée un livre.
   Vérifie auteur existe, catégorie existe, ISBN unique.
   0=succès, -1=auteur inexistant, -2=catégorie inexistante, -3=ISBN déjà pris */
int creerLivre(const Livre *donnees, Livre *nouveau) {
    Auteur a;
    if (trouverAuteurParId(donnees->idAuteur, &a) == 0)
        return -1;

    Categorie c;
    if (trouverCategorieParId(donnees->idCategorie, &c) == 0)
        return -2;

    Livre existant;
    if (trouverLivreParIsbn(donnees->isbn, &existant) == 1)
        return -3;

    *nouveau = *donnees;
    nouveau->id = prochainId();
    nouveau->nombreExemplairesDisponibles = donnees->nombreExemplairesTotal;
    obtenirDateHeureActuelle(nouveau->dateAjout, 20);

    FILE *f = fopen(FICHIER, "ab");
    if (f == NULL) return -3;
    fwrite(nouveau, sizeof(Livre), 1, f);
    fclose(f);
    return 0;
}

/* Trouve par id. 1=trouvé, 0=pas trouvé */
int trouverLivreParId(int id, Livre *res) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) return 0;

    Livre l;
    while (fread(&l, sizeof(Livre), 1, f) == 1) {
        if (l.id == id) {
            *res = l;
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

/* Trouve par ISBN. 1=trouvé, 0=pas trouvé */
int trouverLivreParIsbn(const char *isbn, Livre *res) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) return 0;

    Livre l;
    while (fread(&l, sizeof(Livre), 1, f) == 1) {
        if (strcmp(l.isbn, isbn) == 0) {
            *res = l;
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

/* Modifie un livre. 0=succès, -1=introuvable */
int modifierLivre(int id, const Livre *nouvellesDonnees) {
    FILE *f = fopen(FICHIER, "rb+");
    if (f == NULL) return -3;

    Livre l;
    int rang = 0;
    while (fread(&l, sizeof(Livre), 1, f) == 1) {
        if (l.id == id) {
            Livre modifie = *nouvellesDonnees;
            modifie.id = id;
            fseek(f, rang * sizeof(Livre), SEEK_SET);
            fwrite(&modifie, sizeof(Livre), 1, f);
            fclose(f);
            return 0;
        }
        rang++;
    }
    fclose(f);
    return -1;
}

/* Décrémente le stock (emprunt). 0=succès, -1=stock déjà à 0 */
int decrementerExemplairesDisponibles(int idLivre) {
    FILE *f = fopen(FICHIER, "rb+");
    if (f == NULL) return -3;

    Livre l;
    int rang = 0;
    while (fread(&l, sizeof(Livre), 1, f) == 1) {
        if (l.id == idLivre) {
            if (l.nombreExemplairesDisponibles <= 0) { fclose(f); return -1; }
            l.nombreExemplairesDisponibles--;
            fseek(f, rang * sizeof(Livre), SEEK_SET);
            fwrite(&l, sizeof(Livre), 1, f);
            fclose(f);
            return 0;
        }
        rang++;
    }
    fclose(f);
    return -1;
}

/* Incrémente le stock (retour). 0=succès, -1=déjà au max */
int incrementerExemplairesDisponibles(int idLivre) {
    FILE *f = fopen(FICHIER, "rb+");
    if (f == NULL) return -3;

    Livre l;
    int rang = 0;
    while (fread(&l, sizeof(Livre), 1, f) == 1) {
        if (l.id == idLivre) {
            if (l.nombreExemplairesDisponibles >= l.nombreExemplairesTotal) { fclose(f); return -1; }
            l.nombreExemplairesDisponibles++;
            fseek(f, rang * sizeof(Livre), SEEK_SET);
            fwrite(&l, sizeof(Livre), 1, f);
            fclose(f);
            return 0;
        }
        rang++;
    }
    fclose(f);
    return -1;
}

/* Supprime un livre. Refuse si emprunts actifs. */
int supprimerLivre(int id) {
    if (compterEmpruntsActifsParLivre(id) > 0)
        return -2;

    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) return -3;

    Livre liste[1000];
    int total = 0, trouve = 0;
    Livre l;

    while (fread(&l, sizeof(Livre), 1, f) == 1) {
        if (l.id != id) { liste[total] = l; total++; }
        else { trouve = 1; }
    }
    fclose(f);

    if (!trouve) return -1;

    f = fopen(FICHIER, "wb");
    if (f == NULL) return -3;
    fwrite(liste, sizeof(Livre), total, f);
    fclose(f);
    return 0;
}

/* Compte les livres d'un auteur */
int compterLivresParAuteur(int idAuteur) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) return 0;

    int nb = 0;
    Livre l;
    while (fread(&l, sizeof(Livre), 1, f) == 1) {
        if (l.idAuteur == idAuteur) nb++;
    }
    fclose(f);
    return nb;
}

/* Compte les livres d'une catégorie */
int compterLivresParCategorie(int idCategorie) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) return 0;

    int nb = 0;
    Livre l;
    while (fread(&l, sizeof(Livre), 1, f) == 1) {
        if (l.idCategorie == idCategorie) nb++;
    }
    fclose(f);
    return nb;
}

/* Retourne le stock disponible. -1 si livre introuvable */
int obtenirExemplairesDisponibles(int idLivre) {
    Livre l;
    if (trouverLivreParId(idLivre, &l) == 0) return -1;
    return l.nombreExemplairesDisponibles;
}

/* Liste tous les livres */
int listerLivres(Livre tableau[], int tailleMax, int *nb) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) { *nb = 0; return -3; }

    *nb = 0;
    Livre l;
    while (fread(&l, sizeof(Livre), 1, f) == 1) {
        if (*nb < tailleMax) { tableau[*nb] = l; (*nb)++; }
    }
    fclose(f);
    return 0;
}

/* Recherche par titre (insensible à la casse) */
int rechercherLivresParTitre(const char *motCle, Livre resultats[],
                              int tailleMax, int *nb) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) { *nb = 0; return -3; }

    char motMin[150];
    minuscules(motCle, motMin, 150);

    *nb = 0;
    Livre l;
    while (fread(&l, sizeof(Livre), 1, f) == 1) {
        char titreMin[150];
        minuscules(l.titre, titreMin, 150);
        if (strstr(titreMin, motMin) != NULL) {
            if (*nb < tailleMax) { resultats[*nb] = l; (*nb)++; }
        }
    }
    fclose(f);
    return 0;
}
