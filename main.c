#include <stdio.h>
#include <stdlib.h>


// Effets temporaires appliqués aux combattants
typedef struct {
    char nom[50];
    int valeur;
    int tours_restants;
} EffetActif;

// Structure des combattants
typedef struct {
    char nom[50];
    int pv_courants;
    int pv_max;
    int attaque;
    int defense;
    int agilite;
    int vitesse;
    int nb_techniques;
    EffetActif effets[3];  // Stocker jusqu'à 3 effets actifs
} Combattant;


// Structure pour une équipe
typedef struct {
    char nom[50];
    Combattant combattants[5];  // Max 5 combattants par équipe, modifiable
    int taille;
} Equipe;

// Structure pour les techniques spéciales
typedef struct {
    char nom[50];
    int valeur;
    char description[100];
    int duree_effet;
    int recharge;
} Technique;

// exemple de fichier
//Sasuke 100 100 20 10 15 12 2
//Naruto 120 120 18 12 20 15 3
//Hinata 90 90 15 14 25 17 2

//charger un combattant à partir d'un fichier
void chargerCombattants(char *nomFichier, Combattant liste[], int *taille) {
    FILE *file = fopen(nomFichier, "r");
    if (file == NULL) {
        printf("Erreur de lecture du fichier %s\n", nomFichier);
        return;
    }

    *taille = 0;
    while (fscanf(file, "%s %d %d %d %d %d %d %d", 
                  liste[*taille].nom, &liste[*taille].pv_courants, 
                  &liste[*taille].pv_max, &liste[*taille].attaque, 
                  &liste[*taille].defense, &liste[*taille].agilite, 
                  &liste[*taille].vitesse, &liste[*taille].nb_techniques) == 8) {
        (*taille)++;
    }

    fclose(file);
}

//combat
void attaquer(Combattant *attaquant, Combattant *cible) {
    int degats = attaquant->attaque - cible->defense;
    if (degats > 0) {
        cible->pv_courants -= degats;
        printf("%s attaque %s et inflige %d dégâts !\n", attaquant->nom, cible->nom, degats);
    } else {
        printf("%s attaque %s mais l'attaque est trop faible !\n", attaquant->nom, cible->nom);
    }
}

//Si le joueur affronte un ordinateur
int choisirCibleAleatoire(Equipe adversaire) {
    return rand() % adversaire.taille;  // Cible aléatoire
}

//Carte
typedef struct {
    char nom[50];       // Nom de la carte
    char description[100]; // Effet détaillé
    int valeur;         // Valeur appliquée (ex: +20 attaque)
    int duree;          // Nombre de tours actifs
} Carte;

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


//effets sur plusieurs tours

void majEffets(Combattant *cible) {
    for (int i = 0; i < 3; i++) { // Supposons un max de 3 effets actifs
        if (cible->effets[i].tours_restants > 0) {
            cible->effets[i].tours_restants--;
            if (cible->effets[i].tours_restants == 0) {
                printf("L'effet %s sur %s a expiré.\n", cible->effets[i].nom, cible->nom);
            }
        }
    }
}

//combat selon la vitesse des combattants
int comparerVitesse(const void *a, const void *b) {
    return ((Combattant*)b)->vitesse - ((Combattant*)a)->vitesse;
}

void trierEquipeParVitesse(Equipe *equipe) {
    qsort(equipe->combattants, equipe->taille, sizeof(Combattant), comparerVitesse);
}



//barre de vie
void afficherBarreVie(Combattant c) {
    printf("%s [", c.nom);
    for (int i = 0; i < c.pv_courants / 5; i++) printf("#");
    printf("] (%d/%d PV)\n", c.pv_courants, c.pv_max);
}

void afficherMenu() {
    printf("\nQue veux-tu faire ?\n");
    printf("1 - Attaquer\n");
    printf("2 - Utiliser une technique spéciale\n");
    printf("3 - Voir les stats des combattants\n");
    printf("4 - Quitter\n");
}
// Fonction pour gérer les tours :

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


// action attaques et techniques speciales 
void effectuerTour(Combattant *attaquant, Equipe *equipeAdverse) {
    int choix;
    printf("%s : Que veux-tu faire ?\n", attaquant->nom);
    printf("1 - Attaquer\n");
    printf("2 - Utiliser une technique spéciale\n");
    printf("Choix : ");
    scanf("%d", &choix);

    if (choix == 1) {
        int cibleIndex;
        printf("Choisir un adversaire (numéro) : ");
        scanf("%d", &cibleIndex);

        // Effectuer l'attaque
        attaquer(attaquant, &equipeAdverse->combattants[cibleIndex]);
    } else if (choix == 2) {
        printf("Utilisation de technique spéciale non implémentée encore...\n");
    } else {
        printf("Choix invalide.\n");
    }
}

// etat des equipes
void afficherEtatEquipes(Equipe equipe1, Equipe equipe2) {
    printf("\n--- État des équipes ---\n");

    // Équipe 1
    printf("Équipe 1 : %s\n", equipe1.nom);
    for (int i = 0; i < equipe1.taille; i++) {
        afficherBarreVie(equipe1.combattants[i]);
    }

    // Équipe 2
    printf("Équipe 2 : %s\n", equipe2.nom);
    for (int i = 0; i < equipe2.taille; i++) {
        afficherBarreVie(equipe2.combattants[i]);
    }
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

    // Effectuer l'attaque
    attaquer(&equipeIA->combattants[0], &equipeJoueur->combattants[cibleIndex]);
}
// APPLICATION ATTAQUE DE L'IA
void appliquerTechnique(Technique *technique, Combattant *cible) {
    if (technique->duree_effet > 0) {
        cible->attaque += technique->valeur;  // Exemple : boost d'attaque
        printf("%s subit l'effet de %s pour %d tours.\n", cible->nom, technique->nom, technique->duree_effet);
    }
}

printf("%s utilise %s !\n", attaquant->nom, technique->nom);
appliquerTechnique(technique, cible);

// GESTION EFFET TEMPORAIRE
void majEffets(Combattant *cible) {
    for (int i = 0; i < 3; i++) {  // Maximum 3 effets actifs par combattant
        if (cible->effets[i].tours_restants > 0) {
            cible->effets[i].tours_restants--;

            if (cible->effets[i].tours_restants == 0) {
                printf("L'effet %s sur %s a expiré.\n", cible->effets[i].nom, cible->nom);
                cible->attaque -= cible->effets[i].valeur;  // Retirer l'effet
            }
        }
    }
}

//BARRE DE VIE
void afficherBarreVie(Combattant c) {
    printf("%s [", c.nom);

    // Couleur selon le pourcentage de vie
    int pourcentage = (c.pv_courants * 100) / c.pv_max;
    if (pourcentage > 50) {
        printf("\033[1;32m");  // Vert si PV > 50%
    } else if (pourcentage > 20) {
        printf("\033[1;33m");  // Jaune si 20% < PV <= 50%
    } else {
        printf("\033[1;31m");  // Rouge si PV <= 20%
    }

    // Dessiner la barre
    for (int i = 0; i < c.pv_courants / 5; i++) printf("#");
    printf("\033[0m");  // Réinitialiser les couleurs

    printf("] (%d/%d PV)\n", c.pv_courants, c.pv_max);
}













int main() {
    printf("\033[1;31m--- Combat CY-Fighters ---\033[0m\n");
    int choix;
    do {
        afficherMenu();
        printf("Choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                printf("Attaque sélectionnée !\n");
                break;
            case 2:
                printf("Technique spéciale en cours...\n");
                break;
            case 3:
                printf("Affichage des combattants...\n");
                break;
            case 4:
                printf("Fin du combat !\n");
                break;
            default:
                printf("Choix invalide, essaie encore.\n");
        }
    } while (choix != 4);
    
    Combattant ordreCombat[10]; // Exemple : Max 10 combattants au total
int tailleOrdre = 0;

// Organiser les tours avant de commencer le combat
organiserTours(&equipe1, &equipe2, ordreCombat, &tailleOrdre);
printf("Ordre de combat établi !\n");
    
// Initialisation des équipes et des combattants
    Equipe equipe1 = {"Équipe 1", {{"Naruto", 120, 120, 18, 12, 20, 15, 3}}, 1};
    Equipe equipe2 = {"Équipe 2", {{"Sasuke", 100, 100, 20, 10, 15, 12, 2}}, 1};

    Combattant ordreCombat[10];
    int tailleOrdre = 0;

    // Organiser les tours
    organiserTours(&equipe1, &equipe2, ordreCombat, &tailleOrdre);

    // Boucle principale de combat
    int combatEnCours = 1;
    while (combatEnCours) {
        afficherEtatEquipes(equipe1, equipe2);

        for (int i = 0; i < tailleOrdre; i++) {
            if (ordreCombat[i].pv_courants > 0) {
                effectuerTour(&ordreCombat[i], &equipe2);
            }
        }

        // Vérifie si une équipe est éliminée
        combatEnCours = (equipe1.taille > 0 && equipe2.taille > 0);
    }

    printf("Le combat est terminé !\n");

    return 0;
}












