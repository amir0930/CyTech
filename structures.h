#ifndef STRUCTURES_H
#define STRUCTURES_H

#define MAX_EFFETS 3
#define MAX_CARTES 100

// Effet temporaire (buff/debuff)
typedef struct {
    char nom[50];
    int valeur;
    int tours_restants;
} Effet;

// Technique spéciale
typedef enum { TECH_DAMAGE, TECH_HEAL, TECH_BUFF, TECH_DEBUFF } TechniqueType;
typedef struct Technique {
    char nom[50];
    TechniqueType type;
    int value;
    int cooldown;     // tours max
    int current_cd;   // tours restants
} Technique;

// Combattant
typedef struct {
    char nom[50];
    int pv_courants;
    int pv_max;
    int attaque;
    int defense;
    int agilite;
    int vitesse;
    int nb_techniques;
    Technique *techniques;
    Effet effets[MAX_EFFETS];
} Combattant;

// Équipe
typedef struct {
    char nom[50];
    Combattant *combattants;
    int taille;
} Equipe;

#endif // STRUCTURES_H

