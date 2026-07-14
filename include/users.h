#ifndef USERS_H
#define USERS_H

#include "utils.h"

typedef struct {
    int   id;
    char  nom[50];
    char  prenom[50];
    char  telephone[20];
    char  adresse[100];
    char  email[100];
    char  login[7];
    char  motDePasse[65];
    Role  role;
    EtatCompte etat;
    char  dateCreation[20];
    char  dateDerniereConnexion[20];
    int   doitChangerMotDePasse;
} Utilisateur;

int initialiserFichierUtilisateurs(void);
int creerUtilisateur(const Utilisateur *donnees, Utilisateur *nouvelUtilisateur);
int trouverUtilisateurParLogin(const char *login, Utilisateur *resultat);
int trouverUtilisateurParId(int id, Utilisateur *resultat);
int authentifierUtilisateur(const char *login, const char *motDePasseSaisi, Utilisateur *sessionUtilisateur);
int changerMotDePasse(int idUtilisateur, const char *nouveauMotDePasse);
int bloquerUtilisateur(int idUtilisateur);
int debloquerUtilisateur(int idUtilisateur);
int listerUtilisateurs(Utilisateur tableau[], int tailleMax, int *nombreTrouve);

#endif