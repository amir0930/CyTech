// combat.h
#ifndef COMBAT_H
#define COMBAT_H

#include "structures.h"  // Définitions de Combattant, Equipe, Effet

/**
 * Attaque classique : calcule les dégâts et les applique.
 */
void attaquer(Combattant *attaquant, Combattant *cible);

/**
 * Construit l'ordre de passage des tours selon la vitesse de chaque combattant.
 * @param e1     Pointeur vers la première équipe
 * @param e2     Pointeur vers la seconde équipe
 * @param ordre  Tableau préalloué pour recevoir l'ordre de Combatants
 * @param n      Pointeur sur la taille résultante du tableau
 */
void organiserTours(Equipe *e1, Equipe *e2, Combattant ordre[], int *n);

/**
 * Met à jour et expire les effets (buffs/debuffs) temporaires sur un combattant.
 */
void majEffetsActifs(Combattant *c);

/**
 * Sélectionne une cible aléatoire dans l'équipe adverse.
 * @return indice du combattant ciblé
 */
int choisirCibleAleatoire(Equipe *adv);

/**
 * Comportement de l'IA : attaque ou utilise une technique suivant le niveau.
 * @param ia      Équipe contrôlée par l'IA
 * @param joueur  Équipe adverse
 * @param niveau  Niveau de difficulté (1=noob, 2=facile, 3=moyen)
 */
void attaqueIA(Equipe *ia, Equipe *joueur, int niveau);

/**
 * Affiche la barre de vie d'un combattant avec ses effets actifs colorés.
 */
void afficherBarreVieAvecEffets(Combattant *c);

#endif // COMBAT_H
