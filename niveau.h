#ifndef NIVEAU_H
#define NIVEAU_H

#include "config.h"

typedef struct {
    char tuiles[NIVEAU_HAUTEUR][NIVEAU_LARGEUR];
    int largeur, hauteur;
    float departX, departY;
} Niveau;

void Niveau_vider(Niveau *n);
int  Niveau_estSolide(const Niveau *n, int col, int lig);
char Niveau_tuile(const Niveau *n, int col, int lig);
void Niveau_poser(Niveau *n, int col, int lig, char tuile);
int  Niveau_charger(Niveau *n, const char *chemin);
int  Niveau_sauvegarder(const Niveau *n, const char *chemin);

#endif
