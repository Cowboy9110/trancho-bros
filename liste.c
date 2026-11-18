#include "liste.h"
#include <stdio.h>
#include <string.h>

int Liste_creer(Liste *l, long capacite, size_t taille,
                void (*freeFn)(void *), void (*printFn)(void *)) {
    l->data = NULL;
    l->taille = 0;
    l->capacite = capacite;
    l->elementTaille = taille;
    l->elementFree = freeFn;
    l->elementPrint = printFn;
    if (capacite > 0) {
        l->data = malloc(taille * (size_t)capacite);
        if (!l->data) {
            fprintf(stderr, "Liste_creer : erreur allocation.\n");
            return 1;
        }
    }
    return 0;
}

void Liste_free(Liste *l) {
    long i;
    if (!l || !l->data) return;
    if (l->elementFree)
        for (i = 0; i < l->taille; i++)
            l->elementFree((unsigned char *)l->data + i * l->elementTaille);
    free(l->data);
    l->data = NULL;
    l->taille = 0;
    l->capacite = 0;
}

void Liste_afficher(const Liste *l) {
    long i;
    printf("[");
    for (i = 0; i < l->taille; i++) {
        if (i) printf(", ");
        if (l->elementPrint) l->elementPrint(Liste_element(l, i));
        else printf("?");
    }
    printf("]\n");
}

int Liste_ajouter(Liste *l, const void *element) {
    if (l->taille >= l->capacite) {
        long nouvelleCap = l->capacite * 2 + 10;

        void *tmp = realloc(l->data, (size_t)nouvelleCap * l->elementTaille);
        if (!tmp) {
            fprintf(stderr, "Liste_ajouter : erreur reallocation.\n");
            return 1;
        }
        l->data = tmp;
        l->capacite = nouvelleCap;
    }

    memcpy((unsigned char *)l->data + l->taille * l->elementTaille, element, l->elementTaille);
    l->taille++;
    return 0;
}

void *Liste_element(const Liste *l, long i) {
    if (!l || !l->data || i < 0 || i >= l->taille) return NULL;
    return (unsigned char *)l->data + i * l->elementTaille;
}
