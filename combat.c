void attaquer(Combattant *attaquant, Combattant *cible) {
    int degats = attaquant->attaque - cible->defense;
    if (degats > 0) {
        cible->pv_courants -= degats;
        printf("%s attaque %s et inflige %d dégâts !\n", attaquant->nom, cible->nom, degats);
    } else {
        printf("%s attaque %s mais l'attaque est trop faible !\n", attaquant->nom, cible->nom);
    }
}

void organiserTours(Equipe *equipe1, Equipe *equipe2, Combattant ordreCombat[], int *taille);

void majEffetsActifs(Combattant *cible);

void utiliserTechnique(Combattant *attaquant, Combattant *cible, Technique *technique);
void majRecharge(Technique *technique);

int choisirCibleAleatoire(Equipe adversaire);
void attaqueIA(Equipe *equipeIA, Equipe *equipeJoueur, int niveau);

void afficherBarreVieAvecEffets(Combattant *combattant);
