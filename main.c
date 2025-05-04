// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "structures.h"
#include "combat.h"
#include "techniques.h"
#include "cartes.h"

#define MAX_DISPO 10
#define MAX_ELEM  10    // max total combattants (e1 + e2)
#define TEAM_SIZE 5
#define CARTES_FILE "cartes.txt"
#define COMBATS_FILE "combattants.txt"

// Charge les combattants depuis un fichier texte :
// format par ligne : nom pv_max attaque defense agilite vitesse
void chargerCombattants(const char *filename, Combattant liste[], int *taille) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror(filename);
        *taille = 0;
        return;
    }
    *taille = 0;
    char nom[50];
    int pv, atk, def, agi, vit;
    while (fscanf(f, "%49s %d %d %d %d %d",
                  nom, &pv, &atk, &def, &agi, &vit) == 6)
    {
        Combattant c;
        strncpy(c.nom, nom, 50);
        c.pv_max     = pv;
        c.pv_courants = pv;
        c.attaque    = atk;
        c.defense    = def;
        c.agilite    = agi;
        c.vitesse    = vit;
        c.nb_techniques = 0;
        c.techniques = NULL;
        for (int i = 0; i < MAX_EFFETS; i++)
            c.effets[i].tours_restants = 0;
        liste[(*taille)++] = c;
        if (*taille >= MAX_DISPO) break;
    }
    fclose(f);
}

// Crée une équipe de taille TEAM_SIZE en choisissant parmi dispo[]
void creerEquipe(Equipe *equipe, Combattant dispo[], int nbDispo) {
    printf("Création de l'équipe %s\n", equipe->nom);
    equipe->taille = 0;
    equipe->combattants = malloc(sizeof(Combattant) * TEAM_SIZE);
    while (equipe->taille < TEAM_SIZE) {
        printf("\nCombattants disponibles :\n");
        for (int i = 0; i < nbDispo; i++) {
            printf("%2d) %-10s (PV:%3d ATK:%2d DEF:%2d VIT:%2d)\n",
                   i,
                   dispo[i].nom,
                   dispo[i].pv_max,
                   dispo[i].attaque,
                   dispo[i].defense,
                   dispo[i].vitesse);
        }
        printf("Choisissez un combattant (0-%d) : ", nbDispo - 1);
        int choix;
        if (scanf("%d", &choix) != 1) { getchar(); continue; }
        if (choix >= 0 && choix < nbDispo) {
            equipe->combattants[equipe->taille++] = dispo[choix];
            printf("  -> %s ajouté(e).\n", dispo[choix].nom);
        } else {
            printf("  Choix invalide.\n");
        }
    }
}

// Affiche l'état (barre de vie) des deux équipes
void afficherEtatEquipes(Equipe *e1, Equipe *e2) {
    printf("\n--- État des équipes ---\n");
    printf("%s :\n", e1->nom);
    for (int i = 0; i < e1->taille; i++)
        afficherBarreVieAvecEffets(&e1->combattants[i]);
    printf("\n%s :\n", e2->nom);
    for (int i = 0; i < e2->taille; i++)
        afficherBarreVieAvecEffets(&e2->combattants[i]);
    printf("-------------------------\n");
}

// Boucle de combat : pioche et joue cartes, puis attaque/IA
void boucleCombat(Equipe *joueur, Equipe *ia) {
    Combattant ordre[MAX_ELEM];
    int nOrdre;
    organiserTours(joueur, ia, ordre, &nOrdre);

    // Charger les cartes disponibles
    Carte deck[MAX_CARTES];
    int nCartes;
    chargerCartes(CARTES_FILE, deck, &nCartes);

    while (joueur->taille > 0 && ia->taille > 0) {
        afficherEtatEquipes(joueur, ia);

        for (int i = 0; i < nOrdre; i++) {
            Combattant *actif = &ordre[i];
            if (actif->pv_courants <= 0) continue;

            // Déterminer si c'est un joueur ou l'IA
            int isJoueur = 0;
            for (int j = 0; j < joueur->taille; j++)
                if (actif == &joueur->combattants[j]) isJoueur = 1;

            printf("\n>>> Tour de %s <<<\n", actif->nom);

            if (isJoueur) {
                // Afficher cartes et proposer d'en jouer une
                afficherCartesDisponibles(deck, nCartes);
                printf("Index carte à jouer (-1 pour passer) : ");
                int ci; scanf("%d", &ci);
                if (ci >= 0 && ci < nCartes)
                    utiliserCarte(&deck[ci], actif);

                // Attaque
                printf("Sélectionnez une cible IA (0-%d) : ", ia->taille - 1);
                int tidx; scanf("%d", &tidx);
                if (tidx >= 0 && tidx < ia->taille)
                    attaquer(actif, &ia->combattants[tidx]);
            } else {
                // Tour de l'IA : attaque simple
                attaqueIA(ia, joueur, 2);  // niveau 2 facile
            }

            // Mettre à jour effets actifs
            majEffetsActifs(actif);
        }

        // Nettoyer les KO : réduire taille d'équipe
        int newJ = 0;
        for (int i = 0; i < joueur->taille; i++)
            if (joueur->combattants[i].pv_courants > 0)
                joueur->combattants[newJ++] = joueur->combattants[i];
        joueur->taille = newJ;

        int newIA = 0;
        for (int i = 0; i < ia->taille; i++)
            if (ia->combattants[i].pv_courants > 0)
                ia->combattants[newIA++] = ia->combattants[i];
        ia->taille = newIA;
    }

    printf("\n=== Combat terminé ! ===\n");
    if (joueur->taille > 0)
        printf("Vous avez gagné !\n");
    else
        printf("L'IA a gagné...\n");
}

// Menu principal
void menuPrincipal(Equipe *joueur, Equipe *ia) {
    int choix;
    do {
        printf("\n--- Menu ---\n"
               "1) Lancer combat\n"
               "2) Voir état équipes\n"
               "3) Quitter\n"
               "Votre choix : ");
        scanf("%d", &choix);
        switch (choix) {
            case 1:
                boucleCombat(joueur, ia);
                break;
            case 2:
                afficherEtatEquipes(joueur, ia);
                break;
            case 3:
                printf("Au revoir !\n");
                break;
            default:
                printf("Choix invalide.\n");
        }
    } while (choix != 3);
}

int main(void) {
    srand((unsigned)time(NULL));

    // Charger combattants
    Combattant dispo[MAX_DISPO];
    int nbDispo;
    chargerCombattants(COMBATS_FILE, dispo, &nbDispo);

    // Créer équipes
    Equipe joueur = { .nom = "Joueur", .taille = 0 };
    Equipe ia     = { .nom = "IA",     .taille = 0 };
    creerEquipe(&joueur, dispo, nbDispo);
    creerEquipe(&ia,     dispo, nbDispo);

    // Lancer le menu principal
    menuPrincipal(&joueur, &ia);

    // Libération mémoire
    free(joueur.combattants);
    free(ia.combattants);
    return 0;
}
