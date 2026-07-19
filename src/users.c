#include "../include/users.h"
#include "../include/users.h"
#include <stdio.h>
#include <string.h>

#define FICHIER "DATABASE/USERS.dat"

/* Trouve la position d'un utilisateur dans le fichier.
   Retourne le rang (0, 1, 2...) ou -1 si pas trouvé.
   Remplit le résultat si trouvé. */
static int chercherUtilisateur(const char *login, int id, int mode, Utilisateur *res) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) return -1;

    Utilisateur u;
    int rang = 0;
    while (fread(&u, sizeof(Utilisateur), 1, f) == 1) {
        /* mode 0 = chercher par login, mode 1 = chercher par id */
        if ((mode == 0 && strcmp(u.login, login) == 0) ||
            (mode == 1 && u.id == id)) {
            *res = u;
            fclose(f);
            return rang;
        }
        rang++;
    }
    fclose(f);
    return -1;
}

/* Calcule le prochain id disponible */
static int prochainId(void) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) return 1;
    fseek(f, 0, SEEK_END);
    int id = (int)(ftell(f) / sizeof(Utilisateur)) + 1;
    fclose(f);
    return id;
}

/* Crée le fichier USERS.dat avec un admin par défaut */
int initialiserFichierUtilisateurs(void) {
    FILE *f = fopen(FICHIER, "rb");
    if (f != NULL) { fclose(f); return 0; }

    f = fopen(FICHIER, "ab");
    if (f == NULL) return -3;

    Utilisateur admin;
    memset(&admin, 0, sizeof(Utilisateur));
    admin.id = 1;
    strcpy(admin.nom, "Admin");
    strcpy(admin.prenom, "Systeme");
    strcpy(admin.login, "ADMINS");
    hasherMotDePasse("Library123", admin.motDePasse, 65);
    admin.role = ROLE_ADMIN;
    admin.etat = ETAT_ACTIF;
    admin.doitChangerMotDePasse = 1;
    obtenirDateHeureActuelle(admin.dateCreation, 20);

    fwrite(&admin, sizeof(Utilisateur), 1, f);
    fclose(f);
    return 0;
}

/* Crée un utilisateur. 0=succès, -1=login invalide, -2=login déjà pris */
int creerUtilisateur(const Utilisateur *donnees, Utilisateur *nouveau) {
    if (!loginValide(donnees->login)) return -1;

    Utilisateur tmp;
    if (chercherUtilisateur(donnees->login, 0, 0, &tmp) >= 0) return -2;

    *nouveau = *donnees;
    nouveau->id = prochainId();
    hasherMotDePasse("Library123", nouveau->motDePasse, 65);
    nouveau->doitChangerMotDePasse = 1;
    nouveau->etat = ETAT_ACTIF;
    nouveau->role = ROLE_USER;
    obtenirDateHeureActuelle(nouveau->dateCreation, 20);

    FILE *f = fopen(FICHIER, "ab");
    if (f == NULL) return -3;
    fwrite(nouveau, sizeof(Utilisateur), 1, f);
    fclose(f);
    return 0;
}

/* Trouve par login. 1=trouvé, 0=pas trouvé */
int trouverUtilisateurParLogin(const char *login, Utilisateur *res) {
    return (chercherUtilisateur(login, 0, 0, res) >= 0) ? 1 : 0;
}

/* Trouve par id. 1=trouvé, 0=pas trouvé */
int trouverUtilisateurParId(int id, Utilisateur *res) {
    return (chercherUtilisateur(NULL, id, 1, res) >= 0) ? 1 : 0;
}

/* Authentification. 1=succès, 0=erreur, -1=bloqué */
int authentifierUtilisateur(const char *login, const char *mdp, Utilisateur *session) {
    Utilisateur u;
    int rang = chercherUtilisateur(login, 0, 0, &u);
    if (rang < 0) return 0;
    if (u.etat == ETAT_BLOQUE) return -1;

    char hash[65];
    hasherMotDePasse(mdp, hash, 65);
    if (strcmp(hash, u.motDePasse) != 0) return 0;

    /* Mettre à jour la date de connexion */
    FILE *f = fopen(FICHIER, "rb+");
    if (f != NULL) {
        obtenirDateHeureActuelle(u.dateDerniereConnexion, 20);
        fseek(f, rang * sizeof(Utilisateur), SEEK_SET);
        fwrite(&u, sizeof(Utilisateur), 1, f);
        fclose(f);
    }

    *session = u;
    return 1;
}

/* Change le mot de passe. 0=succès, -1=introuvable */
int changerMotDePasse(int id, const char *nouveau) {
    FILE *f = fopen(FICHIER, "rb+");
    if (f == NULL) return -3;

    Utilisateur u;
    int rang = 0;
    while (fread(&u, sizeof(Utilisateur), 1, f) == 1) {
        if (u.id == id) {
            hasherMotDePasse(nouveau, u.motDePasse, 65);
            u.doitChangerMotDePasse = 0;
            fseek(f, rang * sizeof(Utilisateur), SEEK_SET);
            fwrite(&u, sizeof(Utilisateur), 1, f);
            fclose(f);
            return 0;
        }
        rang++;
    }
    fclose(f);
    return -1;
}

/* Bloque un compte. 0=succès, -1=introuvable */
int bloquerUtilisateur(int id) {
    FILE *f = fopen(FICHIER, "rb+");
    if (f == NULL) return -3;

    Utilisateur u;
    int rang = 0;
    while (fread(&u, sizeof(Utilisateur), 1, f) == 1) {
        if (u.id == id) {
            u.etat = ETAT_BLOQUE;
            fseek(f, rang * sizeof(Utilisateur), SEEK_SET);
            fwrite(&u, sizeof(Utilisateur), 1, f);
            fclose(f);
            return 0;
        }
        rang++;
    }
    fclose(f);
    return -1;
}

/* Débloque un compte. 0=succès, -1=introuvable */
int debloquerUtilisateur(int id) {
    FILE *f = fopen(FICHIER, "rb+");
    if (f == NULL) return -3;

    Utilisateur u;
    int rang = 0;
    while (fread(&u, sizeof(Utilisateur), 1, f) == 1) {
        if (u.id == id) {
            u.etat = ETAT_ACTIF;
            fseek(f, rang * sizeof(Utilisateur), SEEK_SET);
            fwrite(&u, sizeof(Utilisateur), 1, f);
            fclose(f);
            return 0;
        }
        rang++;
    }
    fclose(f);
    return -1;
}

/* Liste tous les utilisateurs */
int listerUtilisateurs(Utilisateur tableau[], int tailleMax, int *nb) {
    FILE *f = fopen(FICHIER, "rb");
    if (f == NULL) { *nb = 0; return -3; }

    *nb = 0;
    Utilisateur u;
    while (fread(&u, sizeof(Utilisateur), 1, f) == 1) {
        if (*nb < tailleMax) {
            tableau[*nb] = u;
            (*nb)++;
        }
    }
    fclose(f);
    return 0;
}