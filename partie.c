#include "partie.h"
#include <stdio.h>
#include <math.h>

void Entite_init(Entite *e, float x, float y, int largeur, int hauteur) {
    e->x = x;
    e->y = y;
    e->vx = 0;
    e->vy = 0;
    e->largeur = largeur;
    e->hauteur = hauteur;
    e->auSol = 0;
    e->toucheMur = 0;
}

static int tuileDe(float px) {
    return (int)floor(px / TAILLE_TUILE);
}

static int collisionTuiles(const Niveau *n, float x, float y, int largeur, int hauteur) {
    int c0 = tuileDe(x);
    int c1 = tuileDe(x + largeur - 0.01f);
    int l0 = tuileDe(y);
    int l1 = tuileDe(y + hauteur - 0.01f);
    int c, l;
    for (l = l0; l <= l1; l++)
        for (c = c0; c <= c1; c++)
            if (Niveau_estSolide(n, c, l)) return 1;
    return 0;
}

void Entite_deplacer(const Niveau *n, Entite *e, float dt) {
    e->toucheMur = 0;
    e->auSol = 0;
    if (e->vy > VITESSE_CHUTE_MAX) e->vy = VITESSE_CHUTE_MAX;

    e->x += e->vx * dt;
    if (collisionTuiles(n, e->x, e->y, e->largeur, e->hauteur)) {
        if (e->vx > 0)
            e->x = (float)(tuileDe(e->x + e->largeur) * TAILLE_TUILE - e->largeur) - 0.01f;
        else if (e->vx < 0)
            e->x = (float)((tuileDe(e->x) + 1) * TAILLE_TUILE) + 0.01f;
        e->vx = 0;
        e->toucheMur = 1;
    }

    e->y += e->vy * dt;
    if (collisionTuiles(n, e->x, e->y, e->largeur, e->hauteur)) {
        if (e->vy > 0) {
            e->y = (float)(tuileDe(e->y + e->hauteur) * TAILLE_TUILE - e->hauteur);
            e->auSol = 1;
        } else if (e->vy < 0) {
            e->y = (float)((tuileDe(e->y) + 1) * TAILLE_TUILE) + 0.01f;
        }
        e->vy = 0;
    }
}

int Entite_touche(const Entite *a, float x, float y, int largeur, int hauteur) {
    return a->x < x + largeur && a->x + a->largeur > x &&
           a->y < y + hauteur && a->y + a->hauteur > y;
}

void Joueur_init(Joueur *j, float x, float y) {
    Entite_init(&j->entite, x, y, LARGEUR_JOUEUR, HAUTEUR_JOUEUR);
    j->x0 = x;
    j->y0 = y;
    j->vies = VIES_DEPART;
    j->score = 0;
    j->pieces = 0;
}

void Joueur_reapparaitre(Joueur *j) {
    j->entite.x = j->x0;
    j->entite.y = j->y0;
    j->entite.vx = 0;
    j->entite.vy = 0;
}

void Joueur_update(const Niveau *n, Joueur *j, const Touches *t, float dt) {
    Entite *e = &j->entite;
    float vitesse = t->courir ? VITESSE_COURSE : VITESSE_MARCHE;

    if (t->gauche)      e->vx = -vitesse;
    else if (t->droite) e->vx = vitesse;
    else                e->vx = 0;

    if (t->saut && e->auSol) e->vy = -VITESSE_SAUT;

    e->vy += GRAVITE * dt;
    Entite_deplacer(n, e, dt);
}

static void Piece_afficher(void *element) {
    const Piece *pc = element;
    printf("(%.0f,%.0f)", pc->x, pc->y);
}

static int creerListes(Partie *p) {
    if (Liste_creer(&p->ennemis, 8, sizeof(Ennemi), NULL, NULL) != 0) return 1;
    if (Liste_creer(&p->pieces, 16, sizeof(Piece), NULL, Piece_afficher) != 0) return 1;
    return 0;
}

int Partie_init(Partie *p) {
    p->nbTypes = 0;
    return creerListes(p);
}

void Partie_free(Partie *p) {
    Liste_free(&p->ennemis);
    Liste_free(&p->pieces);
}

static int estEnnemi(char t) {
    return t == TUILE_ENNEMI || (t >= '0' && t <= '9');
}

static void extraireEntites(Partie *p) {
    Niveau *n = &p->niveau;
    int l, c;
    for (l = 0; l < n->hauteur; l++) {
        for (c = 0; c < n->largeur; c++) {
            char t = n->tuiles[l][c];
            if (t == TUILE_PIECE) {
                Piece pc;
                pc.x = (float)(c * TAILLE_TUILE + TAILLE_TUILE / 2);
                pc.y = (float)(l * TAILLE_TUILE + TAILLE_TUILE / 2);
                pc.prise = 0;
                Liste_ajouter(&p->pieces, &pc);
            } else if (estEnnemi(t)) {
                Ennemi e;

                int type = (t == TUILE_ENNEMI) ? 0 : t - '0';
                const TypeEnnemi *modele;
                if (type >= p->nbTypes) type = 0;
                modele = &p->types[type];

                Entite_init(&e.entite,
                            (float)(c * TAILLE_TUILE + (TAILLE_TUILE - modele->largeur) / 2),
                            (float)((l + 1) * TAILLE_TUILE - modele->hauteur),
                            modele->largeur, modele->hauteur);
                e.x0 = e.entite.x;
                e.y0 = e.entite.y;
                e.type = type;
                e.pv = modele->pv;
                e.vivant = 1;
                e.direction = -1;
                Liste_ajouter(&p->ennemis, &e);
            }

            if (t == TUILE_PIECE || t == TUILE_DEPART || estEnnemi(t))
                n->tuiles[l][c] = TUILE_VIDE;
        }
    }
}

int Partie_charger(Partie *p, const char *chemin) {
    if (Niveau_charger(&p->niveau, chemin) != 0) return 1;
    Joueur_init(&p->joueur, p->niveau.departX, p->niveau.departY);

    Liste_free(&p->ennemis);
    Liste_free(&p->pieces);
    if (creerListes(p) != 0) return 1;
    extraireEntites(p);
    return 0;
}

Coup Partie_update(Partie *p, const Touches *t, float dt) {
    Joueur *j = &p->joueur;
    long i;

    Joueur_update(&p->niveau, j, t, dt);

    for (i = 0; i < p->ennemis.taille; i++) {
        Ennemi *e = Liste_element(&p->ennemis, i);
        if (!e->vivant) continue;
        e->entite.vx = e->direction * p->types[e->type].vitesse;
        e->entite.vy += GRAVITE * dt;
        Entite_deplacer(&p->niveau, &e->entite, dt);
        if (e->entite.toucheMur) e->direction = -e->direction;
        if (e->entite.y > HAUTEUR_ECRAN) e->vivant = 0;
    }

    for (i = 0; i < p->pieces.taille; i++) {
        Piece *pc = Liste_element(&p->pieces, i);
        if (!pc->prise && Entite_touche(&j->entite, pc->x - 8, pc->y - 8, 16, 16)) {
            pc->prise = 1;
            j->pieces++;
            j->score += 100;
        }
    }

    for (i = 0; i < p->ennemis.taille; i++) {
        Ennemi *e = Liste_element(&p->ennemis, i);
        if (!e->vivant) continue;
        if (!Entite_touche(&j->entite, e->entite.x, e->entite.y, e->entite.largeur, e->entite.hauteur))
            continue;
        if (j->entite.y + j->entite.hauteur - e->entite.y < 20) {
            if (j->entite.vy > 0) {
                e->pv--;
                if (e->pv <= 0) {
                    e->vivant = 0;
                    j->score += p->types[e->type].points;
                }
                j->entite.vy = -REBOND_ENNEMI;
            }
        } else {
            j->vies--;
            return COUP_PERDU_VIE;
        }
    }

    if (j->entite.y > HAUTEUR_ECRAN) {
        j->vies--;
        return COUP_PERDU_VIE;
    }
    return COUP_RIEN;
}

void Partie_reinitialiserEnnemis(Partie *p) {
    long i;
    for (i = 0; i < p->ennemis.taille; i++) {
        Ennemi *e = Liste_element(&p->ennemis, i);
        e->entite.x = e->x0;
        e->entite.y = e->y0;
        e->entite.vx = 0;
        e->entite.vy = 0;
        e->pv = p->types[e->type].pv;
        e->vivant = 1;
        e->direction = -1;
    }
}
