void attaquer(Combattant *attaquant, Combattant *cible);

void organiserTours(Equipe *equipe1, Equipe *equipe2, Combattant ordreCombat[], int *taille);

void majEffetsActifs(Combattant *cible);

void utiliserTechnique(Combattant *attaquant, Combattant *cible, Technique *technique);
void majRecharge(Technique *technique);

int choisirCibleAleatoire(Equipe adversaire);
void attaqueIA(Equipe *equipeIA, Equipe *equipeJoueur, int niveau);

void afficherBarreVieAvecEffets(Combattant *combattant);

