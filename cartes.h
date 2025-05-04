// cartes.h
#ifndef CARTES_H
#define CARTES_H

#include "structures.h"  // Pour Combattant et Effet

/**
 * Représentation d'une carte de jeu.
 * @param nom           Nom de la carte
 * @param description   Description textuelle de l'effet
 * @param effet_valeur  Valeur de l'effet (dégâts, soin, buff, etc.)
 * @param duree         Durée de l'effet en tours (0 pour effet instantané)
 * @param type          Type de la carte : "Offensive", "Buff", "Heal", etc.
 */
typedef struct {
    char nom[50];
    char description[100];
    int effet_valeur;
    int duree;
    char type[20];
} Carte;

/**
 * Joue une carte sur une cible en appliquant l'effet approprié.
 * @param carte  Pointeur vers la carte jouée
 * @param cible  Pointeur vers le combattant cible
 */
void utiliserCarte(const Carte *carte, Combattant *cible);

/**
 * Charge depuis un fichier CSV (format : nom;description;effet_valeur;duree;type)
 * la liste des cartes disponibles.
 * @param nomFichier  Chemin vers le fichier CSV
 * @param liste       Tableau préalloué pour stocker les cartes
 * @param taille      Nombre de cartes lues (résultat)
 */
void chargerCartes(const char *nomFichier, Carte liste[], int *taille);

/**
 * Affiche la liste des cartes disponibles avec leurs caractéristiques.
 * @param cartes  Tableau de cartes
 * @param taille  Nombre de cartes dans le tableau
 */
void afficherCartesDisponibles(const Carte
