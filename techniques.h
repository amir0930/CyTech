#ifndef TECHNIQUES_H
#define TECHNIQUES_H

#include "structures.h"  // Pour accéder à Combattant, Technique

void utiliserTechnique(Combattant *attaquant, Combattant *cible, Technique *technique);
void majRecharge(Technique *technique);
void appliquerEffet(Combattant *cible, Technique *technique);  // Si applicable

#endif
