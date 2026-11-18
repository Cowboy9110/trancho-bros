#include "editeur.h"
#include "graphique.h"
#include <stdio.h>

static const char PALETTE[] = { '#', 'B', 'M', 'C', 'E', '1', '2', 'P' };
#define NB_PALETTE 8

static int chargerNiveau(Editeur *e, const char *chemin) {
    int erreur = Niveau_charger(&e->niveau, chemin);
    e->niveau.largeur = NIVEAU_LARGEUR;
    e->niveau.hauteur = NIVEAU_HAUTEUR;
    return erreur;
}

void Editeur_init(Editeur *e, const char *chemin) {
    FILE *f = fopen(chemin, "r");
    if (f) {
        fclose(f);
        chargerNiveau(e, chemin);
    } else {
        Niveau_vider(&e->niveau);
    }
    e->selection = TUILE_SOL;
    e->cameraX = 0;
    e->message = "";
    e->messageTemps = 0;
}

static void message(Editeur *e, const char *texte) {
    e->message = texte;
    e->messageTemps = 3.0f;
}

void Editeur_update(Editeur *e, Jeu *jeu, float dt) {
    int col = (int)((Entree_sourisX() + e->cameraX) / TAILLE_TUILE);
    int lig = Entree_sourisY() / TAILLE_TUILE;
    float vitesse = 400.0f * dt;
    int i;

    if (Entree_enfoncee(TOUCHE_GAUCHE)) e->cameraX -= vitesse;
    if (Entree_enfoncee(TOUCHE_DROITE)) e->cameraX += vitesse;
    e->cameraX = Jeu_calculerCamera(e->cameraX + LARGEUR_ECRAN / 2, e->niveau.largeur);

    for (i = 0; i < NB_PALETTE; i++)
        if (Entree_pressee(TOUCHE_1 + i)) e->selection = PALETTE[i];

    if (Entree_sourisY() >= 30) {
        if (Entree_clicGauche()) Niveau_poser(&e->niveau, col, lig, e->selection);
        if (Entree_clicDroit())  Niveau_poser(&e->niveau, col, lig, TUILE_VIDE);
    }

    if (Entree_pressee(TOUCHE_S) && Niveau_sauvegarder(&e->niveau, CHEMIN_EDITION) == 0)
        message(e, "Carte sauvegardee");
    if (Entree_pressee(TOUCHE_ENTREE)) {
        Niveau_sauvegarder(&e->niveau, CHEMIN_EDITION);
        Jeu_nouvellePartie(jeu, CHEMIN_EDITION);
    }
    if (Entree_pressee(TOUCHE_ECHAP)) jeu->etat = ETAT_MENU;

    if (e->messageTemps > 0) e->messageTemps -= dt;
}

void Editeur_draw(const Editeur *e) {
    int i, c;
    char lettre[2] = { 0, 0 };
    Couleur grille = { 130, 130, 130, 80 };

    Jeu_dessinerNiveau(&e->niveau, e->cameraX);

    for (c = 0; c <= LARGEUR_ECRAN / TAILLE_TUILE + 1; c++) {
        int x = c * TAILLE_TUILE - (int)e->cameraX % TAILLE_TUILE;
        Graphique_ligne(x, 0, x, HAUTEUR_ECRAN, grille);
    }
    for (i = 0; i < NIVEAU_HAUTEUR; i++)
        Graphique_ligne(0, i * TAILLE_TUILE, LARGEUR_ECRAN, i * TAILLE_TUILE, grille);

    Graphique_rect(0, 0, LARGEUR_ECRAN, 30, NOIR);
    Graphique_texte("1-8: tuile | clic G: poser | clic D: effacer | fleches: defiler | S: sauver | ENTREE: tester",
                    8, 10, 9, BLANC);
    if (e->messageTemps > 0) Graphique_texte(e->message, 8, 40, 18, VERT_FONCE);

    for (i = 0; i < NB_PALETTE; i++) {
        Graphique_rect(12 + i * 40, HAUTEUR_ECRAN - 46, 32, 32, PALETTE[i] == e->selection ? JAUNE : GRIS_CLAIR);
        lettre[0] = PALETTE[i];
        Graphique_texte(lettre, 22 + i * 40, HAUTEUR_ECRAN - 40, 20, NOIR);
    }
}
