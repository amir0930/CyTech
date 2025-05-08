#ifndef COMBAT_H
#define COMBAT_H

#include "structures.h"

void attaquer(Combattant *attaquant, Combattant *cible);
void organiserTours(Equipe *e1, Equipe *e2, Combattant ordre[], int *n);
void majEffetsActifs(Combattant *c);
int choisirCibleAleatoire(Equipe *adv);
void attaqueIA(Equipe *ia, Equipe *joueur, int niveau);
void afficherBarreVieAvecEffets(Combattant *c);

#endif // COMBAT_H

