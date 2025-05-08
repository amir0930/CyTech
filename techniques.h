#ifndef TECHNIQUES_H
#define TECHNIQUES_H

#include "structures.h"

void appliquerEffet(Combattant *cible, Technique *tech);
void utiliserTechnique(Combattant *attaquant, Combattant *cible, Technique *tech);
void majRecharge(Technique *tech);

#endif // TECHNIQUES_H

