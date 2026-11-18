#include "jeu.h"
#include "graphique.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

float Jeu_calculerCamera(float xCentre, int largeurNiveau) {
    float cible = xCentre - LARGEUR_ECRAN / 2;
    float max = (float)(largeurNiveau * TAILLE_TUILE - LARGEUR_ECRAN);
    if (cible > max) cible = max;
    if (cible < 0) cible = 0;
    return cible;
}

void Jeu_init(Jeu *jeu) {
    TypeEnnemi goomba = { "Goomba", 1, 70, 28, 28, 200, 150, 80, 20 };
    Skin mario = { "Mario", 220, 50, 40 };

    jeu->etat = ETAT_MENU;
    jeu->quitter = 0;
    jeu->skin = 0;
    jeu->cameraX = 0;

    strcpy(jeu->cheminNiveau, "niveaux/niveau1.txt");

    if (Partie_init(&jeu->partie) != 0) exit(EXIT_FAILURE);
    jeu->partie.nbTypes = Plugins_chargerEnnemis("plugins/ennemis.txt", jeu->partie.types, MAX_TYPES_ENNEMIS);
    if (jeu->partie.nbTypes == 0) {
        jeu->partie.types[0] = goomba;
        jeu->partie.nbTypes = 1;
    }
    jeu->nbSkins = Plugins_chargerSkins("plugins/skins.txt", jeu->skins, MAX_SKINS);
    if (jeu->nbSkins == 0) {
        jeu->skins[0] = mario;
        jeu->nbSkins = 1;
    }
}

void Jeu_free(Jeu *jeu) {
    Partie_free(&jeu->partie);
}

void Jeu_nouvellePartie(Jeu *jeu, const char *chemin) {
    if (Partie_charger(&jeu->partie, chemin) != 0) return;

    if (strcmp(chemin, jeu->cheminNiveau) != 0 && strlen(chemin) < sizeof(jeu->cheminNiveau))
        strcpy(jeu->cheminNiveau, chemin);
    jeu->cameraX = 0;
    jeu->etat = ETAT_JEU;
}

void Jeu_update(Jeu *jeu, float dt) {
    Partie *p = &jeu->partie;
    Touches t;
    Coup coup;

    switch (jeu->etat) {
    case ETAT_MENU:
        if (Entree_pressee(TOUCHE_ENTREE)) Jeu_nouvellePartie(jeu, jeu->cheminNiveau);
        if (Entree_pressee(TOUCHE_E))      jeu->etat = ETAT_EDITEUR;

        if (Entree_pressee(TOUCHE_TAB))    jeu->skin = (jeu->skin + 1) % jeu->nbSkins;
        if (Entree_pressee(TOUCHE_ECHAP))  jeu->quitter = 1;
        break;

    case ETAT_JEU:

        t.gauche = Entree_enfoncee(TOUCHE_GAUCHE) || Entree_enfoncee(TOUCHE_Q);
        t.droite = Entree_enfoncee(TOUCHE_DROITE) || Entree_enfoncee(TOUCHE_D);
        t.saut   = Entree_enfoncee(TOUCHE_ESPACE) || Entree_enfoncee(TOUCHE_HAUT) || Entree_enfoncee(TOUCHE_Z);
        t.courir = Entree_enfoncee(TOUCHE_SHIFT);
        coup = Partie_update(p, &t, dt);
        if (coup == COUP_PERDU_VIE) {
            if (p->joueur.vies <= 0) {
                jeu->etat = ETAT_GAMEOVER;
            } else {
                Joueur_reapparaitre(&p->joueur);
                Partie_reinitialiserEnnemis(p);
            }
        }

        jeu->cameraX = Jeu_calculerCamera(p->joueur.entite.x + p->joueur.entite.largeur / 2, p->niveau.largeur);
        if (Entree_pressee(TOUCHE_ECHAP)) jeu->etat = ETAT_MENU;
        break;

    case ETAT_GAMEOVER:
        if (Entree_pressee(TOUCHE_ENTREE)) Jeu_nouvellePartie(jeu, jeu->cheminNiveau);
        if (Entree_pressee(TOUCHE_ECHAP))  jeu->etat = ETAT_MENU;
        break;

    default:
        break;
    }
}

static void dessinerTuile(char t, int x, int y) {
    if (t == TUILE_VIDE) return;
    if (t == TUILE_SOL || t == TUILE_BRIQUE || t == TUILE_BLOC) {
        Couleur couleur = GRIS;
        if (t == TUILE_SOL)         couleur = Graphique_rgb(139, 90, 43);
        else if (t == TUILE_BRIQUE) couleur = Graphique_rgb(200, 90, 40);
        Graphique_rect(x, y, TAILLE_TUILE, TAILLE_TUILE, couleur);
        Graphique_contour(x, y, TAILLE_TUILE, TAILLE_TUILE, NOIR);
    } else {
        char lettre[2] = { t, '\0' };
        Graphique_texte(lettre, x + 10, y + 6, 20, NOIR);
    }
}

void Jeu_dessinerNiveau(const Niveau *n, float cameraX) {
    int c0 = (int)(cameraX / TAILLE_TUILE);
    int c1 = c0 + LARGEUR_ECRAN / TAILLE_TUILE + 1;
    int l, c;
    if (c1 >= NIVEAU_LARGEUR) c1 = NIVEAU_LARGEUR - 1;

    for (l = 0; l < n->hauteur; l++)
        for (c = c0; c <= c1; c++)
            dessinerTuile(n->tuiles[l][c], (int)(c * TAILLE_TUILE - cameraX), l * TAILLE_TUILE);
}

static void dessinerPartie(const Jeu *jeu) {
    const Partie *p = &jeu->partie;
    const Joueur *j = &p->joueur;
    long i;
    int x;

    Jeu_dessinerNiveau(&p->niveau, jeu->cameraX);

    for (i = 0; i < p->pieces.taille; i++) {
        const Piece *pc = Liste_element(&p->pieces, i);
        if (pc->prise) continue;
        x = (int)(pc->x - jeu->cameraX);
        Graphique_cercle(x, (int)pc->y, 8, OR);
        Graphique_cercle(x, (int)pc->y, 5, JAUNE);
    }

    for (i = 0; i < p->ennemis.taille; i++) {
        const Ennemi *e = Liste_element(&p->ennemis, i);
        const TypeEnnemi *type = &p->types[e->type];
        if (!e->vivant) continue;
        x = (int)(e->entite.x - jeu->cameraX);
        Graphique_rect(x, (int)e->entite.y, e->entite.largeur, e->entite.hauteur,
                       Graphique_rgb(type->r, type->g, type->b));
    }

    {
        const Skin *s = &jeu->skins[jeu->skin];
        Graphique_rect((int)(j->entite.x - jeu->cameraX), (int)j->entite.y,
                       j->entite.largeur, j->entite.hauteur, Graphique_rgb(s->r, s->g, s->b));
    }
}

static void dessinerHUD(const Jeu *jeu) {
    const Joueur *j = &jeu->partie.joueur;
    char texte[64];
    Graphique_rect(0, 0, LARGEUR_ECRAN, 30, NOIR);

    snprintf(texte, sizeof(texte), "SCORE %06d", j->score);
    Graphique_texte(texte, 12, 6, 20, BLANC);
    snprintf(texte, sizeof(texte), "PIECES x%02d", j->pieces);
    Graphique_texte(texte, 220, 6, 20, OR);
    snprintf(texte, sizeof(texte), "VIES x%d", j->vies);
    Graphique_texte(texte, 420, 6, 20, ROUGE);
}

static void dessinerTexteCentre(const char *texte, int y, int taille, Couleur couleur) {
    Graphique_texte(texte, (LARGEUR_ECRAN - Graphique_largeurTexte(texte, taille)) / 2, y, taille, couleur);
}

static void dessinerFin(const Jeu *jeu) {
    char texte[64];
    Couleur voile = { 0, 0, 0, 204 };
    Graphique_rect(0, 0, LARGEUR_ECRAN, HAUTEUR_ECRAN, voile);
    dessinerTexteCentre("GAME OVER", 220, 50, ROUGE);
    snprintf(texte, sizeof(texte), "Score : %d   Pieces : %d", jeu->partie.joueur.score, jeu->partie.joueur.pieces);
    dessinerTexteCentre(texte, 300, 24, BLANC);
    dessinerTexteCentre("ENTREE : rejouer   ECHAP : menu", 360, 22, BLANC);
}

void Jeu_draw(const Jeu *jeu) {
    char texte[64];
    if (jeu->etat == ETAT_MENU) {
        dessinerTexteCentre("TRANCHO BROS", 140, 60, ROUGE);
        dessinerTexteCentre("ENTREE : jouer", 280, 24, NOIR);
        dessinerTexteCentre("E : editeur de niveaux", 320, 24, NOIR);
        snprintf(texte, sizeof(texte), "TAB : changer de skin (%s)", jeu->skins[jeu->skin].nom);
        dessinerTexteCentre(texte, 360, 24, NOIR);
        dessinerTexteCentre("ECHAP : quitter", 400, 24, NOIR);
        return;
    }
    dessinerPartie(jeu);
    dessinerHUD(jeu);

    if (jeu->etat == ETAT_GAMEOVER) dessinerFin(jeu);
}
