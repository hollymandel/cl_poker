#include "all_in_equity.h"
#include "bets.h"
#include "random_player.h"
 
int BUYIN = 100;

int main(){
	srand(time(NULL));

	Hand my_pocket = random_hand(2);

	std::cout << "\nWelcome to command-line poker.";

	std::vector<Player *> plays;
	std::vector<Hand> vill_hands = random_table(2, Hand());

	Hero h = Hero(BUYIN, vill_hands[0]);
	plays.push_back(&h);
	Goldfish g = Goldfish(BUYIN, vill_hands[1]);
	plays.push_back(&g);

	Round(plays, 1, 0);
}