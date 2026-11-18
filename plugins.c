#include "plugins.h"
#include <stdio.h>

int Plugins_chargerEnnemis(const char *chemin, TypeEnnemi *types, int max) {
    FILE *f = fopen(chemin, "r");
    char ligne[256];
    int nb = 0;
    if (!f) {
        fprintf(stderr, "Plugins_chargerEnnemis : \"%s\" introuvable.\n", chemin);
        return 0;
    }
    while (nb < max && fgets(ligne, sizeof(ligne), f)) {
        TypeEnnemi *t = &types[nb];
        if (ligne[0] == '#') continue;

        if (sscanf(ligne, "%31[^;];%d;%f;%d;%d;%d;%d;%d;%d", t->nom, &t->pv, &t->vitesse,
                   &t->largeur, &t->hauteur, &t->points, &t->r, &t->g, &t->b) == 9)
            nb++;
    }
    fclose(f);
    return nb;
}

int Plugins_chargerSkins(const char *chemin, Skin *skins, int max) {
    FILE *f = fopen(chemin, "r");
    char ligne[256];
    int nb = 0;
    if (!f) {
        fprintf(stderr, "Plugins_chargerSkins : \"%s\" introuvable.\n", chemin);
        return 0;
    }
    while (nb < max && fgets(ligne, sizeof(ligne), f)) {
        Skin *s = &skins[nb];
        if (ligne[0] == '#') continue;
        if (sscanf(ligne, "%31[^;];%d;%d;%d", s->nom, &s->r, &s->g, &s->b) == 4)
            nb++;
    }
    fclose(f);
    return nb;
}
