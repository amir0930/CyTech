#include <stdio.h>
#include "structures.h"
#include "techniques.h"

void utiliserTechnique(Combattant *attaquant, Combattant *cible, Technique *technique) {
    if (technique->recharge > 0) {
        printf("%s ne peut pas encore utiliser %s (Recharge : %d tours).\n", 
               attaquant->nom, technique->nom, technique->recharge);
        return;
    }

    printf("%s utilise %s sur %s !\n", attaquant->nom, technique->nom, cible->nom);
    appliquerEffet(cible, technique);

    // Réinitialiser le temps de recharge
    technique->recharge = 3;  // Par exemple, nécessite 3 tours pour réutilisation
}

void majRecharge(Technique *technique) {
    if (technique->recharge > 0) {
        technique->recharge--;
    }
}

