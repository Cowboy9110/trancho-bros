# Trancho Bros — projet C (ESGI)

Jeu de plateforme 2D inspiré de Super Mario Bros, en **C** avec **SDL2**, pour **Ubuntu x64**.

## Compilation et lancement

```sh
sudo apt install build-essential libsdl2-dev
make            # compile ./mini-mario
./mini-mario    # à lancer depuis la racine du projet
make test       # tests de la logique (sans interface graphique)
```

## Contrôles

| Touche | Action |
|---|---|
| Flèches gauche/droite (ou Q/D) | marcher |
| Espace / Haut / Z | sauter |
| Shift | courir |
| Entrée | jouer / rejouer |
| E | éditeur de niveaux (depuis le menu) |
| Tab | changer de skin (menu) |
| Échap | menu / quitter |

**Éditeur** : 1-8 = choisir la tuile, clic gauche = poser, clic droit = effacer,
flèches = défiler, S = sauvegarder, Entrée = sauvegarder et tester, Échap = menu.
La carte est enregistrée dans `niveaux/edition.txt`.

## Règles

- Pièce `C` : +100 points.
- Sauter sur un ennemi l'écrase (les boss demandent plusieurs sauts).
  Le toucher de côté ou tomber dans un trou = -1 vie. 3 vies, puis Game Over.

## Fichiers

```
main.c              fenêtre + boucle principale
config.h            constantes (écran, tuiles, physique)
liste.h/.c          liste générique (tableau dynamique, pointeurs de fonctions)
niveau.h/.c         grille de tuiles + fichiers de niveau
plugins.h/.c        ennemis/boss/skins lus dans des fichiers
partie.h/.c         entités, physique, collisions, règles (sans interface graphique)
jeu.h/.c            états (menu/jeu/game over), dessin du niveau et du HUD
graphique.h/.c      SEUL module qui utilise SDL2 : fenêtre, dessin, police intégrée, clavier, souris
editeur.h/.c        éditeur de niveaux intégré
tests/test_logique.c
niveaux/niveau1.txt   1 caractère = 1 tuile
plugins/ennemis.txt   nom;pv;vitesse;largeur;hauteur;points;r;g;b
plugins/skins.txt     nom;r;g;b
```

Pour ajouter un ennemi, un boss ou un skin : ajouter une ligne dans le fichier plugin
(pas de recompilation). Dans une carte, `E` = ennemi type 0, `1`..`9` = types suivants.

## Tuiles

`.` air/trou · `#` sol · `B` brique · `M` bloc · `C` pièce ·
`E`,`1`,`2` ennemis (Goomba, Koopa, Boss) · `P` départ
