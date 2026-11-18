#ifndef CONFIG_H
#define CONFIG_H

#define TAILLE_TUILE   32
#define NIVEAU_LARGEUR 120
#define NIVEAU_HAUTEUR 20
#define LARGEUR_ECRAN  960

#define HAUTEUR_ECRAN  (NIVEAU_HAUTEUR * TAILLE_TUILE)

enum {
    TUILE_VIDE     = '.',
    TUILE_SOL      = '#',
    TUILE_BRIQUE   = 'B',
    TUILE_BLOC     = 'M',
    TUILE_PIECE    = 'C',
    TUILE_ENNEMI   = 'E',
    TUILE_DEPART   = 'P'
};

#define LARGEUR_JOUEUR     24
#define HAUTEUR_JOUEUR     32
#define VIES_DEPART        3
#define GRAVITE            2000.0f
#define VITESSE_MARCHE     170.0f
#define VITESSE_COURSE     300.0f
#define VITESSE_SAUT       720.0f
#define VITESSE_CHUTE_MAX  800.0f
#define REBOND_ENNEMI      460.0f

#define CHEMIN_EDITION "niveaux/edition.txt"

#endif
