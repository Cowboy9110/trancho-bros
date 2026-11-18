#include "editeur.h"
#include "graphique.h"

int main(void) {
    static Jeu jeu;
    static Editeur editeur;

    if (Graphique_init("Trancho Bros - projet C ESGI") != 0) return 1;
    Jeu_init(&jeu);
    Editeur_init(&editeur, CHEMIN_EDITION);

    while (!Entree_quitter() && !jeu.quitter) {
        float dt = Graphique_dt();
        Entree_maj();

        if (jeu.etat == ETAT_EDITEUR) Editeur_update(&editeur, &jeu, dt);
        else                          Jeu_update(&jeu, dt);

        Graphique_debut(CIEL);
        if (jeu.etat == ETAT_EDITEUR) Editeur_draw(&editeur);
        else                          Jeu_draw(&jeu);
        Graphique_fin();
    }

    Jeu_free(&jeu);
    Graphique_quitter();
    return 0;
}
