CC      = gcc
CFLAGS  = -O2 -Wall -Wextra -Werror -std=c99 $(shell sdl2-config --cflags)
LIBS    = $(shell sdl2-config --libs) -lm
LOGIQUE = liste.c niveau.c plugins.c partie.c
JEU     = main.c jeu.c editeur.c graphique.c $(LOGIQUE)

mini-mario: $(JEU) $(wildcard *.h)
	$(CC) $(CFLAGS) -o $@ $(JEU) $(LIBS)

test: tests/test_logique.c $(LOGIQUE) $(wildcard *.h)
	$(CC) -O2 -Wall -Wextra -Werror -std=c99 -o test_logique tests/test_logique.c $(LOGIQUE) -lm
	./test_logique

clean:
	rm -f mini-mario test_logique tests/tmp_niveau.txt

.PHONY: test clean
