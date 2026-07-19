#include "../include/penalties.h"
#include "../include/utils.h"
#include <stdio.h>
#include <string.h>

#define FICHIER "DATABASE/PENALTIES.dat"

/* Calcule le prochain id */
static int prochainId(void) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) return 1;
    fseek(f, 0, SEEK_END);
    int id = (int)(ftell(f) / sizeof(Penalite)) + 1;
    fclose(f);
    return id;
}

/* Crée une pénalité. Appelée uniquement par creerRetour(). 0=succès */
int creerPenalite(int idUtilisateur, int idEmprunt, int joursRetard,
                   float montant, Penalite *nouvelle) {
    nouvelle->id = prochainId();
    nouvelle->idUtilisateur = idUtilisateur;
    nouvelle->idEmprunt = idEmprunt;
    nouvelle->joursRetard = joursRetard;
    nouvelle->montant = montant;
    obtenirDateHeureActuelle(nouvelle->date, 20);

    FILE *f = fopen(FICHIER, "ab");
    if (f == NULL) return -3;
    fwrite(nouvelle, sizeof(Penalite), 1, f);
    fclose(f);
    return 0;
}

/* Liste les pénalités d'un utilisateur */
int listerPenalitesParUtilisateur(int idUtilisateur, Penalite tableau[],
                                   int tailleMax, int *nb) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) { *nb = 0; return -3; }

    *nb = 0;
    Penalite p;
    while (fread(&p, sizeof(Penalite), 1, f) == 1) {
        if (p.idUtilisateur == idUtilisateur) {
            if (*nb < tailleMax) { tableau[*nb] = p; (*nb)++; }
        }
    }
    fclose(f);
    return 0;
}

/* Total des pénalités d'un utilisateur */
float getTotalPenalitesMontant(int idUtilisateur) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) return 0;

    float total = 0;
    Penalite p;
    while (fread(&p, sizeof(Penalite), 1, f) == 1) {
        if (p.idUtilisateur == idUtilisateur) total += p.montant;
    }
    fclose(f);
    return total;
}

/* Total des pénalités du jour (pour le rapport journalier) */
float getTotalPenalitesMontantJour(void) {
    char aujourdhui[11];
    obtenirDateHeureActuelle(aujourdhui, 11);

    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) return 0;

    float total = 0;
    Penalite p;
    while (fread(&p, sizeof(Penalite), 1, f) == 1) {
        if (strncmp(p.date, aujourdhui, 10) == 0) total += p.montant;
    }
    fclose(f);
    return total;
}