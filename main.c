// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "structures.h"
#include "combat.h"
#include "techniques.h"
#include "cartes.h"

#define MAX_DISPO   10
#define TEAM_SIZE   5
#define CARTES_FILE "cartes.txt"
#define COMBATS_FILE "combattants.txt"

// ------------------------------------------------------------------
// Chargement des combattants
// ------------------------------------------------------------------
void chargerCombattants(const char *filename, Combattant liste[], int *taille) {
    FILE *f = fopen(filename, "r");
    if (!f) { perror(filename); *taille = 0; return; }

    *taille = 0;
    while (*taille < MAX_DISPO) {
        Combattant c;
        if (fscanf(f, "%49s %d %d %d %d %d",
                   c.nom,
                   &c.pv_max,
                   &c.attaque,
                   &c.defense,
                   &c.agilite,
                   &c.vitesse) != 6)
            break;
        c.pv_courants   = c.pv_max;
        c.nb_techniques = 0;
        c.techniques    = NULL;
        for (int i = 0; i < MAX_EFFETS; i++)
            c.effets[i].tours_restants = 0;
        liste[(*taille)++] = c;
    }
    fclose(f);
}

// ------------------------------------------------------------------
// Création d’une équipe par sélection
// ------------------------------------------------------------------
void creerEquipe(Equipe *equipe, Combattant dispo[], int nbDispo) {
    printf("=== Création de l'équipe %s ===\n", equipe->nom);
    equipe->taille     = 0;
    equipe->combattants = malloc(sizeof(Combattant) * TEAM_SIZE);

    while (equipe->taille < TEAM_SIZE) {
        printf("\nCombattants dispos :\n");
        for (int i = 0; i < nbDispo; i++) {
            printf("%2d) %-10s PV:%3d ATK:%2d DEF:%2d VIT:%2d\n",
                   i,
                   dispo[i].nom,
                   dispo[i].pv_max,
                   dispo[i].attaque,
                   dispo[i].defense,
                   dispo[i].vitesse);
        }
        printf("Votre choix (0-%d) : ", nbDispo-1);
        int c; if (scanf("%d", &c)!=1) { getchar(); continue; }
        if (c>=0 && c<nbDispo) {
            equipe->combattants[equipe->taille++] = dispo[c];
            printf("  -> %s ajouté\n", dispo[c].nom);
        } else {
            printf("  Choix invalide\n");
        }
    }
}

// ------------------------------------------------------------------
// Affichage de l'état de deux équipes
// ------------------------------------------------------------------
void afficherEtatEquipes(Equipe *e1, Equipe *e2) {
    printf("\n--- ÉTAT DES ÉQUIPES ---\n");
    printf("%s :\n", e1->nom);
    for (int i = 0; i < e1->taille; i++)
        afficherBarreVieAvecEffets(&e1->combattants[i]);
    printf("\n%s :\n", e2->nom);
    for (int i = 0; i < e2->taille; i++)
        afficherBarreVieAvecEffets(&e2->combattants[i]);
    printf("------------------------\n");
}

// ------------------------------------------------------------------
// Boucle de combat principale
// ------------------------------------------------------------------
void boucleCombat(Equipe *joueur, Equipe *ia) {
    // 1) Organisation des tours
    Combattant ordre[TEAM_SIZE * 2];
    int nOrdre;
    organiserTours(joueur, ia, ordre, &nOrdre);

    // 2) Préparation du deck et des mains
    int nCartes;
    Deck *deck      = init_deck(CARTES_FILE, &nCartes);
    Hand *mainJ     = init_hand(5);
    Hand *mainIA    = init_hand(5);

    // 3) Tour par tour
    while (joueur->taille > 0 && ia->taille > 0) {
        afficherEtatEquipes(joueur, ia);

        for (int t = 0; t < nOrdre; t++) {
            Combattant *actif = &ordre[t];
            if (actif->pv_courants <= 0) continue;

            // Détection joueur ou IA
            int isJ = 0;
            for (int i = 0; i < joueur->taille; i++)
                if (&joueur->combattants[i] == actif) { isJ = 1; break; }

            printf("\n>>> Tour de %s <<<\n", actif->nom);

            if (isJ) {
                // Joueur : pioche et joue 1 carte
                draw_card(deck, mainJ);
                afficher_main(mainJ);
                printf("Carte à jouer (-1=passer) : ");
                int ci; scanf("%d", &ci);
                if (ci >= 0 && ci < mainJ->size)
                    play_card(mainJ, ci, actif);

                // Attaque classique
                printf("Cible IA (0-%d) : ", ia->taille-1);
                int idx; scanf("%d",&idx);
                if (idx>=0 && idx<ia->taille)
                    attaquer(actif, &ia->combattants[idx]);

            } else {
                // IA : pioche et joue aléatoirement
                draw_card(deck, mainIA);
                if (mainIA->size > 0 && rand()%2 == 0) {
                    int ri = rand() % mainIA->size;
                    play_card(mainIA, ri, actif);
                }
                // Attaque IA
                attaqueIA(ia, joueur, 2);
            }

            // Mise à jour effets et CDs
            majEffetsActifs(actif);
            for (int ti = 0; ti < actif->nb_techniques; ti++)
                majRecharge(&actif->techniques[ti]);
        }

        // 4) Purge des KO
        int nj=0, ni=0;
        for (int i=0;i<joueur->taille;i++)
            if (joueur->combattants[i].pv_courants>0)
                joueur->combattants[nj++]=joueur->combattants[i];
        for (int i=0;i<ia->taille;i++)
            if (ia->combattants[i].pv_courants>0)
                ia->combattants[ni++]=ia->combattants[i];
        joueur->taille = nj;
        ia->taille     = ni;
    }

    // 5) Fin de combat
    printf("\n=== Combat terminé ===\n");
    printf(joueur->taille>0 ? "Vous avez gagné !\n" : "L'IA a gagné...\n");

    // 6) Libération mémoire
    free_deck(deck);
    free_hand(mainJ);
    free_hand(mainIA);
}

// ------------------------------------------------------------------
// Menu principal
// ------------------------------------------------------------------
void menuPrincipal(Equipe *j, Equipe *i) {
    int choix;
    do {
        printf("\n--- MENU PRINCIPAL ---\n"
               "1) Lancer combat\n"
               "2) Voir état\n"
               "3) Quitter\n"
               "Choix : ");
        scanf("%d", &choix);
        switch (choix) {
            case 1: boucleCombat(j, i); break;
            case 2: afficherEtatEquipes(j,i); break;
            case 3: printf("Bye !\n"); break;
            default: printf("Invalide.\n");
        }
    } while (choix != 3);
}

// ------------------------------------------------------------------
// Point d’entrée
// ------------------------------------------------------------------
int main(void) {
    srand((unsigned)time(NULL));

    // Charger dispo
    Combattant dispo[MAX_DISPO];
    int nbDispo;
    chargerCombattants(COMBATS_FILE, dispo, &nbDispo);

    // Créer équipes
    Equipe joueur = { .nom = "Joueur", .taille = 0, .combattants = NULL };
    Equipe ia     = { .nom = "IA",     .taille = 0, .combattants = NULL };
    creerEquipe(&joueur, dispo, nbDispo);
    creerEquipe(&ia,     dispo, &nbDispo);

    // Lancer menu
    menuPrincipal(&joueur, &ia);

    // Nettoyage
    free(joueur.combattants);
    free(ia.combattants);
    return 0;
}
