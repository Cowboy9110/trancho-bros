#ifndef GRAPHIQUE_H
#define GRAPHIQUE_H

typedef struct { unsigned char r, g, b, a; } Couleur;

#define NOIR       ((Couleur){0, 0, 0, 255})
#define BLANC       ((Couleur){245, 245, 245, 255})
#define ROUGE       ((Couleur){220, 40, 40, 255})
#define VERT        ((Couleur){40, 190, 70, 255})
#define VERT_FONCE  ((Couleur){0, 110, 40, 255})
#define OR          ((Couleur){255, 200, 0, 255})
#define JAUNE       ((Couleur){255, 240, 80, 255})
#define GRIS        ((Couleur){130, 130, 130, 255})
#define GRIS_CLAIR  ((Couleur){200, 200, 200, 255})
#define CIEL        ((Couleur){120, 190, 255, 255})

typedef enum {
    TOUCHE_GAUCHE, TOUCHE_DROITE, TOUCHE_HAUT, TOUCHE_ESPACE, TOUCHE_SHIFT,
    TOUCHE_ENTREE, TOUCHE_ECHAP, TOUCHE_TAB,
    TOUCHE_Q, TOUCHE_D, TOUCHE_Z, TOUCHE_E, TOUCHE_S,
    TOUCHE_1, TOUCHE_2, TOUCHE_3, TOUCHE_4,
    TOUCHE_5, TOUCHE_6, TOUCHE_7, TOUCHE_8
} Touche;

int   Graphique_init(const char *titre);
void  Graphique_quitter(void);
float Graphique_dt(void);
Couleur Graphique_rgb(int r, int g, int b);

void Graphique_debut(Couleur fond);
void Graphique_fin(void);
void Graphique_rect(int x, int y, int l, int h, Couleur c);
void Graphique_contour(int x, int y, int l, int h, Couleur c);
void Graphique_ligne(int x1, int y1, int x2, int y2, Couleur c);
void Graphique_cercle(int cx, int cy, int rayon, Couleur c);
void Graphique_texte(const char *texte, int x, int y, int taille, Couleur c);
int  Graphique_largeurTexte(const char *texte, int taille);

void Entree_maj(void);
int  Entree_quitter(void);
int  Entree_enfoncee(Touche t);
int  Entree_pressee(Touche t);
int  Entree_sourisX(void);
int  Entree_sourisY(void);
int  Entree_clicGauche(void);
int  Entree_clicDroit(void);

#endif
