#include "../include/stats.h"
#include "../include/borrows.h"
#include "../include/returns.h"
#include "../include/reservations.h"
#include "../include/penalties.h"
#include <stdio.h>
#include <string.h>

/* Compare les 10 premiers caractères d'une date (JJ/MM/AAAA) */
static int memeDate(const char *d1, const char *d2) {
    return (strncmp(d1, d2, 10) == 0) ? 1 : 0;
}

/* Compte les emprunts d'une date */
int compterEmpruntsParDate(const char *date) {
    FILE *f = fopen("DATABASE/BORROWS.dat", "rb");
    if (f == NULL) return 0;
    int nb = 0;
    Emprunt e;
    while (fread(&e, sizeof(Emprunt), 1, f) == 1) {
        if (memeDate(e.dateEmprunt, date)) nb++;
    }
    fclose(f);
    return nb;
}

/* Compte les retours d'une date */
int compterRetoursParDate(const char *date) {
    FILE *f = fopen("DATABASE/RETURNS.dat", "rb");
    if (f == NULL) return 0;
    int nb = 0;
    Retour r;
    while (fread(&r, sizeof(Retour), 1, f) == 1) {
        if (memeDate(r.dateRetour, date)) nb++;
    }
    fclose(f);
    return nb;
}

/* Compte les réservations d'une date */
int compterReservationsParDate(const char *date) {
    FILE *f = fopen("DATABASE/RESERVATIONS.dat", "rb");
    if (f == NULL) return 0;
    int nb = 0;
    Reservation r;
    while (fread(&r, sizeof(Reservation), 1, f) == 1) {
        if (memeDate(r.dateReservation, date)) nb++;
    }
    fclose(f);
    return nb;
}

/* Compte les nouveaux utilisateurs d'une date */
int compterNouveauxUtilisateursParDate(const char *date) {
    FILE *f = fopen("DATABASE/USERS.dat", "rb");
    if (f == NULL) return 0;
    int nb = 0;
    Utilisateur u;
    while (fread(&u, sizeof(Utilisateur), 1, f) == 1) {
        if (memeDate(u.dateCreation, date)) nb++;
    }
    fclose(f);
    return nb;
}

/* Compte les pénalités d'une date */
int compterPenalitesParDate(const char *date) {
    FILE *f = fopen("DATABASE/PENALTIES.dat", "rb");
    if (f == NULL) return 0;
    int nb = 0;
    Penalite p;
    while (fread(&p, sizeof(Penalite), 1, f) == 1) {
        if (memeDate(p.date, date)) nb++;
    }
    fclose(f);
    return nb;
}

/* Trouve le livre le plus emprunté. 1=trouvé, 0=aucun emprunt */
int trouverLivrePlusEmprunte(Livre *resultat) {
    FILE *f = fopen("DATABASE/BORROWS.dat", "rb");
    if (f == NULL) return 0;

    int ids[1000], compteurs[1000], nb = 0;
    Emprunt e;

    while (fread(&e, sizeof(Emprunt), 1, f) == 1) {
        int i, trouve = 0;
        for (i = 0; i < nb; i++) {
            if (ids[i] == e.idLivre) {
                compteurs[i]++;
                trouve = 1;
                break;
            }
        }
        if (!trouve) {
            ids[nb] = e.idLivre;
            compteurs[nb] = 1;
            nb++;
        }
    }
    fclose(f);

    if (nb == 0) return 0;

    int maxIdx = 0, i;
    for (i = 1; i < nb; i++) {
        if (compteurs[i] > compteurs[maxIdx]) maxIdx = i;
    }

    return trouverLivreParId(ids[maxIdx], resultat);
}

/* Trouve l'utilisateur le plus actif. 1=trouvé, 0=aucun emprunt */
int trouverUtilisateurPlusActif(Utilisateur *resultat) {
    FILE *f = fopen("DATABASE/BORROWS.dat", "rb");
    if (f == NULL) return 0;

    int ids[1000], compteurs[1000], nb = 0;
    Emprunt e;

    while (fread(&e, sizeof(Emprunt), 1, f) == 1) {
        int i, trouve = 0;
        for (i = 0; i < nb; i++) {
            if (ids[i] == e.idUtilisateur) {
                compteurs[i]++;
                trouve = 1;
                break;
            }
        }
        if (!trouve) {
            ids[nb] = e.idUtilisateur;
            compteurs[nb] = 1;
            nb++;
        }
    }
    fclose(f);

    if (nb == 0) return 0;

    int maxIdx = 0, i;
    for (i = 1; i < nb; i++) {
        if (compteurs[i] > compteurs[maxIdx]) maxIdx = i;
    }

    return trouverUtilisateurParId(ids[maxIdx], resultat);
}

/* Génère le rapport journalier dans REPORTS/DAILY/ */
int genererRapportJournalier(const char *date) {
    int j, m, a;
    sscanf(date, "%d/%d/%d", &j, &m, &a);

    char nomFichier[100];
    sprintf(nomFichier, "REPORTS/DAILY/REPORT_%04d%02d%02d.txt", a, m, j);

    FILE *f = fopen(nomFichier, "w");
    if (f == NULL) return -3;

    Livre livreTop;
    Utilisateur userTop;

    fprintf(f, "========================================\n");
    fprintf(f, "  RAPPORT JOURNALIER - %s\n", date);
    fprintf(f, "  E-LIBRARY ISI DAKAR\n");
    fprintf(f, "========================================\n\n");
    fprintf(f, "Emprunts          : %d\n", compterEmpruntsParDate(date));
    fprintf(f, "Retours           : %d\n", compterRetoursParDate(date));
    fprintf(f, "Reservations      : %d\n", compterReservationsParDate(date));
    fprintf(f, "Nouveaux comptes  : %d\n", compterNouveauxUtilisateursParDate(date));
    fprintf(f, "Penalites         : %d\n", compterPenalitesParDate(date));
    fprintf(f, "Montant penalites : %.0f FCFA\n\n", getTotalPenalitesMontantJour());

    if (trouverLivrePlusEmprunte(&livreTop))
        fprintf(f, "Livre le plus emprunte   : %s (ISBN: %s)\n", livreTop.titre, livreTop.isbn);
    else
        fprintf(f, "Livre le plus emprunte   : Aucun\n");

    if (trouverUtilisateurPlusActif(&userTop))
        fprintf(f, "Utilisateur le plus actif : %s %s (%s)\n", userTop.prenom, userTop.nom, userTop.login);
    else
        fprintf(f, "Utilisateur le plus actif : Aucun\n");

    fclose(f);
    return 0;
}
