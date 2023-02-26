#include "all_in_equity.h"
#include "headsup.h"
#include "random_player.h"
 
int BUYIN = 100;

int main(){
	srand(time(NULL));
	Hand my_hand = Hand({11,12,13,14,2,9,9},{0,1,0,2,0,3,0});
	std::cout << straight(my_hand);
	
	//std::vector<Tuple> tups = tuples(my_hand);
	//for(auto itr = tups.begin(); itr != tups.end(); itr++){
	//	std::cout << itr->mult << itr->value; 
	//}
	//std::cout << equity(my_hand, Hand(), 1);
	//Hand my_pocket = random_hand(3);

	//std::vector<Player *> plays;
	//std::vector<Hand> vill_hands = random_table(2, Hand());

	//Hero h = Hero("hero", BUYIN, vill_hands[0]);
	//plays.push_back(&h);
	//Calling_Station g1 = Calling_Station("CS1", BUYIN, vill_hands[1]);
	//plays.push_back(&g1);

	//Heads_Up hu = Heads_Up(plays, 1, 1);
	//hu.play();
}