#ifndef STATS_H
#define STATS_H

#include "books.h"
#include "users.h"

int compterEmpruntsParDate(const char *date);
int compterRetoursParDate(const char *date);
int compterReservationsParDate(const char *date);
int compterNouveauxUtilisateursParDate(const char *date);
int compterPenalitesParDate(const char *date);
int trouverLivrePlusEmprunte(Livre *resultat);
int trouverUtilisateurPlusActif(Utilisateur *resultat);
int genererRapportJournalier(const char *date);

#endif