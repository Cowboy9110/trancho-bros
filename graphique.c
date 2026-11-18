#include "graphique.h"
#include "config.h"
#include <SDL.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static SDL_Window   *fenetre = NULL;
static SDL_Renderer *rendu = NULL;
static int quitter = 0;

typedef struct { char c; unsigned char lignes[7]; } Glyphe;

static const Glyphe POLICE[] = {
    {' ', {0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
    {'A', {0x0E,0x11,0x11,0x1F,0x11,0x11,0x11}}, {'B', {0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E}},
    {'C', {0x0E,0x11,0x10,0x10,0x10,0x11,0x0E}}, {'D', {0x1E,0x11,0x11,0x11,0x11,0x11,0x1E}},
    {'E', {0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F}}, {'F', {0x1F,0x10,0x10,0x1E,0x10,0x10,0x10}},
    {'G', {0x0E,0x11,0x10,0x17,0x11,0x11,0x0F}}, {'H', {0x11,0x11,0x11,0x1F,0x11,0x11,0x11}},
    {'I', {0x0E,0x04,0x04,0x04,0x04,0x04,0x0E}}, {'J', {0x07,0x02,0x02,0x02,0x02,0x12,0x0C}},
    {'K', {0x11,0x12,0x14,0x18,0x14,0x12,0x11}}, {'L', {0x10,0x10,0x10,0x10,0x10,0x10,0x1F}},
    {'M', {0x11,0x1B,0x15,0x15,0x11,0x11,0x11}}, {'N', {0x11,0x19,0x15,0x13,0x11,0x11,0x11}},
    {'O', {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E}}, {'P', {0x1E,0x11,0x11,0x1E,0x10,0x10,0x10}},
    {'Q', {0x0E,0x11,0x11,0x11,0x15,0x12,0x0D}}, {'R', {0x1E,0x11,0x11,0x1E,0x14,0x12,0x11}},
    {'S', {0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E}}, {'T', {0x1F,0x04,0x04,0x04,0x04,0x04,0x04}},
    {'U', {0x11,0x11,0x11,0x11,0x11,0x11,0x0E}}, {'V', {0x11,0x11,0x11,0x11,0x11,0x0A,0x04}},
    {'W', {0x11,0x11,0x11,0x15,0x15,0x1B,0x11}}, {'X', {0x11,0x11,0x0A,0x04,0x0A,0x11,0x11}},
    {'Y', {0x11,0x11,0x0A,0x04,0x04,0x04,0x04}}, {'Z', {0x1F,0x01,0x02,0x04,0x08,0x10,0x1F}},
    {'0', {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E}}, {'1', {0x04,0x0C,0x04,0x04,0x04,0x04,0x0E}},
    {'2', {0x0E,0x11,0x01,0x02,0x04,0x08,0x1F}}, {'3', {0x1F,0x02,0x04,0x02,0x01,0x11,0x0E}},
    {'4', {0x02,0x06,0x0A,0x12,0x1F,0x02,0x02}}, {'5', {0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E}},
    {'6', {0x06,0x08,0x10,0x1E,0x11,0x11,0x0E}}, {'7', {0x1F,0x01,0x02,0x04,0x08,0x08,0x08}},
    {'8', {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E}}, {'9', {0x0E,0x11,0x11,0x0F,0x01,0x02,0x0C}},
    {':', {0x00,0x0C,0x0C,0x00,0x0C,0x0C,0x00}}, {'.', {0x00,0x00,0x00,0x00,0x00,0x0C,0x0C}},
    {',', {0x00,0x00,0x00,0x00,0x0C,0x04,0x08}}, {'-', {0x00,0x00,0x00,0x1F,0x00,0x00,0x00}},
    {'+', {0x00,0x04,0x04,0x1F,0x04,0x04,0x00}}, {'!', {0x04,0x04,0x04,0x04,0x04,0x00,0x04}},
    {'?', {0x0E,0x11,0x01,0x02,0x04,0x00,0x04}}, {'/', {0x01,0x01,0x02,0x04,0x08,0x10,0x10}},
    {'(', {0x02,0x04,0x08,0x08,0x08,0x04,0x02}}, {')', {0x08,0x04,0x02,0x02,0x02,0x04,0x08}},
    {'|', {0x04,0x04,0x04,0x04,0x04,0x04,0x04}}, {'#', {0x0A,0x0A,0x1F,0x0A,0x1F,0x0A,0x0A}}
};
#define NB_GLYPHES ((int)(sizeof(POLICE) / sizeof(POLICE[0])))

static const Glyphe *trouverGlyphe(char c) {
    int i;
    if (c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');
    for (i = 0; i < NB_GLYPHES; i++)
        if (POLICE[i].c == c) return &POLICE[i];
    for (i = 0; i < NB_GLYPHES; i++)
        if (POLICE[i].c == '?') return &POLICE[i];
    return &POLICE[0];
}

static int echelle(int taille) {
    int e = taille / 9;
    return e < 1 ? 1 : e;
}

int Graphique_init(const char *titre) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init : %s\n", SDL_GetError());
        return 1;
    }
    fenetre = SDL_CreateWindow(titre, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               LARGEUR_ECRAN, HAUTEUR_ECRAN, 0);
    if (!fenetre) {
        fprintf(stderr, "SDL_CreateWindow : %s\n", SDL_GetError());
        return 1;
    }
    rendu = SDL_CreateRenderer(fenetre, -1, SDL_RENDERER_ACCELERATED);
    if (!rendu) rendu = SDL_CreateRenderer(fenetre, -1, SDL_RENDERER_SOFTWARE);
    if (!rendu) {
        fprintf(stderr, "SDL_CreateRenderer : %s\n", SDL_GetError());
        return 1;
    }
    SDL_SetRenderDrawBlendMode(rendu, SDL_BLENDMODE_BLEND);
    return 0;
}

void Graphique_quitter(void) {
    if (rendu) SDL_DestroyRenderer(rendu);
    if (fenetre) SDL_DestroyWindow(fenetre);
    SDL_Quit();
}

Couleur Graphique_rgb(int r, int g, int b) {
    Couleur c = { (unsigned char)r, (unsigned char)g, (unsigned char)b, 255 };
    return c;
}

float Graphique_dt(void) {
    static Uint32 dernier = 0;
    Uint32 maintenant = SDL_GetTicks();
    float dt = dernier ? (maintenant - dernier) / 1000.0f : 1.0f / 60.0f;
    dernier = maintenant;
    return dt > 1.0f / 30.0f ? 1.0f / 30.0f : dt;
}

static void choisirCouleur(Couleur c) {
    SDL_SetRenderDrawColor(rendu, c.r, c.g, c.b, c.a);
}

void Graphique_debut(Couleur fond) {
    choisirCouleur(fond);
    SDL_RenderClear(rendu);
}

void Graphique_fin(void) {
    static Uint32 debutImage = 0;
    Uint32 duree;
    SDL_RenderPresent(rendu);
    duree = SDL_GetTicks() - debutImage;
    if (duree < 16) SDL_Delay(16 - duree);
    debutImage = SDL_GetTicks();
}

void Graphique_rect(int x, int y, int l, int h, Couleur c) {
    SDL_Rect r = { x, y, l, h };
    choisirCouleur(c);
    SDL_RenderFillRect(rendu, &r);
}

void Graphique_contour(int x, int y, int l, int h, Couleur c) {
    SDL_Rect r = { x, y, l, h };
    choisirCouleur(c);
    SDL_RenderDrawRect(rendu, &r);
}

void Graphique_ligne(int x1, int y1, int x2, int y2, Couleur c) {
    choisirCouleur(c);
    SDL_RenderDrawLine(rendu, x1, y1, x2, y2);
}

void Graphique_cercle(int cx, int cy, int rayon, Couleur c) {
    int dy;
    for (dy = -rayon; dy <= rayon; dy++) {
        int dx = (int)sqrt((double)(rayon * rayon - dy * dy));
        Graphique_rect(cx - dx, cy + dy, 2 * dx + 1, 1, c);
    }
}

void Graphique_texte(const char *texte, int x, int y, int taille, Couleur c) {
    int e = echelle(taille);
    int i, ligne, col;
    for (i = 0; texte[i] != '\0'; i++) {
        const Glyphe *g = trouverGlyphe(texte[i]);
        for (ligne = 0; ligne < 7; ligne++)
            for (col = 0; col < 5; col++)
                if (g->lignes[ligne] & (1 << (4 - col)))
                    Graphique_rect(x + (i * 6 + col) * e, y + ligne * e, e, e, c);
    }
}

int Graphique_largeurTexte(const char *texte, int taille) {
    return (int)strlen(texte) * 6 * echelle(taille);
}

static Uint8 etatActuel[SDL_NUM_SCANCODES];
static Uint8 etatPrecedent[SDL_NUM_SCANCODES];
static int sourisX = 0, sourisY = 0;
static Uint32 boutons = 0;

static SDL_Scancode codeTouche(Touche t) {
    switch (t) {
    case TOUCHE_GAUCHE: return SDL_SCANCODE_LEFT;
    case TOUCHE_DROITE: return SDL_SCANCODE_RIGHT;
    case TOUCHE_HAUT:   return SDL_SCANCODE_UP;
    case TOUCHE_ESPACE: return SDL_SCANCODE_SPACE;
    case TOUCHE_ENTREE: return SDL_SCANCODE_RETURN;
    case TOUCHE_ECHAP:  return SDL_SCANCODE_ESCAPE;
    case TOUCHE_TAB:    return SDL_SCANCODE_TAB;
    case TOUCHE_Q: return SDL_GetScancodeFromKey(SDLK_q);
    case TOUCHE_D: return SDL_GetScancodeFromKey(SDLK_d);
    case TOUCHE_Z: return SDL_GetScancodeFromKey(SDLK_z);
    case TOUCHE_E: return SDL_GetScancodeFromKey(SDLK_e);
    case TOUCHE_S: return SDL_GetScancodeFromKey(SDLK_s);
    default:       return (SDL_Scancode)(SDL_SCANCODE_1 + (t - TOUCHE_1));
    }
}

static int etat(const Uint8 *tableau, Touche t) {
    if (t == TOUCHE_SHIFT)
        return tableau[SDL_SCANCODE_LSHIFT] || tableau[SDL_SCANCODE_RSHIFT];
    return tableau[codeTouche(t)];
}

void Entree_maj(void) {
    SDL_Event ev;
    int nb = 0;
    const Uint8 *clavier;
    memcpy(etatPrecedent, etatActuel, sizeof(etatActuel));
    while (SDL_PollEvent(&ev))
        if (ev.type == SDL_QUIT) quitter = 1;
    clavier = SDL_GetKeyboardState(&nb);
    if ((size_t)nb > sizeof(etatActuel)) nb = (int)sizeof(etatActuel);
    memcpy(etatActuel, clavier, (size_t)nb);
    boutons = SDL_GetMouseState(&sourisX, &sourisY);
}

int Entree_quitter(void)       { return quitter; }
int Entree_enfoncee(Touche t)  { return etat(etatActuel, t); }
int Entree_pressee(Touche t)   { return etat(etatActuel, t) && !etat(etatPrecedent, t); }
int Entree_sourisX(void)       { return sourisX; }
int Entree_sourisY(void)       { return sourisY; }
int Entree_clicGauche(void)    { return (boutons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0; }
int Entree_clicDroit(void)     { return (boutons & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0; }
