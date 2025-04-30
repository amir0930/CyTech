#include <stdio.h>
#include <stdlib.h>
#include "combat.h"
#include "structures.h" 

void attaquer(Combattant *attaquant, Combattant *cible) {
    int degats = attaquant->attaque - cible->defense;
    if (degats > 0) {
        cible->pv_courants -= degats;
        printf("%s attaque %s et inflige %d dégâts !\n", attaquant->nom, cible->nom, degats);
    } else {
        printf("%s attaque %s mais l'attaque est trop faible !\n", attaquant->nom, cible->nom);
    }
}


void organiserTours(Equipe *equipe1, Equipe *equipe2, Combattant ordreCombat[], int *taille) {
    int index = 0;

    // Ajouter les combattants de chaque équipe à la liste d'ordre
    for (int i = 0; i < equipe1->taille; i++) {
        ordreCombat[index++] = equipe1->combattants[i];
    }
    for (int i = 0; i < equipe2->taille; i++) {
        ordreCombat[index++] = equipe2->combattants[i];
    }

    *taille = index;

    // Trier les combattants selon leur vitesse
    qsort(ordreCombat, *taille, sizeof(Combattant), comparerVitesse);
}

//Ajoute la gestion des durées des effets actifs sur les combattants.
void majEffetsActifs(Combattant *combattant) {
    for (int i = 0; i < 3; i++) { // Maximum 3 effets actifs
        if (combattant->effets[i].tours_restants > 0) {
            combattant->effets[i].tours_restants--;
            if (combattant->effets[i].tours_restants == 0) {
                printf("L'effet %s sur %s a expiré.\n", combattant->effets[i].nom, combattant->nom);
                combattant->attaque -= combattant->effets[i].valeur;  // Exemple : retrait du bonus d'attaque
            }
        }
    }
}

//technique spéciale
void utiliserTechnique(Combattant *attaquant, Combattant *cible, Technique *technique) {
    if (technique->recharge > 0) {
        printf("%s ne peut pas encore utiliser %s (Recharge restante : %d tours)\n", 
               attaquant->nom, technique->nom, technique->recharge);
        return;
    }
    printf("%s utilise %s sur %s !\n", attaquant->nom, technique->nom, cible->nom);
    cible->attaque += technique->valeur;  // Applique l'effet de la technique
    technique->recharge = 3;  // Maintenant la mise à jour est effective
}

void majRecharge(Technique *technique) {
    if (technique->recharge > 0) {
        technique->recharge--;
    }
}

int choisirCibleAleatoire(Equipe adversaire) {
    return rand() % adversaire.taille;  // Cible aléatoire
}

// ATTAQUE DE L'IA
void attaqueIA(Equipe *equipeIA, Equipe *equipeJoueur, int niveau) {
    // Trouver une cible selon le niveau
    int cibleIndex = 0;
    switch (niveau) {
        case 1:  // Niveau noob : attaque aléatoire
            cibleIndex = rand() % equipeJoueur->taille;
            break;

        case 2:  // Niveau facile : cible le combattant le plus faible
            for (int i = 1; i < equipeJoueur->taille; i++) {
                if (equipeJoueur->combattants[i].pv_courants > 0 &&
                    equipeJoueur->combattants[i].pv_courants < equipeJoueur->combattants[cibleIndex].pv_courants) {
                    cibleIndex = i;
                }
            }
            break;

        case 3:  // Niveau moyen : utilise aussi des techniques spéciales si possible
            if (rand() % 2 == 0) {  // 50% de chances d'utiliser une technique spéciale
                printf("%s utilise une technique spéciale !\n", equipeIA->combattants[0].nom);
                return;  // Ici, on simplifie avec juste une indication pour l'instant
            }
            // Sinon, cible le plus faible comme en "facile"
            for (int i = 1; i < equipeJoueur->taille; i++) {
                if (equipeJoueur->combattants[i].pv_courants > 0 &&
                    equipeJoueur->combattants[i].pv_courants < equipeJoueur->combattants[cibleIndex].pv_courants) {
                    cibleIndex = i;
                }
            }
            break;

        default:
            printf("Niveau IA inconnu !\n");
            return;
    }

void afficherBarreVieAvecEffets(Combattant *combattant) {
    printf("%s [", combattant->nom);

    int pourcentage = (combattant->pv_courants * 100) / combattant->pv_max;
    if (pourcentage > 50) {
        printf("\033[1;32m");  // Vert si PV > 50%
    } else if (pourcentage > 20) {
        printf("\033[1;33m");  // Jaune si 20% < PV <= 50%
    } else {
        printf("\033[1;31m");  // Rouge si PV <= 20%
    }

    for (int i = 0; i < combattant->pv_courants / 5; i++) printf("#");
    printf("\033[0m] (%d/%d PV)\n", combattant->pv_courants, combattant->pv_max);

    printf("Effets actifs :\n");
    for (int i = 0; i < 3; i++) {
        if (combattant->effets[i].tours_restants > 0) {
            printf("- %s (%d tours restants)\n", combattant->effets[i].nom, combattant->effets[i].tours_restants);
        }
    }
}
