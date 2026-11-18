#ifndef LISTE_H
#define LISTE_H

#include <stdlib.h>

typedef struct {
    void *data;
    long taille;
    long capacite;
    size_t elementTaille;
    void (*elementFree)(void *);
    void (*elementPrint)(void *);
} Liste;

int   Liste_creer(Liste *l, long capacite, size_t taille,
                  void (*freeFn)(void *), void (*printFn)(void *));
void  Liste_free(Liste *l);
void  Liste_afficher(const Liste *l);
int   Liste_ajouter(Liste *l, const void *element);
void *Liste_element(const Liste *l, long i);

#endif
