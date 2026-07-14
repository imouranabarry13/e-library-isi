#ifndef RESERVATIONS_H
#define RESERVATIONS_H

#include "utils.h"

typedef struct {
    int  id;
    int  idUtilisateur;
    int  idLivre;
    char dateReservation[20];
    EtatReservation etat;
} Reservation;

int creerReservation(int idUtilisateur, int idLivre, Reservation *nouvelleReservation);
int promouvoirPremiereReservation(int idLivre);
int annulerReservation(int idReservation);
int listerReservationsParLivre(int idLivre, Reservation tableau[], int tailleMax, int *nombreTrouve);
int listerReservationsParUtilisateur(int idUtilisateur, Reservation tableau[], int tailleMax, int *nombreTrouve);

#endif