#include <stdio.h>
#include <string.h>
#include "techniques.h"
//applique l’effet d’une technique (tech) sur un combattant (cible).
void appliquerEffet(Combattant *cible, Technique *tech) {
    switch (tech->type) {
        case TECH_DAMAGE:
            cible->pv_courants -= tech->value;
            if (cible->pv_courants < 0) cible->pv_courants = 0;
            printf("%s subit %d degats.\n", cible->nom, tech->value);
            break;
        case TECH_HEAL:
            cible->pv_courants += tech->value;
            if (cible->pv_courants > cible->pv_max)
                cible->pv_courants = cible->pv_max;
            printf("%s recupere %d PV.\n", cible->nom, tech->value);
            break;
        case TECH_BUFF:
            cible->attaque += tech->value;
            printf("%s gagne +%d ATK pour %d tours.\n",
                   cible->nom, tech->value, tech->cooldown);
            break;
        case TECH_DEBUFF:
            cible->defense -= tech->value;
            printf("%s perd %d DEF pour %d tours.\n",
                   cible->nom, tech->value, tech->cooldown);
            break;
    }
}
//permet à un combattant (attaquant) d'utiliser une technique sur une cible (cible), en tenant compte du cooldown.
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
//met à jour le cooldown d’une technique s’il n’est pas à 0.
// decremente de 1
void majRecharge(Technique *tech) {
    if (tech->current_cd > 0) tech->current_cd--;
}

