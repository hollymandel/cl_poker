#include "all_in_equity.h"
#include "headsup.h"
#include "random_player.h"
 
int BUYIN = 100;

int main(){
	srand(time(NULL));

	Hand my_pocket = random_hand(3);

	std::cout << "\nWelcome to command-line heads-up poker.";

	std::vector<Player *> plays;
	std::vector<Hand> vill_hands = random_table(2, Hand());

	Hero h = Hero("hero", BUYIN, vill_hands[0]);
	plays.push_back(&h);
	Calling_Station g1 = Calling_Station("CS1", BUYIN, vill_hands[1]);
	plays.push_back(&g1);

	Heads_Up(plays, 1, 1);
}