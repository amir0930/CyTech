// techniques.c
#include <stdio.h>
#include <string.h>
#include "techniques.h"

void appliquerEffet(Combattant *cible, Technique *tech) {
    switch (tech->type) {
        case TECH_DAMAGE:
            cible->pv_courants -= tech->value;
            printf("%s subit %d dégâts.\n", cible->nom, tech->value);
            break;
        case TECH_HEAL:
            cible->pv_courants = (cible->pv_courants + tech->value > cible->pv_max)
                ? cible->pv_max
                : cible->pv_courants + tech->value;
            printf("%s récupère %d PV.\n", cible->nom, tech->value);
            break;
        case TECH_BUFF:
            cible->attaque += tech->value;
            printf("%s gagne +%d d'attaque pour %d tours.\n",
                   cible->nom, tech->value, tech->cooldown);
            break;
        case TECH_DEBUFF:
            cible->defense -= tech->value;
            printf("%s perd %d de défense pour %d tours.\n",
                   cible->nom, tech->value, tech->cooldown);
            break;
    }
}

void utiliserTechnique(Combattant *attaquant, Combattant *cible, Technique *tech) {
    if (tech->current_cd > 0) {
        printf("%s ne peut pas utiliser %s (CD restant : %d tours).\n",
               attaquant->nom, tech->nom, tech->current_cd);
        return;
    }
    printf("%s utilise %s sur %s !\n",
           attaquant->nom, tech->nom, cible->nom);
    appliquerEffet(cible, tech);
    tech->current_cd = tech->cooldown;
}

void majRecharge(Technique *tech) {
    if (tech->current_cd > 0) {
        tech->current_cd--;
    }
}
