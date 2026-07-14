#ifndef HISTORY_H
#define HISTORY_H

#include "borrows.h"
#include "returns.h"
#include "users.h"
#include "books.h"
#include "authors.h"

int creerDossiersProjet(void);
int journaliserOperation(const char *login, const char *operation);
int genererRecuEmprunt(const Emprunt *emprunt, const Utilisateur *utilisateur, const Livre *livre, const Auteur *auteur);
int genererRecuRetour(const Retour *retour, const Emprunt *emprunt, const Utilisateur *utilisateur, const Livre *livre);

#endif