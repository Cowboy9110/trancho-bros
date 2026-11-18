#ifndef PARTIE_H
#define PARTIE_H

#include "config.h"
#include "liste.h"
#include "niveau.h"
#include "plugins.h"

typedef struct {
    float x, y;
    float vx, vy;
    int largeur, hauteur;
    int auSol;
    int toucheMur;
} Entite;

typedef struct { int gauche, droite, saut, courir; } Touches;

typedef struct {
    Entite entite;
    float x0, y0;
    int vies, score, pieces;
} Joueur;

typedef struct {
    Entite entite;
    float x0, y0;
    int type;
    int pv, vivant;
    int direction;
} Ennemi;

typedef struct {
    float x, y;
    int prise;
} Piece;

typedef struct {
    Niveau niveau;
    Joueur joueur;
    Liste ennemis;
    Liste pieces;
    TypeEnnemi types[MAX_TYPES_ENNEMIS];
    int nbTypes;
} Partie;

typedef enum { COUP_RIEN, COUP_PERDU_VIE } Coup;

void Entite_init(Entite *e, float x, float y, int largeur, int hauteur);
void Entite_deplacer(const Niveau *n, Entite *e, float dt);
int  Entite_touche(const Entite *a, float x, float y, int l, int h);

void Joueur_init(Joueur *j, float x, float y);
void Joueur_reapparaitre(Joueur *j);
void Joueur_update(const Niveau *n, Joueur *j, const Touches *t, float dt);

int  Partie_init(Partie *p);
void Partie_free(Partie *p);

int  Partie_charger(Partie *p, const char *chemin);

Coup Partie_update(Partie *p, const Touches *t, float dt);
void Partie_reinitialiserEnnemis(Partie *p);

#endif
