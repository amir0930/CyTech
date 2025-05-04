// techniques.h
#ifndef TECHNIQUES_H
#define TECHNIQUES_H

#include "structures.h"  // Pour Combattant et Technique

/**
 * Applique l'effet d'une technique sur une cible (dégâts, soin, buff, debuff).
 * @param cible  Pointeur vers le combattant recevant l'effet
 * @param tech   Pointeur vers la technique à appliquer
 */
void appliquerEffet(Combattant *cible, Technique *tech);

/**
 * tente d'utiliser une technique depuis un attaquant sur une cible.
 * Vérifie le cooldown et applique l'effet si possible.
 * @param attaquant  Pointeur vers le combattant lançant la technique
 * @param cible      Pointeur vers le combattant ciblé
 * @param tech       Pointeur vers la technique à utiliser
 */
void utiliserTechnique(Combattant *attaquant, Combattant *cible, Technique *tech);

/**
 * Décrémente le compteur de cooldown d'une technique si nécessaire.
 * @param tech  Pointeur vers la technique à mettre à jour
 */
void majRecharge(Technique *tech);

#endif // TECHNIQUES_H
