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
    
    return 0;
}












