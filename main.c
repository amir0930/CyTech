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

// ------------------------------------------------------------------
// Chargement des combattants depuis un fichier texte
// Format attendu par ligne :
//   nom pv_courants pv_max attaque defense agilite vitesse nb_techniques
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
        Combattant *c = &liste[*taille];
        if (fscanf(f,
                   "%49s %d %d %d %d %d %d %d",
                   c->nom,
                   &c->pv_courants,
                   &c->pv_max,
                   &c->attaque,
                   &c->defense,
                   &c->agilite,
                   &c->vitesse,
                   &c->nb_techniques) != 8) {
            break;
        }
        // initialisation des techniques et des effets
        c->techniques = NULL;
        for (int i = 0; i < MAX_EFFETS; i++) {
            c->effets[i].tours_restants = 0;
        }
        (*taille)++;
    }
    fclose(f);
}

// ------------------------------------------------------------------
// Creation d'une equipe par selection utilisateur
// ------------------------------------------------------------------
void creerEquipe(Equipe *equipe, Combattant dispo[], int nbDispo) {
    printf("=== Creation de l'equipe %s ===\n", equipe->nom);
    equipe->taille      = 0;
    equipe->combattants = malloc(sizeof(Combattant) * TEAM_SIZE);

    while (equipe->taille < TEAM_SIZE) {
        printf("\nCombattants disponibles :\n");
        for (int i = 0; i < nbDispo; i++) {
            printf("%2d) %-10s PV:%3d/%3d ATK:%2d DEF:%2d AGI:%2d VIT:%2d\n",
                   i,
                   dispo[i].nom,
                   dispo[i].pv_courants,
                   dispo[i].pv_max,
                   dispo[i].attaque,
                   dispo[i].defense,
                   dispo[i].agilite,
                   dispo[i].vitesse);
        }
        printf("Votre choix (0-%d) : ", nbDispo - 1);
        int choix;
        if (scanf("%d", &choix) != 1) { getchar(); continue; }
        if (choix >= 0 && choix < nbDispo) {
            equipe->combattants[equipe->taille++] = dispo[choix];
            printf("  -> %s ajoute\n", dispo[choix].nom);
        } else {
            printf("  Choix invalide\n");
        }
    }
}

// ------------------------------------------------------------------
// Affiche l'etat (barres de vie + effets) de deux equipes
// ------------------------------------------------------------------
void afficherEtatEquipes(Equipe *e1, Equipe *e2) {
    printf("\n--- ETAT DES EQUIPES ---\n");

    printf("%s :\n", e1->nom);
    for (int i = 0; i < e1->taille; i++) {
        afficherBarreVieAvecEffets(&e1->combattants[i]);
    }

    printf("\n%s :\n", e2->nom);
    for (int i = 0; i < e2->taille; i++) {
        afficherBarreVieAvecEffets(&e2->combattants[i]);
    }

    printf("------------------------\n");
}

// ------------------------------------------------------------------
// Boucle de combat principale
// ------------------------------------------------------------------
void boucleCombat(Equipe *joueur, Equipe *ia) {
    Combattant ordre[TEAM_SIZE * 2];
    int nOrdre;
    organiserTours(joueur, ia, ordre, &nOrdre);

    int nCartes;
    Deck *deck     = init_deck(CARTES_FILE, &nCartes);
    Hand *mainJ    = init_hand(5);
    Hand *mainIA   = init_hand(5);

    while (joueur->taille > 0 && ia->taille > 0) {
        afficherEtatEquipes(joueur, ia);

        for (int t = 0; t < nOrdre; t++) {
            Combattant *actif = &ordre[t];
            if (actif->pv_courants <= 0) continue;

            int isJoueur = 0;
            for (int i = 0; i < joueur->taille; i++)
                if (&joueur->combattants[i] == actif) { isJoueur = 1; break; }

            printf("\n>>> Tour de %s <<<\n", actif->nom);

            if (isJoueur) {
                draw_card(deck, mainJ);
                afficher_main(mainJ);
                printf("Carte a jouer (-1 passer) : ");
                int ci; scanf("%d", &ci);
                if (ci >= 0 && ci < mainJ->size) play_card(mainJ, ci, actif);

                printf("Choisir cible IA (0-%d) : ", ia->taille - 1);
                int idx; scanf("%d", &idx);
                if (idx >= 0 && idx < ia->taille)
                    attaquer(actif, &ia->combattants[idx]);

            } else {
                draw_card(deck, mainIA);
                if (mainIA->size > 0 && rand() % 2 == 0) {
                    int ri = rand() % mainIA->size;
                    play_card(mainIA, ri, actif);
                }
                attaqueIA(ia, joueur, 2);
            }

            majEffetsActifs(actif);
            for (int ti = 0; ti < actif->nb_techniques; ti++)
                majRecharge(&actif->techniques[ti]);
        }

        // Purge KO
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

    printf("\n=== Combat termine ===\n");
    if (joueur->taille > 0)
        printf("Vous avez gagne !\n");
    else
        printf("L'IA a gagne...\n");

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
        printf("\n--- MENU PRINCIPAL ---\n"
               "1) Lancer combat\n"
               "2) Voir etat des equipes\n"
               "3) Quitter\n"
               "Votre choix : ");
        scanf("%d", &choix);
        switch (choix) {
            case 1: boucleCombat(joueur, ia); break;
            case 2: afficherEtatEquipes(joueur, ia); break;
            case 3: printf("Au revoir !\n"); break;
            default: printf("Choix invalide.\n");
        }
    } while (choix != 3);
}

// ------------------------------------------------------------------
// Point d'entree
// ------------------------------------------------------------------
int main(void) {
    srand((unsigned)time(NULL));

    Combattant dispo[MAX_DISPO];
    int nbDispo;
    chargerCombattants(COMBATS_FILE, dispo, &nbDispo);

    Equipe joueur = { .nom = "Joueur", .taille = 0, .combattants = NULL };
    Equipe ia     = { .nom = "IA",     .taille = 0, .combattants = NULL };
    creerEquipe(&joueur, dispo, nbDispo);
    creerEquipe(&ia,     dispo, nbDispo);

    menuPrincipal(&joueur, &ia);

    free(joueur.combattants);
    free(ia.combattants);
    return 0;
}
