#include "../include/returns.h"
#include "../include/borrows.h"
#include "../include/books.h"
#include "../include/reservations.h"
#include "../include/penalties.h"
#include "../include/users.h"
#include "../include/history.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#define FICHIER "DATABASE/RETURNS.dat"

/* Calcule le prochain id */
static int prochainId(void) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) return 1;
    fseek(f, 0, SEEK_END);
    int id = (int)(ftell(f) / sizeof(Retour)) + 1;
    fclose(f);
    return id;
}

/* Calcule le nombre de jours de retard.
   0 si pas de retard, sinon le nombre de jours */
int calculerJoursRetard(const char *datePrevue, const char *dateReelle) {
    int j1, m1, a1, j2, m2, a2;
    sscanf(datePrevue, "%d/%d/%d", &j1, &m1, &a1);
    sscanf(dateReelle, "%d/%d/%d", &j2, &m2, &a2);

    struct tm t1 = {0}, t2 = {0};
    t1.tm_year = a1 - 1900; t1.tm_mon = m1 - 1; t1.tm_mday = j1;
    t2.tm_year = a2 - 1900; t2.tm_mon = m2 - 1; t2.tm_mday = j2;

    double diff = difftime(mktime(&t2), mktime(&t1));
    int jours = (int)(diff / 86400);
    return (jours > 0) ? jours : 0;
}

/* Crée un retour.
   Met à jour 5 fichiers : RETURNS, BORROWS, BOOKS, PENALTIES, RESERVATIONS.
   0=succès, -1=emprunt introuvable, -2=déjà retourné */
int creerRetour(int idEmprunt, Retour *nouveau) {
    Emprunt emprunt;
    if (trouverEmpruntParId(idEmprunt, &emprunt) == 0)
        return -1;
    if (emprunt.etat == EMPRUNT_RETOURNE)
        return -2;

    /* Remplir le retour */
    nouveau->idRetour = prochainId();
    nouveau->idEmprunt = idEmprunt;
    obtenirDateHeureActuelle(nouveau->dateRetour, 20);

    /* Calculer le retard et la pénalité */
    nouveau->joursRetard = calculerJoursRetard(emprunt.datePrevueRetour, nouveau->dateRetour);

    if (nouveau->joursRetard > 0) {
        nouveau->montantPenalite = nouveau->joursRetard * PENALITE_PAR_JOUR;
        Penalite p;
        creerPenalite(emprunt.idUtilisateur, idEmprunt,
                      nouveau->joursRetard, nouveau->montantPenalite, &p);
    } else {
        nouveau->montantPenalite = 0;
    }

    /* Écrire dans RETURNS.dat */
    FILE *f = fopen(FICHIER, "ab");
    if (f == NULL) return -3;
    fwrite(nouveau, sizeof(Retour), 1, f);
    fclose(f);

    /* Mettre à jour l'emprunt → RETOURNE */
    f = fopen("DATABASE/BORROWS.dat", "rb+");
    if (f != NULL) {
        Emprunt e;
        int rang = 0;
        while (fread(&e, sizeof(Emprunt), 1, f) == 1) {
            if (e.id == idEmprunt) {
                e.etat = EMPRUNT_RETOURNE;
                fseek(f, rang * sizeof(Emprunt), SEEK_SET);
                fwrite(&e, sizeof(Emprunt), 1, f);
                break;
            }
            rang++;
        }
        fclose(f);
    }

    /* Incrémenter le stock du livre */
    incrementerExemplairesDisponibles(emprunt.idLivre);

    /* Promouvoir la première réservation en attente */
    promouvoirPremiereReservation(emprunt.idLivre);

    /* Journaliser et générer le reçu */
    Utilisateur u;
    trouverUtilisateurParId(emprunt.idUtilisateur, &u);
    Livre livre;
    trouverLivreParId(emprunt.idLivre, &livre);

    char msg[256];
    sprintf(msg, "Retour du livre \"%s\" (emprunt %s) - %d jours de retard",
            livre.titre, emprunt.numeroEmprunt, nouveau->joursRetard);
    journaliserOperation(u.login, msg);
    genererRecuRetour(nouveau, &emprunt, &u, &livre);

    return 0;
}

/* Liste tous les retours */
int listerRetours(Retour tableau[], int tailleMax, int *nb) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) { *nb = 0; return -3; }

    *nb = 0;
    Retour r;
    while (fread(&r, sizeof(Retour), 1, f) == 1) {
        if (*nb < tailleMax) { tableau[*nb] = r; (*nb)++; }
    }
    fclose(f);
    return 0;
}

/* Liste les retours d'un utilisateur */
int listerRetoursParUtilisateur(int idUtilisateur, Retour tableau[],
                                 int tailleMax, int *nb) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) { *nb = 0; return -3; }

    *nb = 0;
    Retour r;
    while (fread(&r, sizeof(Retour), 1, f) == 1) {
        Emprunt e;
        if (trouverEmpruntParId(r.idEmprunt, &e) == 1) {
            if (e.idUtilisateur == idUtilisateur) {
                if (*nb < tailleMax) { tableau[*nb] = r; (*nb)++; }
            }
        }
    }
    fclose(f);
    return 0;
}