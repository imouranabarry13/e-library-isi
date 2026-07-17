#include "../include/history.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#endif

/* Crée les dossiers du projet s'ils n'existent pas */
int creerDossiersProjet(void) {
    mkdir("DATABASE", 0755);
    mkdir("REPORTS", 0755);
    mkdir("REPORTS/BORROWS", 0755);
    mkdir("REPORTS/RETURNS", 0755);
    mkdir("REPORTS/DAILY", 0755);
    return 0;
}

/* Journalise une opération dans REPORTS/HISTORY.txt */
int journaliserOperation(const char *login, const char *operation) {
    FILE *f = fopen("REPORTS/HISTORY.txt", "a");
    if (f == NULL) return -3;

    time_t t = time(NULL);
    struct tm *m = localtime(&t);

    fprintf(f, "[%02d/%02d/%04d %02d:%02d:%02d] %s — %s\n",
            m->tm_mday, m->tm_mon + 1, m->tm_year + 1900,
            m->tm_hour, m->tm_min, m->tm_sec, login, operation);

    fclose(f);
    return 0;
}

/* Génère un reçu d'emprunt dans REPORTS/BORROWS/ */
int genererRecuEmprunt(const Emprunt *emprunt, const Utilisateur *utilisateur,
                        const Livre *livre, const Auteur *auteur) {
    char nom[256];
    sprintf(nom, "REPORTS/BORROWS/BORROW_%s_%s.txt",
            emprunt->numeroEmprunt, utilisateur->login);

    FILE *f = fopen(nom, "w");
    if (f == NULL) return -3;

    fprintf(f, "========================================\n");
    fprintf(f, "         RECU D'EMPRUNT\n");
    fprintf(f, "    E-LIBRARY ISI DAKAR\n");
    fprintf(f, "========================================\n\n");
    fprintf(f, "Numero d'emprunt : %s\n", emprunt->numeroEmprunt);
    fprintf(f, "Lecteur          : %s %s\n", utilisateur->prenom, utilisateur->nom);
    fprintf(f, "Livre            : %s\n", livre->titre);
    fprintf(f, "Auteur           : %s\n", auteur->nomComplet);
    fprintf(f, "ISBN             : %s\n", livre->isbn);
    fprintf(f, "Date d'emprunt   : %s\n", emprunt->dateEmprunt);
    fprintf(f, "Retour prevu     : %s\n", emprunt->datePrevueRetour);
    fprintf(f, "\n========================================\n");

    fclose(f);
    return 0;
}

/* Génère un reçu de retour dans REPORTS/RETURNS/ */
int genererRecuRetour(const Retour *retour, const Emprunt *emprunt,
                       const Utilisateur *utilisateur, const Livre *livre) {
    char nom[256];
    sprintf(nom, "REPORTS/RETURNS/RETURN_%s_%s.txt",
            emprunt->numeroEmprunt, utilisateur->login);

    FILE *f = fopen(nom, "w");
    if (f == NULL) return -3;

    fprintf(f, "========================================\n");
    fprintf(f, "          RECU DE RETOUR\n");
    fprintf(f, "    E-LIBRARY ISI DAKAR\n");
    fprintf(f, "========================================\n\n");
    fprintf(f, "Numero d'emprunt : %s\n", emprunt->numeroEmprunt);
    fprintf(f, "Lecteur          : %s %s\n", utilisateur->prenom, utilisateur->nom);
    fprintf(f, "Livre            : %s\n", livre->titre);
    fprintf(f, "Date de retour   : %s\n", retour->dateRetour);

    if (retour->joursRetard > 0) {
        fprintf(f, "Retard           : %d jour(s)\n", retour->joursRetard);
        fprintf(f, "Penalite         : %.0f FCFA\n", retour->montantPenalite);
    } else {
        fprintf(f, "Retard           : Aucun\n");
        fprintf(f, "Penalite         : 0 FCFA\n");
    }

    fprintf(f, "\n========================================\n");

    fclose(f);
    return 0;
}
