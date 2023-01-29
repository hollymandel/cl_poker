#ifndef __equity__
#define __equity__

#include "cards.h"

float game(Hand draw, Hand table, int n_villains, bool loud = true){
	Hand exclude = table.append(draw);
	int table_remainder_size = TABLE_SIZE - table.get_set()->size();
	table = table.append(random_hand(table_remainder_size, exclude));

	Hand hero_hand = table.append(draw);
	std::string hero_bh = hero_hand.best_hand();

	if(loud){
		std::cout << "Hero has " << draw << "\n";
		std::cout << "Table is: " << table << "\n";
		std::cout << "Hero's best hand is: " << hero_bh << "\n";
	}

	float hero_villain_tie = 0.0;

	std::vector<Hand> villain_draws = random_table(n_villains, hero_hand);
	for(auto itr = villain_draws.begin(); itr != villain_draws.end(); itr++){
		if(loud){
			std::cout << "villain draw is " << *itr << "\n";
		}

		Hand get_villain_hand = table.append(*itr);
		std::string vill_bh = get_villain_hand.best_hand();

		if(loud){
			std::cout << "villain bh is " << vill_bh << "\n";
		}

		if(vill_bh.compare(hero_bh) > 0){
			return 0.0;
		}
		if(vill_bh.compare(hero_bh) == 0){
			hero_villain_tie = hero_villain_tie + 1;
		}
	}
	if(hero_villain_tie > 0){
		return 1 / (hero_villain_tie+1);
	};
	return 1.0;	
}

double equity(Hand draw, Hand table, int n_villains, int n_samples = 10000){
	double wins = 0;
	for(int i = 0; i != n_samples; i++){
		wins = wins + game(draw, table, n_villains, false);
	}
	return wins / n_samples;
}

#endif //__equity__
