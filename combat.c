// combat.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "combat.h"
#include "techniques.h"    // <-- pour utiliserTechnique

static int comparerVitesse(const void *a, const void *b) {
    const Combattant *A = a;
    const Combattant *B = b;
    return B->vitesse - A->vitesse;
}

void attaquer(Combattant *attaquant, Combattant *cible) {
    int degats = attaquant->attaque - cible->defense;
    if (degats > 0) {
        cible->pv_courants -= degats;
        printf("%s attaque %s et inflige %d dégâts !\n",
               attaquant->nom, cible->nom, degats);
    } else {
        printf("%s attaque %s mais l'attaque est trop faible !\n",
               attaquant->nom, cible->nom);
    }
}

void organiserTours(Equipe *e1, Equipe *e2, Combattant ordre[], int *n) {
    int idx = 0;
    for (int i = 0; i < e1->taille; i++) ordre[idx++] = e1->combattants[i];
    for (int i = 0; i < e2->taille; i++) ordre[idx++] = e2->combattants[i];
    *n = idx;
    qsort(ordre, *n, sizeof(Combattant), comparerVitesse);
}

void majEffetsActifs(Combattant *c) {
    for (int i = 0; i < MAX_EFFETS; i++) {
        Effet *e = &c->effets[i];
        if (e->tours_restants > 0) {
            e->tours_restants--;
            if (e->tours_restants == 0) {
                printf("L'effet %s sur %s a expiré.\n",
                       e->nom, c->nom);
                c->attaque -= e->valeur;
            }
        }
    }
}

int choisirCibleAleatoire(Equipe *adv) {
    return rand() % adv->taille;
}

void attaqueIA(Equipe *ia, Equipe *joueur, int niveau) {
    int idx = choisirCibleAleatoire(joueur);
    switch (niveau) {
        case 1:
            attaquer(&ia->combattants[0], &joueur->combattants[idx]);
            break;
        case 2: {
            int minPv = joueur->combattants[0].pv_courants;
            for (int i = 1; i < joueur->taille; i++) {
                int pv = joueur->combattants[i].pv_courants;
                if (pv > 0 && pv < minPv) {
                    minPv = pv;
                    idx = i;
                }
            }
            attaquer(&ia->combattants[0], &joueur->combattants[idx]);
            break;
        }
        case 3: {
            Technique *tech = ia->combattants[0].techniques;
            if (tech->current_cd == 0 && rand() % 2 == 0) {
                utiliserTechnique(&ia->combattants[0],
                                  &joueur->combattants[idx],
                                  tech);
            } else {
                attaquer(&ia->combattants[0],
                         &joueur->combattants[idx]);
            }
            break;
        }
        default:
            printf("Niveau IA inconnu !\n");
    }
}

void afficherBarreVieAvecEffets(Combattant *c) {
    printf("%s [", c->nom);
    int pct = c->pv_courants * 100 / c->pv_max;
    if (pct > 50)
        printf("\033[1;32m");
    else if (pct > 20)
        printf("\033[1;33m");
    else
        printf("\033[1;31m");

    for (int i = 0; i < c->pv_courants / 5; i++)
        printf("#");
    printf("\033[0m] (%d/%d PV)\n",
           c->pv_courants, c->pv_max);

    printf("Effets :\n");
    for (int i = 0; i < MAX_EFFETS; i++) {
        if (c->effets[i].tours_restants > 0) {
            printf("- %s (%d tours restants)\n",
                   c->effets[i].nom,
                   c->effets[i].tours_restants);
        }
    }
}
