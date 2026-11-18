#ifndef JEU_H
#define JEU_H

#include "partie.h"

typedef enum { ETAT_MENU, ETAT_JEU, ETAT_EDITEUR, ETAT_GAMEOVER } EtatJeu;

typedef struct {
    EtatJeu etat;
    int quitter;
    Partie partie;
    Skin skins[MAX_SKINS];
    int nbSkins;
    int skin;
    float cameraX;
    char cheminNiveau[256];
} Jeu;

void Jeu_init(Jeu *jeu);
void Jeu_free(Jeu *jeu);
void Jeu_nouvellePartie(Jeu *jeu, const char *chemin);
void Jeu_update(Jeu *jeu, float dt);
void Jeu_draw(const Jeu *jeu);

float Jeu_calculerCamera(float xCentre, int largeurNiveau);
void  Jeu_dessinerNiveau(const Niveau *n, float cameraX);

#endif
