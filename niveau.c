#include "niveau.h"
#include <stdio.h>

void Niveau_vider(Niveau *n) {
    int l, c;
    for (l = 0; l < NIVEAU_HAUTEUR; l++)
        for (c = 0; c < NIVEAU_LARGEUR; c++)
            n->tuiles[l][c] = TUILE_VIDE;
    n->largeur  = NIVEAU_LARGEUR;
    n->hauteur  = NIVEAU_HAUTEUR;
    n->departX  = TAILLE_TUILE;
    n->departY  = TAILLE_TUILE;
}

int Niveau_estSolide(const Niveau *n, int col, int lig) {
    char t;
    if (lig < 0 || lig >= NIVEAU_HAUTEUR) return 0;
    if (col < 0 || col >= NIVEAU_LARGEUR) return 1;
    t = n->tuiles[lig][col];

    return t == TUILE_SOL || t == TUILE_BRIQUE || t == TUILE_BLOC;
}

char Niveau_tuile(const Niveau *n, int col, int lig) {
    if (lig < 0 || lig >= NIVEAU_HAUTEUR || col < 0 || col >= NIVEAU_LARGEUR)
        return TUILE_VIDE;
    return n->tuiles[lig][col];
}

void Niveau_poser(Niveau *n, int col, int lig, char tuile) {
    if (lig < 0 || lig >= NIVEAU_HAUTEUR || col < 0 || col >= NIVEAU_LARGEUR) return;
    n->tuiles[lig][col] = tuile;
}

int Niveau_charger(Niveau *n, const char *chemin) {
    FILE *f = fopen(chemin, "r");
    char ligne[512];
    int l = 0, c, largeur = 1;

    Niveau_vider(n);
    if (!f) {
        fprintf(stderr, "Niveau_charger : impossible d'ouvrir \"%s\".\n", chemin);
        return 1;
    }

    while (l < NIVEAU_HAUTEUR && fgets(ligne, sizeof(ligne), f)) {
        for (c = 0; c < NIVEAU_LARGEUR && ligne[c] != '\n' && ligne[c] != '\r' && ligne[c] != '\0'; c++)
            n->tuiles[l][c] = ligne[c];
        if (c > largeur) largeur = c;
        l++;
    }
    fclose(f);
    n->largeur = largeur;
    n->hauteur = l > 0 ? l : 1;

    for (l = 0; l < n->hauteur; l++)
        for (c = 0; c < n->largeur; c++) {
            if (n->tuiles[l][c] == TUILE_DEPART) {
                n->departX = (float)(c * TAILLE_TUILE);

                n->departY = (float)((l + 1) * TAILLE_TUILE - HAUTEUR_JOUEUR);
            }
        }
    return 0;
}

int Niveau_sauvegarder(const Niveau *n, const char *chemin) {
    FILE *f = fopen(chemin, "w");
    int l, c;
    if (!f) {
        fprintf(stderr, "Niveau_sauvegarder : impossible de creer \"%s\".\n", chemin);
        return 1;
    }
    for (l = 0; l < n->hauteur; l++) {
        for (c = 0; c < n->largeur; c++)
            fputc(n->tuiles[l][c], f);
        fputc('\n', f);
    }
    fclose(f);
    return 0;
}
