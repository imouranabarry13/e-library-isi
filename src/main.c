#include "../include/users.h"
#include "../include/authors.h"
#include "../include/categories.h"
#include "../include/books.h"
#include "../include/borrows.h"
#include "../include/returns.h"
#include "../include/reservations.h"
#include "../include/penalties.h"
#include "../include/stats.h"
#include "../include/history.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void afficherMenuAdmin(Utilisateur session);
void afficherMenuUtilisateur(Utilisateur session);
void sousMenuUtilisateurs(void);
void sousMenuAuteurs(void);
void sousMenuCategories(void);
void sousMenuLivres(void);

/* ===== POINT D'ENTRÉE ===== */
int main(void) {
    creerDossiersProjet();
    initialiserFichierUtilisateurs();
    mettreAJourEmpruntsEnRetard();

    Utilisateur session;
    int choix;

    while (1) {
        printf("\n========================================\n");
        printf("   E-LIBRARY ISI DAKAR\n");
        printf("========================================\n");
        printf("1. Se connecter\n");
        printf("0. Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choix);

        if (choix == 0) { printf("Au revoir !\n"); break; }
        if (choix != 1) { printf("Choix invalide.\n"); continue; }

        char login[7], mdp[50];
        printf("Login : "); scanf("%s", login);
        printf("Mot de passe : "); scanf("%s", mdp);

        int res = authentifierUtilisateur(login, mdp, &session);

        if (res == -1) {
            printf("Compte bloque. Contactez un administrateur.\n");
        } else if (res == 0) {
            printf("Login ou mot de passe incorrect.\n");
        } else {
            printf("Bienvenue %s %s !\n", session.prenom, session.nom);

            if (session.doitChangerMotDePasse == 1) {
                char nouveau[50];
                printf("\n--- Premiere connexion : changez votre mot de passe ---\n");
                printf("Nouveau mot de passe : ");
                scanf("%s", nouveau);
                changerMotDePasse(session.id, nouveau);
                printf("Mot de passe change !\n");
            }

            if (session.role == ROLE_ADMIN)
                afficherMenuAdmin(session);
            else
                afficherMenuUtilisateur(session);
        }
    }
    return 0;
}

/* ===== SOUS-MENU : GESTION DES UTILISATEURS ===== */
void sousMenuUtilisateurs(void) {
    int choix = 0;
    while (choix != 5) {
        printf("\n-- UTILISATEURS --\n");
        printf("1. Creer un utilisateur\n");
        printf("2. Lister les utilisateurs\n");
        printf("3. Bloquer un utilisateur\n");
        printf("4. Debloquer un utilisateur\n");
        printf("5. Retour\n");
        printf("Choix : "); scanf("%d", &choix);

        if (choix == 1) {
            Utilisateur donnees, nouveau;
            memset(&donnees, 0, sizeof(Utilisateur));
            printf("Nom : "); scanf("%s", donnees.nom);
            printf("Prenom : "); scanf("%s", donnees.prenom);
            printf("Login (6 MAJUSCULES) : "); scanf("%s", donnees.login);
            printf("Telephone : "); scanf("%s", donnees.telephone);
            printf("Email : "); scanf("%s", donnees.email);

            int r = creerUtilisateur(&donnees, &nouveau);
            if (r == 0) printf("Cree ! ID=%d, mdp=Library123\n", nouveau.id);
            else if (r == -1) printf("Erreur : login invalide.\n");
            else if (r == -2) printf("Erreur : login deja utilise.\n");
        }
        else if (choix == 2) {
            Utilisateur liste[100];
            int nb, i;
            listerUtilisateurs(liste, 100, &nb);
            for (i = 0; i < nb; i++) {
                printf("[%d] %s %s - Login: %s - Role: %s - Etat: %s\n",
                    liste[i].id, liste[i].prenom, liste[i].nom,
                    liste[i].login,
                    liste[i].role == ROLE_ADMIN ? "ADMIN" : "USER",
                    liste[i].etat == ETAT_ACTIF ? "Actif" : "Bloque");
            }
        }
        else if (choix == 3) {
            int id; printf("ID utilisateur : "); scanf("%d", &id);
            if (bloquerUtilisateur(id) == 0) printf("Bloque.\n");
            else printf("Introuvable.\n");
        }
        else if (choix == 4) {
            int id; printf("ID utilisateur : "); scanf("%d", &id);
            if (debloquerUtilisateur(id) == 0) printf("Debloque.\n");
            else printf("Introuvable.\n");
        }
    }
}

/* ===== SOUS-MENU : GESTION DES AUTEURS ===== */
void sousMenuAuteurs(void) {
    int choix = 0;
    while (choix != 5) {
        printf("\n-- AUTEURS --\n");
        printf("1. Creer un auteur\n");
        printf("2. Lister les auteurs\n");
        printf("3. Modifier un auteur\n");
        printf("4. Supprimer un auteur\n");
        printf("5. Retour\n");
        printf("Choix : "); scanf("%d", &choix);

        if (choix == 1) {
            Auteur donnees, nouveau;
            memset(&donnees, 0, sizeof(Auteur));
            printf("Nom complet : "); getchar();
            fgets(donnees.nomComplet, 100, stdin);
            donnees.nomComplet[strcspn(donnees.nomComplet, "\n")] = 0;
            printf("Nationalite : "); scanf("%s", donnees.nationalite);
            printf("Date naissance (JJ/MM/AAAA) : "); scanf("%s", donnees.dateNaissance);

            if (creerAuteur(&donnees, &nouveau) == 0)
                printf("Cree ! ID=%d\n", nouveau.id);
        }
        else if (choix == 2) {
            Auteur liste[100];
            int nb, i;
            listerAuteurs(liste, 100, &nb);
            for (i = 0; i < nb; i++) {
                printf("[%d] %s - %s\n", liste[i].id, liste[i].nomComplet, liste[i].nationalite);
            }
        }
        else if (choix == 3) {
            int id; printf("ID auteur : "); scanf("%d", &id);
            Auteur donnees;
            memset(&donnees, 0, sizeof(Auteur));
            printf("Nouveau nom : "); getchar();
            fgets(donnees.nomComplet, 100, stdin);
            donnees.nomComplet[strcspn(donnees.nomComplet, "\n")] = 0;
            printf("Nouvelle nationalite : "); scanf("%s", donnees.nationalite);

            if (modifierAuteur(id, &donnees) == 0) printf("Modifie.\n");
            else printf("Introuvable.\n");
        }
        else if (choix == 4) {
            int id; printf("ID auteur : "); scanf("%d", &id);
            int r = supprimerAuteur(id);
            if (r == 0) printf("Supprime.\n");
            else if (r == -2) printf("Erreur : des livres sont associes.\n");
            else printf("Introuvable.\n");
        }
    }
}

/* ===== SOUS-MENU : GESTION DES CATEGORIES ===== */
void sousMenuCategories(void) {
    int choix = 0;
    while (choix != 5) {
        printf("\n-- CATEGORIES --\n");
        printf("1. Creer une categorie\n");
        printf("2. Lister les categories\n");
        printf("3. Modifier une categorie\n");
        printf("4. Supprimer une categorie\n");
        printf("5. Retour\n");
        printf("Choix : "); scanf("%d", &choix);

        if (choix == 1) {
            Categorie donnees, nouveau;
            memset(&donnees, 0, sizeof(Categorie));
            printf("Libelle : "); scanf("%s", donnees.libelle);
            printf("Description : "); getchar();
            fgets(donnees.description, 200, stdin);
            donnees.description[strcspn(donnees.description, "\n")] = 0;

            if (creerCategorie(&donnees, &nouveau) == 0)
                printf("Creee ! ID=%d\n", nouveau.id);
        }
        else if (choix == 2) {
            Categorie liste[100];
            int nb, i;
            listerCategories(liste, 100, &nb);
            for (i = 0; i < nb; i++) {
                printf("[%d] %s - %s\n", liste[i].id, liste[i].libelle, liste[i].description);
            }
        }
        else if (choix == 3) {
            int id; printf("ID categorie : "); scanf("%d", &id);
            Categorie donnees;
            memset(&donnees, 0, sizeof(Categorie));
            printf("Nouveau libelle : "); scanf("%s", donnees.libelle);
            printf("Nouvelle description : "); getchar();
            fgets(donnees.description, 200, stdin);
            donnees.description[strcspn(donnees.description, "\n")] = 0;

            if (modifierCategorie(id, &donnees) == 0) printf("Modifiee.\n");
            else printf("Introuvable.\n");
        }
        else if (choix == 4) {
            int id; printf("ID categorie : "); scanf("%d", &id);
            int r = supprimerCategorie(id);
            if (r == 0) printf("Supprimee.\n");
            else if (r == -2) printf("Erreur : des livres utilisent cette categorie.\n");
            else printf("Introuvable.\n");
        }
    }
}

/* ===== SOUS-MENU : GESTION DES LIVRES ===== */
void sousMenuLivres(void) {
    int choix = 0;
    while (choix != 5) {
        printf("\n-- LIVRES --\n");
        printf("1. Creer un livre\n");
        printf("2. Lister les livres\n");
        printf("3. Rechercher par titre\n");
        printf("4. Supprimer un livre\n");
        printf("5. Retour\n");
        printf("Choix : "); scanf("%d", &choix);

        if (choix == 1) {
            Livre donnees, nouveau;
            memset(&donnees, 0, sizeof(Livre));
            printf("ISBN : "); scanf("%s", donnees.isbn);
            printf("Titre : "); getchar();
            fgets(donnees.titre, 150, stdin);
            donnees.titre[strcspn(donnees.titre, "\n")] = 0;
            printf("ID auteur : "); scanf("%d", &donnees.idAuteur);
            printf("ID categorie : "); scanf("%d", &donnees.idCategorie);
            printf("Editeur : "); scanf("%s", donnees.editeur);
            printf("Annee publication : "); scanf("%d", &donnees.anneePublication);
            printf("Langue : "); scanf("%s", donnees.langue);
            printf("Nombre de pages : "); scanf("%d", &donnees.nombrePages);
            printf("Nombre d'exemplaires : "); scanf("%d", &donnees.nombreExemplairesTotal);
            printf("Emplacement : "); scanf("%s", donnees.emplacement);

            int r = creerLivre(&donnees, &nouveau);
            if (r == 0) printf("Cree ! ID=%d\n", nouveau.id);
            else if (r == -1) printf("Erreur : auteur inexistant.\n");
            else if (r == -2) printf("Erreur : categorie inexistante.\n");
            else if (r == -3) printf("Erreur : ISBN deja utilise.\n");
        }
        else if (choix == 2) {
            Livre liste[100];
            int nb, i;
            listerLivres(liste, 100, &nb);
            for (i = 0; i < nb; i++) {
                printf("[%d] %s (ISBN: %s) - %d/%d dispo\n",
                    liste[i].id, liste[i].titre, liste[i].isbn,
                    liste[i].nombreExemplairesDisponibles,
                    liste[i].nombreExemplairesTotal);
            }
        }
        else if (choix == 3) {
            char motCle[150];
            printf("Mot-cle : "); getchar();
            fgets(motCle, 150, stdin);
            motCle[strcspn(motCle, "\n")] = 0;

            Livre resultats[50];
            int nb, i;
            rechercherLivresParTitre(motCle, resultats, 50, &nb);
            if (nb == 0) printf("Aucun livre trouve.\n");
            for (i = 0; i < nb; i++) {
                printf("[%d] %s - %d dispo\n",
                    resultats[i].id, resultats[i].titre,
                    resultats[i].nombreExemplairesDisponibles);
            }
        }
        else if (choix == 4) {
            int id; printf("ID livre : "); scanf("%d", &id);
            int r = supprimerLivre(id);
            if (r == 0) printf("Supprime.\n");
            else if (r == -2) printf("Erreur : livre actuellement emprunte.\n");
            else printf("Introuvable.\n");
        }
    }
}

/* ===== MENU ADMIN ===== */
void afficherMenuAdmin(Utilisateur session) {
    int choix = 0;
    while (choix != 9) {
        printf("\n--- MENU ADMIN (%s) ---\n", session.login);
        printf("1. Gestion des utilisateurs\n");
        printf("2. Gestion des auteurs\n");
        printf("3. Gestion des categories\n");
        printf("4. Gestion des livres\n");
        printf("5. Consulter les emprunts\n");
        printf("6. Consulter les penalites\n");
        printf("7. Generer le rapport du jour\n");
        printf("8. Changer mon mot de passe\n");
        printf("9. Se deconnecter\n");
        printf("Choix : "); scanf("%d", &choix);

        if (choix == 1) sousMenuUtilisateurs();
        else if (choix == 2) sousMenuAuteurs();
        else if (choix == 3) sousMenuCategories();
        else if (choix == 4) sousMenuLivres();
        else if (choix == 5) {
            printf("\n--- EMPRUNTS EN COURS ---\n");
            /* Lecture directe de BORROWS.dat pour l'admin */
            FILE *f = fopen("DATABASE/BORROWS.dat", "rb");
            if (f != NULL) {
                Emprunt e;
                int nb = 0;
                while (fread(&e, sizeof(Emprunt), 1, f) == 1) {
                    if (e.etat != EMPRUNT_RETOURNE) {
                        printf("[%d] %s - User %d - Livre %d - Retour prevu : %s - %s\n",
                            e.id, e.numeroEmprunt, e.idUtilisateur, e.idLivre,
                            e.datePrevueRetour,
                            e.etat == EMPRUNT_EN_COURS ? "En cours" : "En retard");
                        nb++;
                    }
                }
                if (nb == 0) printf("Aucun emprunt en cours.\n");
                fclose(f);
            }
        }
        else if (choix == 6) {
            printf("\n--- TOUTES LES PENALITES ---\n");
            FILE *f = fopen("DATABASE/PENALTIES.dat", "rb");
            if (f != NULL) {
                Penalite p;
                int nb = 0;
                while (fread(&p, sizeof(Penalite), 1, f) == 1) {
                    printf("[%d] User %d - %d jours - %.0f FCFA - %s\n",
                        p.id, p.idUtilisateur, p.joursRetard, p.montant, p.date);
                    nb++;
                }
                if (nb == 0) printf("Aucune penalite.\n");
                fclose(f);
            }
        }
        else if (choix == 7) {
            char date[11];
            printf("Date (JJ/MM/AAAA) : "); scanf("%s", date);
            if (genererRapportJournalier(date) == 0)
                printf("Rapport genere dans REPORTS/DAILY/\n");
            else
                printf("Erreur lors de la generation.\n");
        }
        else if (choix == 8) {
            char nouveau[50];
            printf("Nouveau mot de passe : "); scanf("%s", nouveau);
            changerMotDePasse(session.id, nouveau);
            printf("Mot de passe change.\n");
        }
    }
}

/* ===== MENU UTILISATEUR ===== */
void afficherMenuUtilisateur(Utilisateur session) {
    int choix = 0;
    while (choix != 8) {
        printf("\n--- MENU LECTEUR (%s %s) ---\n", session.prenom, session.nom);
        printf("1. Rechercher un livre\n");
        printf("2. Emprunter un livre\n");
        printf("3. Retourner un livre\n");
        printf("4. Reserver un livre\n");
        printf("5. Mes emprunts en cours\n");
        printf("6. Mes penalites\n");
        printf("7. Changer mon mot de passe\n");
        printf("8. Se deconnecter\n");
        printf("Choix : "); scanf("%d", &choix);

        if (choix == 1) {
            char motCle[150];
            printf("Titre a chercher : "); getchar();
            fgets(motCle, 150, stdin);
            motCle[strcspn(motCle, "\n")] = 0;

            Livre resultats[50];
            int nb, i;
            rechercherLivresParTitre(motCle, resultats, 50, &nb);
            if (nb == 0) printf("Aucun livre trouve.\n");
            for (i = 0; i < nb; i++) {
                printf("  [%d] %s (ISBN: %s) - %d dispo\n",
                    resultats[i].id, resultats[i].titre,
                    resultats[i].isbn, resultats[i].nombreExemplairesDisponibles);
            }
        }
        else if (choix == 2) {
            int idLivre;
            printf("ID du livre : "); scanf("%d", &idLivre);
            Emprunt emprunt;
            int r = creerEmprunt(session.id, idLivre, &emprunt);
            if (r == 0) {
                printf("Emprunt reussi ! Numero : %s\n", emprunt.numeroEmprunt);
                printf("Retour prevu : %s\n", emprunt.datePrevueRetour);
            } else if (r == -2) printf("Votre compte est bloque.\n");
            else if (r == -3) printf("Livre indisponible. Pensez a le reserver.\n");
            else if (r == -4) printf("Vous avez deja 3 emprunts en cours (max).\n");
            else printf("Erreur : utilisateur ou livre introuvable.\n");
        }
        else if (choix == 3) {
            int idEmprunt;
            printf("ID de l'emprunt : "); scanf("%d", &idEmprunt);
            Retour retour;
            int r = creerRetour(idEmprunt, &retour);
            if (r == 0) {
                printf("Retour enregistre.\n");
                if (retour.joursRetard > 0)
                    printf("Retard : %d jour(s) - Penalite : %.0f FCFA\n",
                        retour.joursRetard, retour.montantPenalite);
                else
                    printf("Retour a temps, aucune penalite.\n");
            } else if (r == -2) printf("Cet emprunt a deja ete retourne.\n");
            else printf("Emprunt introuvable.\n");
        }
        else if (choix == 4) {
            int idLivre;
            printf("ID du livre : "); scanf("%d", &idLivre);
            Reservation resa;
            int r = creerReservation(session.id, idLivre, &resa);
            if (r == 0) printf("Reservation enregistree !\n");
            else printf("Ce livre est encore disponible, empruntez-le.\n");
        }
        else if (choix == 5) {
            printf("\n--- MES EMPRUNTS ---\n");
            Emprunt liste[20];
            int nb, i;
            listerEmpruntsActifsParUtilisateur(session.id, liste, 20, &nb);
            if (nb == 0) printf("Aucun emprunt en cours.\n");
            for (i = 0; i < nb; i++) {
                printf("  [%d] %s - Livre %d - Retour prevu : %s - %s\n",
                    liste[i].id, liste[i].numeroEmprunt, liste[i].idLivre,
                    liste[i].datePrevueRetour,
                    liste[i].etat == EMPRUNT_EN_COURS ? "En cours" : "En retard");
            }
        }
        else if (choix == 6) {
            printf("\n--- MES PENALITES ---\n");
            Penalite liste[20];
            int nb, i;
            listerPenalitesParUtilisateur(session.id, liste, 20, &nb);
            if (nb == 0) printf("Aucune penalite.\n");
            for (i = 0; i < nb; i++) {
                printf("  [%d] %d jour(s) - %.0f FCFA - %s\n",
                    liste[i].id, liste[i].joursRetard,
                    liste[i].montant, liste[i].date);
            }
            if (nb > 0)
                printf("  Total : %.0f FCFA\n", getTotalPenalitesMontant(session.id));
        }
        else if (choix == 7) {
            char nouveau[50];
            printf("Nouveau mot de passe : "); scanf("%s", nouveau);
            changerMotDePasse(session.id, nouveau);
            printf("Mot de passe change.\n");
        }
    }
}