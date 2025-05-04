// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "structures.h"
#include "combat.h"
#include "techniques.h"
#include "cartes.h"

#define MAX_DISPO    10
#define TEAM_SIZE    5
#define CARTES_FILE  "cartes.txt"
#define COMBATS_FILE "combattants.txt"

// Couleurs ANSI
#define C_RESET    "\033[0m"
#define C_BLUE     "\033[1;34m"
#define C_CYAN     "\033[1;36m"
#define C_GREEN    "\033[1;32m"
#define C_YELLOW   "\033[1;33m"
#define C_RED      "\033[1;31m"

// Prototypes
void chargerCombattants(const char *filename, Combattant liste[], int *taille);
void creerEquipe(Equipe *equipe, Combattant dispo[], int *nbDispo);
void afficherEtatEquipes(Equipe *e1, Equipe *e2);
void boucleCombat(Equipe *joueur, Equipe *ia);

// ------------------------------------------------------------------
// Chargement des combattants (6 champs : nom, pv_max, atk, def, agi, vit)
// ------------------------------------------------------------------
void chargerCombattants(const char *filename, Combattant liste[], int *taille) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror(filename);
        *taille = 0;
        return;
    }
    *taille = 0;
    while (*taille < MAX_DISPO) {
        char name[50];
        int pv_max, atk, def, agi, vit;
        if (fscanf(f, "%49s %d %d %d %d %d",
                   name, &pv_max, &atk, &def, &agi, &vit) != 6)
            break;

        Combattant *c = &liste[*taille];
        strcpy(c->nom, name);
        c->pv_max       = pv_max;
        c->pv_courants  = pv_max;
        c->attaque      = atk;
        c->defense      = def;
        c->agilite      = agi;
        c->vitesse      = vit;
        c->nb_techniques = 0;
        c->techniques    = NULL;
        for (int i = 0; i < MAX_EFFETS; i++)
            c->effets[i].tours_restants = 0;

        (*taille)++;
    }
    fclose(f);
}

// ------------------------------------------------------------------
// Création d'une équipe sans doublon : retire chaque choix de dispo
// ------------------------------------------------------------------
void creerEquipe(Equipe *equipe, Combattant dispo[], int *nbDispo) {
    printf("%s=== Creation de l'equipe %s ===%s\n",
           C_BLUE, equipe->nom, C_RESET);

    equipe->taille      = 0;
    equipe->combattants = malloc(sizeof(Combattant) * TEAM_SIZE);

    while (equipe->taille < TEAM_SIZE && *nbDispo > 0) {
        printf("\nCombattants disponibles :\n");
        for (int i = 0; i < *nbDispo; i++) {
            Combattant *d = &dispo[i];
            printf(" %s%2d)%s %-10s PV:%3d/%-3d ATK:%2d DEF:%2d AGI:%2d VIT:%2d\n",
                   C_CYAN, i, C_RESET,
                   d->nom, d->pv_courants, d->pv_max,
                   d->attaque, d->defense, d->agilite, d->vitesse);
        }

        printf("%sVotre choix (0-%d) : %s",
               C_GREEN, *nbDispo - 1, C_RESET);
        int choix;
        if (scanf("%d", &choix) != 1) { getchar(); continue; }

        if (choix < 0 || choix >= *nbDispo) {
            printf("%s  Choix invalide%s\n", C_RED, C_RESET);
            continue;
        }

        printf("  %s-> %s ajoute%s\n",
               C_YELLOW, dispo[choix].nom, C_RESET);
        equipe->combattants[equipe->taille++] = dispo[choix];
        for (int j = choix; j < *nbDispo - 1; j++)
            dispo[j] = dispo[j+1];
        (*nbDispo)--;
    }
}

// ------------------------------------------------------------------
// Affiche l'état (barres de vie + effets) des deux équipes
// ------------------------------------------------------------------
void afficherEtatEquipes(Equipe *e1, Equipe *e2) {
    printf("\n%s--- ETAT DES EQUIPES --- %s\n", C_BLUE, C_RESET);

    printf("%s :\n", e1->nom);
    for (int i = 0; i < e1->taille; i++)
        afficherBarreVieAvecEffets(&e1->combattants[i]);

    printf("\n%s :\n", e2->nom);
    for (int i = 0; i < e2->taille; i++)
        afficherBarreVieAvecEffets(&e2->combattants[i]);

    printf("%s------------------------%s\n", C_BLUE, C_RESET);
}

// ------------------------------------------------------------------
// Boucle de combat principale (distinction joueur/IA par strcmp)
// ------------------------------------------------------------------
void boucleCombat(Equipe *joueur, Equipe *ia) {
    Combattant ordre[TEAM_SIZE * 2];
    int nOrdre;
    organiserTours(joueur, ia, ordre, &nOrdre);

    int nCartes;
    Deck *deck   = init_deck(CARTES_FILE, &nCartes);
    Hand *mainJ  = init_hand(5);
    Hand *mainIA = init_hand(5);

    while (joueur->taille > 0 && ia->taille > 0) {
        afficherEtatEquipes(joueur, ia);

        for (int t = 0; t < nOrdre; t++) {
            Combattant *actif = &ordre[t];
            if (actif->pv_courants <= 0) continue;

            // Détecter si c'est un combattant du joueur
            int isJoueur = 0;
            for (int i = 0; i < joueur->taille; i++) {
                if (strcmp(joueur->combattants[i].nom,
                           actif->nom) == 0) {
                    isJoueur = 1;
                    break;
                }
            }

            printf("\n%s>>> Tour de %s <<<%s\n",
                   C_YELLOW, actif->nom, C_RESET);

            if (isJoueur) {
                // Interaction du joueur
                draw_card(deck, mainJ);
                afficher_main(mainJ);
                printf("%sCarte a jouer (-1 passer) : %s",
                       C_GREEN, C_RESET);
                int ci; scanf("%d", &ci);
                if (ci >= 0 && ci < mainJ->size)
                    play_card(mainJ, ci, actif);

                printf("%sChoisir cible IA (0-%d) : %s",
                       C_GREEN, ia->taille - 1, C_RESET);
                int idx; scanf("%d", &idx);
                if (idx >= 0 && idx < ia->taille)
                    attaquer(actif, &ia->combattants[idx]);

            } else {
                // Tour de l'IA
                draw_card(deck, mainIA);
                if (mainIA->size > 0 && rand() % 2 == 0) {
                    int ri = rand() % mainIA->size;
                    play_card(mainIA, ri, actif);
                }
                // <- ici on passe le pointeur Equipe*
                int ci = choisirCibleAleatoire(joueur);
                attaquer(actif, &joueur->combattants[ci]);
            }

            // Mise à jour des effets et cooldowns
            majEffetsActifs(actif);
            for (int ti = 0; ti < actif->nb_techniques; ti++)
                majRecharge(&actif->techniques[ti]);
        }

        // Purge des KO
        int nj = 0;
        for (int i = 0; i < joueur->taille; i++)
            if (joueur->combattants[i].pv_courants > 0)
                joueur->combattants[nj++] = joueur->combattants[i];
        joueur->taille = nj;

        int ni = 0;
        for (int i = 0; i < ia->taille; i++)
            if (ia->combattants[i].pv_courants > 0)
                ia->combattants[ni++] = ia->combattants[i];
        ia->taille = ni;
    }

    printf("\n%s=== Combat termine ===%s\n",
           C_BLUE, C_RESET);
    if (joueur->taille > 0)
        printf("%sVous avez gagne !%s\n", C_GREEN, C_RESET);
    else
        printf("%sL'IA a gagne...%s\n", C_RED, C_RESET);

    free_deck(deck);
    free_hand(mainJ);
    free_hand(mainIA);
}

// ------------------------------------------------------------------
// Menu principal
// ------------------------------------------------------------------
void menuPrincipal(Equipe *joueur, Equipe *ia) {
    int choix;
    do {
        printf("\n%s--- MENU PRINCIPAL --- %s\n",
               C_BLUE, C_RESET);
        printf("  %s1)%s Lancer combat\n", C_GREEN, C_RESET);
        printf("  %s2)%s Voir etat des equipes\n", C_GREEN, C_RESET);
        printf("  %s3)%s Quitter\n", C_GREEN, C_RESET);
        printf("%sVotre choix : %s", C_YELLOW, C_RESET);
        scanf("%d", &choix);
        switch (choix) {
            case 1:
                boucleCombat(joueur, ia);
                break;
            case 2:
                afficherEtatEquipes(joueur, ia);
                break;
            case 3:
                printf("%sAu revoir !%s\n", C_CYAN, C_RESET);
                break;
            default:
                printf("%sChoix invalide.%s\n",
                       C_RED, C_RESET);
        }
    } while (choix != 3);
}

// ------------------------------------------------------------------
// Point d'entrée
// ------------------------------------------------------------------
int main(void) {
    srand((unsigned)time(NULL));

    Combattant dispo[MAX_DISPO];
    int nbDispo;
    chargerCombattants(COMBATS_FILE, dispo, &nbDispo);

    Equipe joueur = { .nom = "Joueur" };
    Equipe ia     = { .nom = "IA"     };

    creerEquipe(&joueur, dispo, &nbDispo);
    creerEquipe(&ia,     dispo, &nbDispo);

    menuPrincipal(&joueur, &ia);

    free(joueur.combattants);
    free(ia.combattants);
    return 0;
}
