#include "../include/borrows.h"

#include "../include/borrows.h"
#include "../include/users.h"
#include "../include/books.h"
#include "../include/history.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#define FICHIER_BORROWS "DATABASE/BORROWS.dat"

/* =============================================
 *  FONCTION 1 : genererNumeroEmprunt
 *  Génère un numéro au format EMP_AAAAMMJJHHMMSS
 *  Exemple : EMP_20260615173025
 * ============================================= */
void genererNumeroEmprunt(char *sortie) {
    time_t t = time(NULL);
    struct tm *maintenant = localtime(&t);

    sprintf(sortie, "EMP_%04d%02d%02d%02d%02d%02d",
            maintenant->tm_year + 1900,
            maintenant->tm_mon + 1,
            maintenant->tm_mday,
            maintenant->tm_hour,
            maintenant->tm_min,
            maintenant->tm_sec);
}

/* =============================================
 *  FONCTION 2 : compterEmpruntsActifsParUtilisateur
 *  Compte les emprunts en cours ou en retard
 *  d'un utilisateur (pour vérifier le max de 3)
 * ============================================= */
int compterEmpruntsActifsParUtilisateur(int idUtilisateur) {
    FILE *f = fopen(FICHIER_BORROWS, "rb");
    if (f == NULL) return 0;

    int compteur = 0;
    Emprunt e;

    while (fread(&e, sizeof(Emprunt), 1, f) == 1) {
        if (e.idUtilisateur == idUtilisateur) {
            if (e.etat == EMPRUNT_EN_COURS || e.etat == EMPRUNT_EN_RETARD) {
                compteur++;
            }
        }
    }

    fclose(f);
    return compteur;
}

/* =============================================
 *  FONCTION 3 : compterEmpruntsActifsParLivre
 *  Compte les emprunts actifs sur un livre
 *  (utilisé avant la suppression d'un livre)
 * ============================================= */
int compterEmpruntsActifsParLivre(int idLivre) {
    FILE *f = fopen(FICHIER_BORROWS, "rb");
    if (f == NULL) return 0;

    int compteur = 0;
    Emprunt e;

    while (fread(&e, sizeof(Emprunt), 1, f) == 1) {
        if (e.idLivre == idLivre) {
            if (e.etat == EMPRUNT_EN_COURS || e.etat == EMPRUNT_EN_RETARD) {
                compteur++;
            }
        }
    }

    fclose(f);
    return compteur;
}

/* =============================================
 *  FONCTION 4 : trouverEmpruntParId
 *  Cherche un emprunt par son id
 *  Retourne 1 si trouvé, 0 sinon
 * ============================================= */
int trouverEmpruntParId(int id, Emprunt *resultat) {
    FILE *f = fopen(FICHIER_BORROWS, "rb");
    if (f == NULL) return 0;

    Emprunt e;

    while (fread(&e, sizeof(Emprunt), 1, f) == 1) {
        if (e.id == id) {
            *resultat = e;
            fclose(f);
            return 1;
        }
    }

    fclose(f);
    return 0;
}

/* =============================================
 *  FONCTION 5 : trouverEmpruntParNumero
 *  Cherche un emprunt par son numéro
 *  (ex : EMP_20260615173025)
 *  Retourne 1 si trouvé, 0 sinon
 * ============================================= */
int trouverEmpruntParNumero(const char *numeroEmprunt, Emprunt *resultat) {
    FILE *f = fopen(FICHIER_BORROWS, "rb");
    if (f == NULL) return 0;

    Emprunt e;

    while (fread(&e, sizeof(Emprunt), 1, f) == 1) {
        if (strcmp(e.numeroEmprunt, numeroEmprunt) == 0) {
            *resultat = e;
            fclose(f);
            return 1;
        }
    }

    fclose(f);
    return 0;
}

/* =============================================
 *  FONCTION 6 : listerEmpruntsActifsParUtilisateur
 *  Remplit un tableau avec les emprunts actifs
 *  (en cours ou en retard) d'un utilisateur
 * ============================================= */
int listerEmpruntsActifsParUtilisateur(int idUtilisateur, Emprunt tableau[],
                                        int tailleMax, int *nombreTrouve) {
    FILE *f = fopen(FICHIER_BORROWS, "rb");
    if (f == NULL) {
        *nombreTrouve = 0;
        return -3;
    }

    *nombreTrouve = 0;
    Emprunt e;

    while (fread(&e, sizeof(Emprunt), 1, f) == 1) {
        if (e.idUtilisateur == idUtilisateur) {
            if (e.etat == EMPRUNT_EN_COURS || e.etat == EMPRUNT_EN_RETARD) {
                if (*nombreTrouve < tailleMax) {
                    tableau[*nombreTrouve] = e;
                    (*nombreTrouve)++;
                }
            }
        }
    }

    fclose(f);
    return 0;
}

/* =============================================
 *  FONCTION 7 : mettreAJourEmpruntsEnRetard
 *  Passe tous les emprunts EN_COURS en EN_RETARD
 *  si la date prévue de retour est dépassée.
 *  À appeler au démarrage dans main().
 * ============================================= */

/* Fonction utilitaire interne : convertit une date
   JJ/MM/AAAA HH:MM:SS en nombre entier AAAAMMJJHHMMSS
   pour pouvoir comparer les dates simplement */
static long dateVersNombre(const char *date) {
    int jour, mois, annee, heure, minute, seconde;
    sscanf(date, "%d/%d/%d %d:%d:%d",
           &jour, &mois, &annee, &heure, &minute, &seconde);
    return (long)annee * 10000000000L +
           (long)mois   * 100000000L +
           (long)jour   * 1000000L +
           (long)heure  * 10000L +
           (long)minute * 100L +
           (long)seconde;
}

int mettreAJourEmpruntsEnRetard(void) {
    FILE *f = fopen(FICHIER_BORROWS, "rb");
    if (f == NULL) return 0;

    /* Lire tous les emprunts en mémoire */
    Emprunt emprunts[1000];
    int nombreTotal = 0;

    while (fread(&emprunts[nombreTotal], sizeof(Emprunt), 1, f) == 1) {
        nombreTotal++;
    }
    fclose(f);

    /* Date du jour */
    char dateAujourdhui[20];
    obtenirDateHeureActuelle(dateAujourdhui, sizeof(dateAujourdhui));
    long nombreAujourdhui = dateVersNombre(dateAujourdhui);

    /* Parcourir et mettre à jour les retards */
    int modifie = 0;
    int i;

    for (i = 0; i < nombreTotal; i++) {
        if (emprunts[i].etat == EMPRUNT_EN_COURS) {
            long nombrePrevu = dateVersNombre(emprunts[i].datePrevueRetour);
            if (nombreAujourdhui > nombrePrevu) {
                emprunts[i].etat = EMPRUNT_EN_RETARD;
                modifie = 1;
            }
        }
    }

    /* Réécrire le fichier si des modifications ont été faites */
    if (modifie) {
        f = fopen(FICHIER_BORROWS, "wb");
        if (f == NULL) return -3;
        fwrite(emprunts, sizeof(Emprunt), nombreTotal, f);
        fclose(f);
    }

    return 0;
}

/* =============================================
 *  FONCTION 8 : creerEmprunt
 *  Fonction principale du module.
 *
 *  Vérifie dans l'ordre :
 *    1. L'utilisateur existe et est actif
 *    2. Le livre existe et a du stock
 *    3. Moins de 3 emprunts en cours
 *
 *  Puis crée l'enregistrement, décrémente le
 *  stock, journalise et génère le reçu.
 *
 *  Retour :
 *    0  = succès
 *   -1  = utilisateur ou livre introuvable
 *   -2  = compte bloqué
 *   -3  = stock à zéro
 *   -4  = limite de 3 emprunts atteinte
 * ============================================= */
int creerEmprunt(int idUtilisateur, int idLivre, Emprunt *nouvelEmprunt) {

    /* ÉTAPE 1 : Vérifier l'utilisateur */
    Utilisateur utilisateur;
    if (trouverUtilisateurParId(idUtilisateur, &utilisateur) == 0) {
        return -1;
    }
    if (utilisateur.etat == ETAT_BLOQUE) {
        return -2;
    }

    /* ÉTAPE 2 : Vérifier le livre */
    Livre livre;
    if (trouverLivreParId(idLivre, &livre) == 0) {
        return -1;
    }
    if (livre.nombreExemplairesDisponibles <= 0) {
        return -3;
    }

    /* ÉTAPE 3 : Vérifier la limite de 3 emprunts */
    if (compterEmpruntsActifsParUtilisateur(idUtilisateur) >= MAX_EMPRUNTS) {
        return -4;
    }

    /* ÉTAPE 4 : Calculer le prochain id */
    FILE *f = fopen(FICHIER_BORROWS, "rb");
    int prochainId = 1;
    if (f != NULL) {
        fseek(f, 0, SEEK_END);
        prochainId = (int)(ftell(f) / sizeof(Emprunt)) + 1;
        fclose(f);
    }

    /* ÉTAPE 5 : Remplir la structure */
    nouvelEmprunt->id = prochainId;
    genererNumeroEmprunt(nouvelEmprunt->numeroEmprunt);
    nouvelEmprunt->idUtilisateur = idUtilisateur;
    nouvelEmprunt->idLivre = idLivre;
    nouvelEmprunt->etat = EMPRUNT_EN_COURS;

    /* Date d'emprunt = maintenant */
    obtenirDateHeureActuelle(nouvelEmprunt->dateEmprunt, 20);

    /* Date de retour prévue = maintenant + 14 jours */
    time_t t = time(NULL);
    t += DUREE_EMPRUNT_JOURS * 24 * 60 * 60;
    struct tm *dans14jours = localtime(&t);
    sprintf(nouvelEmprunt->datePrevueRetour, "%02d/%02d/%04d %02d:%02d:%02d",
            dans14jours->tm_mday, dans14jours->tm_mon + 1,
            dans14jours->tm_year + 1900,
            dans14jours->tm_hour, dans14jours->tm_min,
            dans14jours->tm_sec);

    /* ÉTAPE 6 : Écrire dans le fichier */
    f = fopen(FICHIER_BORROWS, "ab");
    if (f == NULL) return -3;
    fwrite(nouvelEmprunt, sizeof(Emprunt), 1, f);
    fclose(f);

    /* ÉTAPE 7 : Décrémenter le stock du livre */
    decrementerExemplairesDisponibles(idLivre);

    /* ÉTAPE 8 : Journaliser */
    char message[256];
    sprintf(message, "Emprunt %s du livre \"%s\"",
            nouvelEmprunt->numeroEmprunt, livre.titre);
    journaliserOperation(utilisateur.login, message);

    /* ÉTAPE 9 : Générer le reçu */
    Auteur auteur;
    trouverAuteurParId(livre.idAuteur, &auteur);
    genererRecuEmprunt(nouvelEmprunt, &utilisateur, &livre, &auteur);

    return 0;
}
