#ifndef UTILS_H
#define UTILS_H

typedef enum { ROLE_ADMIN, ROLE_USER } Role;
typedef enum { ETAT_ACTIF, ETAT_BLOQUE } EtatCompte;
typedef enum { EMPRUNT_EN_COURS, EMPRUNT_RETOURNE, EMPRUNT_EN_RETARD } EtatEmprunt;
typedef enum { RESA_EN_ATTENTE, RESA_DISPONIBLE, RESA_ANNULEE } EtatReservation;

#define MAX_EMPRUNTS        3
#define DUREE_EMPRUNT_JOURS 14
#define PENALITE_PAR_JOUR   500.0f

int  loginValide(const char *login);
void obtenirDateHeureActuelle(char *buffer, int taille);
int  hasherMotDePasse(const char *motDePasse, char *sortie, int tailleSortie);

#endif