#ifndef BOOKS_H
#define BOOKS_H

typedef struct {
    int  id;
    char isbn[20];
    char titre[150];
    int  idAuteur;
    int  idCategorie;
    char editeur[100];
    int  anneePublication;
    char langue[30];
    int  nombrePages;
    int  nombreExemplairesTotal;
    int  nombreExemplairesDisponibles;
    char emplacement[50];
    char resume[500];
    char dateAjout[20];
} Livre;

int creerLivre(const Livre *donnees, Livre *nouveauLivre);
int trouverLivreParId(int id, Livre *resultat);
int trouverLivreParIsbn(const char *isbn, Livre *resultat);
int modifierLivre(int id, const Livre *nouvellesDonnees);
int decrementerExemplairesDisponibles(int idLivre);
int incrementerExemplairesDisponibles(int idLivre);
int supprimerLivre(int id);
int compterLivresParAuteur(int idAuteur);
int compterLivresParCategorie(int idCategorie);
int obtenirExemplairesDisponibles(int idLivre);
int listerLivres(Livre tableau[], int tailleMax, int *nombreTrouve);
int rechercherLivresParTitre(const char *motCle, Livre resultats[], int tailleMax, int *nombreTrouve);

#endif