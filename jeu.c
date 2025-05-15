#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "structures.h"
#include "combat.h"
#include "techniques.h"
#include "cartes.h"
#include "jeu.h"

// Couleurs ANSI
#define C_RESET    "\033[0m"
#define C_BLUE     "\033[1;34m"
#define C_CYAN     "\033[1;36m"
#define C_GREEN    "\033[1;32m"
#define C_YELLOW   "\033[1;33m"
#define C_RED      "\033[1;31m"


// chargerCombattants : lit combattant.txt 
// remplit un tableau de structure Combattant

void chargerCombattants(const char *filename, Combattant liste[], int *taille) {
    FILE *fp = fopen(filename, "r");
    if (!fp) { perror(filename); *taille = 0; return; }
    *taille = 0;
    while (*taille < MAX_DISPO) {
        char name[50];
        int pv_max, atk, def, agi, vit;
        if (fscanf(fp, "%49s %d %d %d %d %d", 
                   name, &pv_max, &atk, &def, &agi, &vit) != 6)
            break;
        Combattant *c = &liste[(*taille)++];          // tableau de structure combattant
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
    }
    fclose(fp);
}

// creerEquipe : affiche la liste dispo[], demande choisir perso de 0 a nbDispo-1
// et le retire de dispo[] pour l’ajouter à l’équipe
// jusqu’à TEAM_SIZE combattants ou plus de dispo[].
// Affiche l’équipe créée
void creerEquipe(Equipe *equipe, Combattant dispo[], int *nbDispo) {
    printf("%s=== Creation de l'equipe %s ===%s\n",
           C_BLUE, equipe->nom, C_RESET);
    equipe->taille = 0;
    equipe->combattants = malloc(sizeof(Combattant) * TEAM_SIZE);

    while (equipe->taille < TEAM_SIZE && *nbDispo > 0) {
        printf("\nCombattants disponibles :\n");
        for (int i = 0; i < *nbDispo; i++) {
            Combattant *d = &dispo[i];
            printf(" %s%2d)%s %-10s PV:%3d ATK:%2d DEF:%2d AGI:%2d VIT:%2d\n",
                   C_CYAN, i, C_RESET,
                   d->nom, d->pv_courants,
                   d->attaque, d->defense,
                   d->agilite, d->vitesse);
        }
        printf("%sVotre choix (0-%d) : %s",
               C_GREEN, *nbDispo - 1, C_RESET);
        int choix;
        if (scanf("%d", &choix) != 1) { getchar(); continue; }
        if (choix < 0 || choix >= *nbDispo) {
            printf("%sChoix invalide%s\n", C_RED, C_RESET);
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

// afficherEtatEquipes : barres de vie + effets colorés

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


// boucleCombat (solo IA) 

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
            if (actif->pv_courants <= 0) 
                continue;

            // Déterminer si 'actif' appartient au joueur ou à l'IA
            int isJoueur = 0;
            for (int i = 0; i < joueur->taille; i++) {
                if (strcmp(joueur->combattants[i].nom, actif->nom) == 0) {
                    isJoueur = 1;
                    break;
                }
            }

            printf("\n%s>>> Tour de %s <<<%s\n",
                   C_YELLOW, actif->nom, C_RESET);

            if (isJoueur) {
                // ==== Tour du joueur ====
                draw_card(deck, mainJ);
                afficher_main(mainJ);

                // Jouer une carte
                printf("%sCarte a jouer (-1 passer) : %s", C_GREEN, C_RESET);
                int ci;
                if (scanf("%d", &ci) == 1) {
                    while (getchar() != '\n');
                    if (ci >= 0 && ci < mainJ->size) {
                        Carte *carte = &mainJ->cartes[ci];
                        Combattant *cibleCarte = NULL;

                        if (strcmp(carte->type, "Offensive") == 0) { //compare chaine
                            printf("%sChoisir cible IA (0-%d) : %s",
                                   C_GREEN, ia->taille - 1, C_RESET);
                            int idx; scanf("%d", &idx); while(getchar()!='\n');
                            if (idx >= 0 && idx < ia->taille)
                                cibleCarte = &ia->combattants[idx];
                        } else {
                            printf("%sChoisir cible allie (0-%d) : %s",
                                   C_GREEN, joueur->taille - 1, C_RESET);
                            int idx; scanf("%d", &idx); while(getchar()!='\n');
                            if (idx >= 0 && idx < joueur->taille)
                                cibleCarte = &joueur->combattants[idx];
                        }

                        if (cibleCarte)
                            play_card(mainJ, ci, cibleCarte);
                    }
                }

                // Attaque classique
                printf("%sChoisir cible IA (0-%d) : %s",
                       C_GREEN, ia->taille - 1, C_RESET);
                int at; 
                if (scanf("%d", &at) == 1) {
                    while (getchar() != '\n');
                    if (at >= 0 && at < ia->taille)
                        attaquer(actif, &ia->combattants[at]);
                }

            } else {
                // ==== Tour de l'IA ====
                draw_card(deck, mainIA);
                if (mainIA->size > 0 && rand() % 2 == 0) {
                    int ri = rand() % mainIA->size;
                    play_card(mainIA, ri, actif);
                }
                int ci = choisirCibleAleatoire(joueur);
                if (ci >= 0)
                    attaquer(actif, &joueur->combattants[ci]);
            }

            // Mise à jour des effets et cooldowns
            majEffetsActifs(actif);
            for (int k = 0; k < actif->nb_techniques; k++)
                majRecharge(&actif->techniques[k]);
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

    // Fin du combat
    printf("\n%s=== Combat termine ===%s\n", C_BLUE, C_RESET);
    if (joueur->taille > 0)
        printf("%sVous avez gagne !%s\n", C_GREEN, C_RESET);
    else
        printf("%sL'IA a gagne...%s\n", C_RED, C_RESET);

    free_deck(deck);
    free_hand(mainJ);
    free_hand(mainIA);
}

// ------------------------------------------------------------------
// boucleCombatLocal : 2 joueurs en local
// ------------------------------------------------------------------
void boucleCombatLocal(Equipe *p1, Equipe *p2) {
    Combattant ordre[TEAM_SIZE * 2];
    int nOrdre;
    organiserTours(p1, p2, ordre, &nOrdre);

    int nCartes;
    Deck *deck  = init_deck(CARTES_FILE, &nCartes);
    Hand *main1 = init_hand(5);
    Hand *main2 = init_hand(5);

    while (p1->taille > 0 && p2->taille > 0) {
        afficherEtatEquipes(p1, p2);

        for (int t = 0; t < nOrdre; t++) {
            Combattant *act = &ordre[t];
            if (act->pv_courants <= 0) continue;

            // Détermine si c'est Joueur 1 ou Joueur 2
            int estJ1 = 0;
            for (int i = 0; i < p1->taille; i++)
                if (strcmp(p1->combattants[i].nom, act->nom) == 0)
                    estJ1 = 1;

            // Indication du joueur
            printf("\n%s[%s]%s\n",
                   C_CYAN,
                   estJ1 ? "Tour Joueur 1" : "Tour Joueur 2",
                   C_RESET);

            printf("%s>>> Tour de %s <<<%s\n",
                   C_YELLOW, act->nom, C_RESET);

            Hand   *mainAct = estJ1 ? main1 : main2;
            Equipe *adv     = estJ1 ? p2    : p1;
            Equipe *ally    = estJ1 ? p1    : p2;

            // 1) Pioche + main
            draw_card(deck, mainAct);
            afficher_main(mainAct);

            // 2) Jouer une carte
            printf("%sCarte a jouer (-1 passer) : %s", C_GREEN, C_RESET);
            int ci;
            if (scanf("%d", &ci) == 1) {
                while (getchar() != '\n');
                if (ci >= 0 && ci < mainAct->size) {
                    Carte *carte = &mainAct->cartes[ci];
                    Combattant *cible = NULL;
                    if (strcmp(carte->type, "Offensive") == 0) {
                        printf("%sChoisir cible ennemi (0-%d) : %s",
                               C_GREEN, adv->taille - 1, C_RESET);
                        int idx; scanf("%d", &idx); while(getchar()!='\n');
                        if (idx >= 0 && idx < adv->taille)
                            cible = &adv->combattants[idx];
                    } else {
                        printf("%sChoisir cible allie (0-%d) : %s",
                               C_GREEN, ally->taille - 1, C_RESET);
                        int idx; scanf("%d", &idx); while(getchar()!='\n');
                        if (idx >= 0 && idx < ally->taille)
                            cible = &ally->combattants[idx];
                    }
                    if (cible)
                        play_card(mainAct, ci, cible);
                }
            }

            // 3) Attaque basique
            printf("%sChoisir cible ennemi (0-%d) : %s",
                   C_GREEN, adv->taille - 1, C_RESET);
            int at;
            if (scanf("%d", &at) == 1) {
                while (getchar() != '\n');
                if (at >= 0 && at < adv->taille)
                    attaquer(act, &adv->combattants[at]);
            }

            // 4) Mise à jour effets & cooldowns
            majEffetsActifs(act);
            for (int k = 0; k < act->nb_techniques; k++)
                majRecharge(&act->techniques[k]);
        }

        // 5) Purge des KO
        int newSz = 0;
        for (int i = 0; i < p1->taille; i++)
            if (p1->combattants[i].pv_courants > 0)
                p1->combattants[newSz++] = p1->combattants[i];
        p1->taille = newSz;

        newSz = 0;
        for (int i = 0; i < p2->taille; i++)
            if (p2->combattants[i].pv_courants > 0)
                p2->combattants[newSz++] = p2->combattants[i];
        p2->taille = newSz;
    }

    // 6) Affichage du résultat
    printf("\n%s=== Combat termine ===%s\n", C_BLUE, C_RESET);
    if (p1->taille > 0)
        printf("%sJoueur 1 gagne !%s\n", C_GREEN, C_RESET);
    else
        printf("%sJoueur 2 gagne !%s\n", C_RED,   C_RESET);

    // 7) Libération
    free_deck(deck);
    free_hand(main1);
    free_hand(main2);
}

// ------------------------------------------------------------------
// menuPrincipal : choix 1=IA, 2=local, 3=état, 4=quitter
// ------------------------------------------------------------------
void menuPrincipal(Equipe *joueur, Equipe *ia) {
    int mode;
    do {
        printf("\n%s=== MENU PRINCIPAL ===%s\n",C_BLUE,C_RESET);
        printf("  %s1)%s Jouer contre l'IA\n",        C_GREEN,C_RESET);
        printf("  %s2)%s Jouer en local (2 joueurs)\n", C_GREEN,C_RESET);
        printf("  %s3)%s Voir etat des equipes\n",      C_GREEN,C_RESET);
        printf("  %s4)%s Quitter\n",                   C_GREEN,C_RESET);
        printf("%sVotre choix : %s",C_YELLOW,C_RESET);
        if(scanf("%d",&mode)!=1) { getchar(); mode=0; }

        switch(mode) {
            case 1: boucleCombat(joueur,ia);    break;
            case 2: boucleCombatLocal(joueur,ia); break;
            case 3: afficherEtatEquipes(joueur,ia); break;
            case 4: printf("%sAu revoir !%s\n",C_CYAN,C_RESET); break;
            default: printf("%sChoix invalide%s\n",C_RED,C_RESET);
        }
    } while(mode!=4);
}

