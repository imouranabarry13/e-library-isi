#include "../include/reservations.h"
#include "../include/books.h"
#include "../include/utils.h"
#include <stdio.h>
#include <string.h>

#define FICHIER "DATABASE/RESERVATIONS.dat"

/* Calcule le prochain id */
static int prochainId(void) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) return 1;
    fseek(f, 0, SEEK_END);
    int id = (int)(ftell(f) / sizeof(Reservation)) + 1;
    fclose(f);
    return id;
}

/* Crée une réservation.
   Refuse si le livre a encore du stock.
   0=succès, -1=livre encore disponible */
int creerReservation(int idUtilisateur, int idLivre, Reservation *nouvelle) {
    if (obtenirExemplairesDisponibles(idLivre) > 0)
        return -1;

    nouvelle->id = prochainId();
    nouvelle->idUtilisateur = idUtilisateur;
    nouvelle->idLivre = idLivre;
    nouvelle->etat = RESA_EN_ATTENTE;
    obtenirDateHeureActuelle(nouvelle->dateReservation, 20);

    FILE *f = fopen(FICHIER, "ab");
    if (f == NULL) return -3;
    fwrite(nouvelle, sizeof(Reservation), 1, f);
    fclose(f);
    return 0;
}

/* Promeut la première réservation en attente pour un livre.
   Appelée après un retour.
   1=promue, 0=aucune réservation en attente */
int promouvoirPremiereReservation(int idLivre) {
    FILE *f = fopen(FICHIER, "rb+");
    if (f == NULL) return 0;

    Reservation r;
    int rang = 0;
    while (fread(&r, sizeof(Reservation), 1, f) == 1) {
        if (r.idLivre == idLivre && r.etat == RESA_EN_ATTENTE) {
            r.etat = RESA_DISPONIBLE;
            fseek(f, rang * sizeof(Reservation), SEEK_SET);
            fwrite(&r, sizeof(Reservation), 1, f);
            fclose(f);
            return 1;
        }
        rang++;
    }
    fclose(f);
    return 0;
}

/* Annule une réservation. 0=succès, -1=introuvable */
int annulerReservation(int idReservation) {
    FILE *f = fopen(FICHIER, "rb+");
    if (f == NULL) return -3;

    Reservation r;
    int rang = 0;
    while (fread(&r, sizeof(Reservation), 1, f) == 1) {
        if (r.id == idReservation) {
            r.etat = RESA_ANNULEE;
            fseek(f, rang * sizeof(Reservation), SEEK_SET);
            fwrite(&r, sizeof(Reservation), 1, f);
            fclose(f);
            return 0;
        }
        rang++;
    }
    fclose(f);
    return -1;
}

/* Liste les réservations d'un livre */
int listerReservationsParLivre(int idLivre, Reservation tableau[],
                                int tailleMax, int *nb) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) { *nb = 0; return -3; }

    *nb = 0;
    Reservation r;
    while (fread(&r, sizeof(Reservation), 1, f) == 1) {
        if (r.idLivre == idLivre) {
            if (*nb < tailleMax) { tableau[*nb] = r; (*nb)++; }
        }
    }
    fclose(f);
    return 0;
}

/* Liste les réservations d'un utilisateur */
int listerReservationsParUtilisateur(int idUtilisateur, Reservation tableau[],
                                      int tailleMax, int *nb) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) { *nb = 0; return -3; }

    *nb = 0;
    Reservation r;
    while (fread(&r, sizeof(Reservation), 1, f) == 1) {
        if (r.idUtilisateur == idUtilisateur) {
            if (*nb < tailleMax) { tableau[*nb] = r; (*nb)++; }
        }
    }
    fclose(f);
    return 0;
}
