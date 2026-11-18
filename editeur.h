#ifndef EDITEUR_H
#define EDITEUR_H

#include "jeu.h"

typedef struct {
    Niveau niveau;
    char selection;
    float cameraX;
    const char *message;
    float messageTemps;
} Editeur;

void Editeur_init(Editeur *e, const char *chemin);
void Editeur_update(Editeur *e, Jeu *jeu, float dt);
void Editeur_draw(const Editeur *e);

#endif
