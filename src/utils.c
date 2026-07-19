#include "../include/utils.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

/* Vérifie que le login fait 6 lettres majuscules */
int loginValide(const char *login) {
    if (strlen(login) != 6) return 0;
    int i;
    for (i = 0; i < 6; i++) {
        if (login[i] < 'A' || login[i] > 'Z') return 0;
    }
    return 1;
}

/* Récupère la date et l'heure au format JJ/MM/AAAA HH:MM:SS */
void obtenirDateHeureActuelle(char *buffer, int taille) {
    time_t t = time(NULL);
    struct tm *m = localtime(&t);
    snprintf(buffer, taille, "%02d/%02d/%04d %02d:%02d:%02d",
             m->tm_mday, m->tm_mon + 1, m->tm_year + 1900,
             m->tm_hour, m->tm_min, m->tm_sec);
}

/* Hachage simple djb2. Jamais stocké en clair. */
int hasherMotDePasse(const char *motDePasse, char *sortie, int tailleSortie) {
    unsigned long hash = 5381;
    int i;
    for (i = 0; motDePasse[i] != '\0'; i++) {
        hash = ((hash << 5) + hash) + motDePasse[i];
    }
    snprintf(sortie, tailleSortie, "%lu", hash);
    return 1;
}