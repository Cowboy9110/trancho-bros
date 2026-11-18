#include <stdio.h>
#include <assert.h>
#include "../partie.h"

#define DT (1.0f / 60.0f)
#define TMP "tests/tmp_niveau.txt"

static Partie partie;

static void test_liste(void) {
    Liste l;
    long i;
    assert(Liste_creer(&l, 2, sizeof(int), NULL, NULL) == 0);
    for (i = 0; i < 25; i++) assert(Liste_ajouter(&l, &i) == 0);
    assert(l.taille == 25);
    assert(*(int *)Liste_element(&l, 24) == 24);
    assert(Liste_element(&l, 25) == NULL);
    Liste_afficher(&l);
    Liste_free(&l);
    assert(l.data == NULL);
    printf("OK liste generique\n");
}

static void niveau_sol(Niveau *n) {
    int c;
    Niveau_vider(n);
    for (c = 0; c < NIVEAU_LARGEUR; c++) {
        Niveau_poser(n, c, 18, TUILE_SOL);
        Niveau_poser(n, c, 19, TUILE_SOL);
    }
}

static void test_niveau_fichier(void) {
    Niveau n, n2;
    int l, c;
    Niveau_vider(&n);
    for (c = 0; c < 10; c++) Niveau_poser(&n, c, 18, TUILE_SOL);
    Niveau_poser(&n, 2, 17, TUILE_DEPART);
    Niveau_poser(&n, 5, 14, TUILE_BLOC);
    assert(Niveau_sauvegarder(&n, TMP) == 0);
    assert(Niveau_charger(&n2, TMP) == 0);
    assert(n2.largeur == n.largeur && n2.hauteur == n.hauteur);
    for (l = 0; l < n.hauteur; l++)
        for (c = 0; c < n.largeur; c++)
            assert(n2.tuiles[l][c] == n.tuiles[l][c]);
    assert(Niveau_estSolide(&n2, 5, 18) == 1);
    assert(Niveau_estSolide(&n2, 5, 14) == 1);
    assert(Niveau_estSolide(&n2, 5, 0) == 0);
    assert(Niveau_estSolide(&n2, -1, 10) == 1);
    assert(n2.departX == 2 * TAILLE_TUILE);
    assert(Niveau_charger(&n2, "fichier_inexistant.txt") == 1);
    printf("OK niveau : charger/sauvegarder/estSolide\n");
}

static void test_gravite_et_sol(void) {
    Niveau n;
    Joueur j;
    Touches t = {0, 0, 0, 0};
    long i;
    niveau_sol(&n);
    Joueur_init(&j, 3 * TAILLE_TUILE, 10 * TAILLE_TUILE);
    for (i = 0; i < 120; i++) Joueur_update(&n, &j, &t, DT);
    assert(j.entite.auSol == 1);
    assert(j.entite.y == 18 * TAILLE_TUILE - HAUTEUR_JOUEUR);
    printf("OK gravite : le joueur repose sur le sol\n");
}

static void test_mur(void) {
    Niveau n;
    Joueur j;
    Touches t = {0, 1, 0, 1};
    long i;
    niveau_sol(&n);
    for (i = 0; i < 18; i++) Niveau_poser(&n, 10, i, TUILE_BLOC);
    Joueur_init(&j, 5 * TAILLE_TUILE, 18 * TAILLE_TUILE - HAUTEUR_JOUEUR);
    for (i = 0; i < 120; i++) Joueur_update(&n, &j, &t, DT);
    assert(j.entite.x + j.entite.largeur <= 10 * TAILLE_TUILE + 1);
    printf("OK collision : le mur bloque le joueur\n");
}

static void test_saut(void) {
    Niveau n;
    Joueur j;
    Touches t = {0, 0, 1, 0};
    float ySol;
    int i, enlair = 0;
    niveau_sol(&n);
    Joueur_init(&j, 3 * TAILLE_TUILE, 18 * TAILLE_TUILE - HAUTEUR_JOUEUR);
    ySol = j.entite.y;
    for (i = 0; i < 90; i++) {
        Joueur_update(&n, &j, &t, DT);
        t.saut = (i < 30);
        if (j.entite.y < ySol - 10) enlair = 1;
    }
    assert(enlair == 1);
    assert(j.entite.auSol == 1 && j.entite.y == ySol);
    printf("OK saut : le joueur decolle puis retombe\n");
}

static void test_bord_gauche(void) {
    Niveau n;
    Joueur j;
    Touches t = {1, 0, 0, 1};
    int i;
    niveau_sol(&n);
    Joueur_init(&j, 2 * TAILLE_TUILE, 18 * TAILLE_TUILE - HAUTEUR_JOUEUR);
    for (i = 0; i < 120; i++) Joueur_update(&n, &j, &t, DT);
    assert(j.entite.x >= 0);
    printf("OK collision : le bord gauche du niveau bloque le joueur\n");
}

static void test_partie(void) {
    Partie *p = &partie;
    Touches t = {0, 0, 0, 0};
    Ennemi *e;
    long i;

    assert(Partie_init(p) == 0);
    p->nbTypes = Plugins_chargerEnnemis("plugins/ennemis.txt", p->types, MAX_TYPES_ENNEMIS);
    assert(p->nbTypes >= 3);
    assert(p->types[2].pv > 1);

    niveau_sol(&p->niveau);
    Niveau_poser(&p->niveau, 2, 17, TUILE_DEPART);
    Niveau_poser(&p->niveau, 10, 17, TUILE_PIECE);
    Niveau_poser(&p->niveau, 20, 17, TUILE_ENNEMI);
    Niveau_poser(&p->niveau, 30, 17, '2');
    assert(Niveau_sauvegarder(&p->niveau, TMP) == 0);
    assert(Partie_charger(p, TMP) == 0);
    assert(p->pieces.taille == 1 && p->ennemis.taille == 2);
    assert(p->joueur.x0 == p->niveau.departX);
    assert(Niveau_tuile(&p->niveau, 20, 17) == TUILE_VIDE);

    e = Liste_element(&p->ennemis, 0);
    p->joueur.entite.x = e->entite.x;
    p->joueur.entite.y = e->entite.y - HAUTEUR_JOUEUR - 2;
    p->joueur.entite.vy = 100;
    assert(Partie_update(p, &t, DT) == COUP_RIEN);
    assert(e->vivant == 0);
    assert(p->joueur.score == p->types[0].points);
    assert(p->joueur.entite.vy < 0);

    e = Liste_element(&p->ennemis, 1);
    p->joueur.entite.x = e->entite.x + 5;
    p->joueur.entite.y = e->entite.y;
    p->joueur.entite.vy = 0;
    assert(Partie_update(p, &t, DT) == COUP_PERDU_VIE);
    assert(p->joueur.vies == VIES_DEPART - 1);
    assert(e->vivant == 1);

    Joueur_init(&p->joueur, e->entite.x, e->entite.y - HAUTEUR_JOUEUR - 2);
    p->joueur.entite.vy = 100;
    assert(Partie_update(p, &t, DT) == COUP_RIEN);
    assert(e->pv == p->types[2].pv - 1 && e->vivant == 1);
    assert(Partie_update(p, &t, DT) == COUP_RIEN);
    assert(p->joueur.vies == VIES_DEPART);

    Joueur_init(&p->joueur, 10 * TAILLE_TUILE, 17 * TAILLE_TUILE);
    Partie_update(p, &t, DT);
    assert(p->joueur.pieces == 1 && p->joueur.score == 100);

    Joueur_init(&p->joueur, 3 * TAILLE_TUILE, HAUTEUR_ECRAN + 10);
    assert(Partie_update(p, &t, DT) == COUP_PERDU_VIE);
    assert(p->joueur.vies == VIES_DEPART - 1);

    Partie_reinitialiserEnnemis(p);
    for (i = 0; i < p->ennemis.taille; i++) assert(((Ennemi *)Liste_element(&p->ennemis, i))->vivant == 1);
    Partie_free(p);
    printf("OK partie : extraction, ecrasement, perte de vie, piece, trou\n");
}

static void test_simulation_niveau1(void) {
    Partie *p = &partie;
    Touches t = {0, 1, 0, 1};
    float xDepart;
    long i;

    assert(Partie_init(p) == 0);
    p->nbTypes = Plugins_chargerEnnemis("plugins/ennemis.txt", p->types, MAX_TYPES_ENNEMIS);
    assert(Partie_charger(p, "niveaux/niveau1.txt") == 0);
    assert(p->niveau.largeur == 120 && p->niveau.hauteur == 20);
    xDepart = p->joueur.entite.x;

    for (i = 0; i < 1200; i++) {
        t.saut = (i % 45) < 12;
        if (Partie_update(p, &t, DT) == COUP_PERDU_VIE) {
            if (p->joueur.vies <= 0) break;
            Joueur_reapparaitre(&p->joueur);
            Partie_reinitialiserEnnemis(p);
        }
    }
    assert(p->joueur.entite.x > xDepart + 200);
    Partie_free(p);
    printf("OK simulation niveau1 : avance de %d px, vies=%d, score=%d\n",
           (int)(p->joueur.entite.x - xDepart), p->joueur.vies, p->joueur.score);
}

int main(void) {
    test_liste();
    test_niveau_fichier();
    test_gravite_et_sol();
    test_mur();
    test_saut();
    test_bord_gauche();
    test_partie();
    test_simulation_niveau1();
    printf("Tous les tests passent.\n");
    return 0;
}
