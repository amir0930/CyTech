// combat.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "combat.h"

#define C_RESET   "\033[0m"
#define C_RED     "\033[1;31m"
#define C_YELLOW  "\033[1;33m"
#define C_CYAN    "\033[1;36m"

// Compareur pour qsort : vitesse décroissante
static int comparerVitesse(const void *a, const void *b) {
    const Combattant *A = a, *B = b;
    return B->vitesse - A->vitesse;
}

// Attaque basique
void attaquer(Combattant *attaquant, Combattant *cible) {
    int degats = attaquant->attaque - cible->defense;
    if (degats > 0) {
        cible->pv_courants -= degats;
        if (cible->pv_courants < 0) cible->pv_courants = 0;
        printf("\n%s%s%s attaque %s%s%s et inflige %s%d%s degats !\n",
               C_CYAN, attaquant->nom, C_RESET,
               C_CYAN, cible->nom, C_RESET,
               C_RED, degats, C_RESET);
    } else {
        printf("\n%s%s%s attaque %s%s%s mais l'attaque est trop faible !\n",
               C_CYAN, attaquant->nom, C_RESET,
               C_CYAN, cible->nom, C_RESET);
    }
}

// Organise l'ordre de jeu
void organiserTours(Equipe *e1, Equipe *e2, Combattant ordre[], int *n) {
    int idx = 0;
    for (int i = 0; i < e1->taille; i++) ordre[idx++] = e1->combattants[i];
    for (int i = 0; i < e2->taille; i++) ordre[idx++] = e2->combattants[i];
    *n = idx;
    qsort(ordre, *n, sizeof(Combattant), comparerVitesse);
}

// Mise à jour des buffs/debuffs
void majEffetsActifs(Combattant *c) {
    for (int i = 0; i < MAX_EFFETS; i++) {
        if (c->effets[i].tours_restants > 0) {
            c->effets[i].tours_restants--;
            if (c->effets[i].tours_restants == 0) {
                printf("\n%sL'effet %s sur %s a expire.%s\n",
                       C_YELLOW, c->effets[i].nom, c->nom, C_RESET);
                c->attaque -= c->effets[i].valeur;
            }
        }
    }
}

// Technique spéciale
void utiliserTechnique(Combattant *attaquant, Combattant *cible, Technique *tech) {
    if (tech->current_cd > 0) {
        printf("\n%s%s%s ne peut pas encore utiliser %s (CD restante : %d tours)%s\n",
               C_CYAN, attaquant->nom, C_RESET,
               tech->nom, tech->current_cd, C_RESET);
        return;
    }

    printf("\n%s%s%s utilise %s sur %s !%s\n",
           C_CYAN, attaquant->nom, C_RESET,
           tech->nom, cible->nom, C_RESET);

    switch (tech->type) {
        case TECH_DAMAGE:
            cible->pv_courants -= tech->value;
            if (cible->pv_courants < 0) cible->pv_courants = 0;
            printf("%s%s%s subit %s%d%s degats (PV : %d/%d)\n",
                   C_CYAN, cible->nom, C_RESET,
                   C_RED, tech->value, C_RESET,
                   cible->pv_courants, cible->pv_max);
            break;
        case TECH_HEAL:
            cible->pv_courants += tech->value;
            if (cible->pv_courants > cible->pv_max)
                cible->pv_courants = cible->pv_max;
            printf("%s%s%s recupere %s%d%s PV (PV : %d/%d)\n",
                   C_CYAN, cible->nom, C_RESET,
                   C_GREEN, tech->value, C_RESET,
                   cible->pv_courants, cible->pv_max);
            break;
        case TECH_BUFF:
            cible->attaque += tech->value;
            printf("%s%s%s gagne +%d d'attaque pour %d tours\n",
                   C_CYAN, cible->nom, C_RESET,
                   tech->value, tech->cooldown);
            // On pourrait ajouter un Effet ici
            break;
        case TECH_DEBUFF:
            cible->defense -= tech->value;
            printf("%s%s%s perd %d de defense pour %d tours\n",
                   C_CYAN, cible->nom, C_RESET,
                   tech->value, tech->cooldown);
            // On pourrait ajouter un Effet ici
            break;
    }

    tech->current_cd = tech->cooldown;
}

void majRecharge(Technique *tech) {
    if (tech->current_cd > 0) tech->current_cd--;
}

// Choix aléatoire d'une cible dans une équipe
int choisirCibleAleatoire(Equipe *adversaire) {
    if (adversaire->taille <= 0) return -1;
    return rand() % adversaire->taille;
}

// Attaque de l'IA (utilise toujours le premier de son équipe)
void attaqueIA(Equipe *equipeIA, Equipe *equipeJoueur, int niveau) {
    int cibleIndex = 0;
    switch (niveau) {
        case 1:
            cibleIndex = rand() % equipeJoueur->taille;
            break;
        case 2:
            for (int i = 1; i < equipeJoueur->taille; i++) {
                if (equipeJoueur->combattants[i].pv_courants > 0 &&
                    equipeJoueur->combattants[i].pv_courants <
                    equipeJoueur->combattants[cibleIndex].pv_courants)
                {
                    cibleIndex = i;
                }
            }
            break;
        case 3:
            if (rand() % 2 == 0) {
                printf("\n%s%s%s utilise une technique speciale !%s\n",
                       C_CYAN,
                       equipeIA->combattants[0].nom,
                       C_RESET,
                       C_RESET);
                return;
            }
            for (int i = 1; i < equipeJoueur->taille; i++) {
                if (equipeJoueur->combattants[i].pv_courants > 0 &&
                    equipeJoueur->combattants[i].pv_courants <
                    equipeJoueur->combattants[cibleIndex].pv_courants)
                {
                    cibleIndex = i;
                }
            }
            break;
        default:
            printf("Niveau IA inconnu !\n");
            return;
    }
    attaquer(&equipeIA->combattants[0],
             &equipeJoueur->combattants[cibleIndex]);
}

// Affiche barre de vie et effets
void afficherBarreVieAvecEffets(Combattant *c) {
    int pct = c->pv_courants * 100 / c->pv_max;
    const char *col = C_YELLOW;
    if (pct > 50) col = C_CYAN;
    else if (pct <= 20) col = C_RED;

    printf("\n%s%-10s %s[", C_CYAN, c->nom, C_RESET);
    for (int i = 0; i < c->pv_courants / 5; i++)
        printf("%s#%s", col, C_RESET);
    printf("] (%d/%d PV)\n", c->pv_courants, c->pv_max);

    int has = 0;
    for (int i = 0; i < MAX_EFFETS; i++) {
        if (c->effets[i].tours_restants > 0) {
            if (!has) { printf("   Effets :\n"); has = 1; }
            printf("     - %s (%d tours restants)\n",
                   c->effets[i].nom, c->effets[i].tours_restants);
        }
    }
    if (!has) printf("   (pas d'effets actifs)\n");
}
