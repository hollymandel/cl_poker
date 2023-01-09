#include "cards.h"

float equity(Hand draw, Hand table, int n_villains, int n_samples = 1000){
	float wins = 0;
	float losses = 0;
	for(int i = 0; i != n_samples; i++){
		Hand exclude = table.append(draw);
		int table_remainder_size = TABLE_SIZE - table.get_set()->size();
		table = table.append(random_hand(table_remainder_size, exclude));

		Hand hero_hand = table.append(draw);
		std::string hero_bh = hero_hand.best_hand();

		std::vector<Hand> villain_draws = random_table(n_villains, hero_hand);
		wins++;
		for(auto itr = villain_draws.begin(); itr != villain_draws.end(); itr++){
			Hand get_villain_hand = table.append(*itr);
			if(get_villain_hand.best_hand() > hero_bh){
				losses++;
				wins--;
				break;
			}
		}
	}
	assert((wins + losses) == n_samples);
	return wins / (wins + losses);
}