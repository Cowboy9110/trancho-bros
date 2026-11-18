#ifndef PLUGINS_H
#define PLUGINS_H

#define MAX_TYPES_ENNEMIS 10
#define MAX_SKINS 8
#define TAILLE_NOM 32

typedef struct {
    char nom[TAILLE_NOM];
    int pv;
    float vitesse;
    int largeur, hauteur;
    int points;
    int r, g, b;
} TypeEnnemi;

typedef struct {
    char nom[TAILLE_NOM];
    int r, g, b;
} Skin;

int Plugins_chargerEnnemis(const char *chemin, TypeEnnemi *types, int max);
int Plugins_chargerSkins(const char *chemin, Skin *skins, int max);

#endif
