#include "all_in_equity.h"
#include "bets.h"
#include "random_player.h"
 
int BUYIN = 100;

int main(){
	srand(time(NULL));

	Hand my_pocket = random_hand(3);

	std::cout << "\nWelcome to command-line poker.";

	std::vector<Player *> plays;
	std::vector<Hand> vill_hands = random_table(3, Hand());

	Hero h = Hero("hero", BUYIN, vill_hands[0]);
	plays.push_back(&h);
	Goldfish g1 = Goldfish("fish1", BUYIN, vill_hands[1]);
	plays.push_back(&g1);
	Goldfish g2 = Goldfish("fish2", BUYIN, vill_hands[2]);
	plays.push_back(&g2);

	Round(plays, 1, 0);
}