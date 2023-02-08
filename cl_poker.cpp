#include "all_in_equity.h"
#include "bets.h"
#include "random_player.h"

int N_VILLAINS = 1; 
int BUYIN = 100;

int main(){
	srand(time(NULL));

	Hand my_pocket = random_hand(2);

	std::cout << "\nWelcome to command-line poker. You are the dealer. There are " << N_VILLAINS << " villains.\n\nYou draw:\n" << my_pocket;
	double preflop_equity = equity(my_pocket, Hand(), N_VILLAINS);
	std::cout << "\nYour preflop equity is " << preflop_equity << ".\n";

	std::vector<Villain *> vills;
	std::vector<Hand> vill_hands = random_table(N_VILLAINS, my_pocket);

	Goldfish g = Goldfish(BUYIN, vill_hands[0]);
	vills.push_back(&g);
	vills[0]->act();
	
	//std::cout << "Goldfish has " << vill_hands[0] << "\nVillain has equity " << equity(vill_hands[0], Hand(), 1) << ".\n";
	
	//std::cout << "Villain bets " << g.act(vill_hands[0], Hand(), 20, 2, 1) << ".\n\n\n";
}