#include "all_in_equity.h"
#include "bets.h"
#include "random_player.h"

int N_VILLAINS = 1; 

int main(){
	srand(time(NULL));
	//double random_wins = 0;
	//float N_samples = 100000;
	//for(int i = 0; i < N_samples; i++){
	//	Hand my_pocket = random_hand(2);
	//	random_wins += game(my_pocket, Hand(), N_VILLAINS, false);
	//}
	//std::cout << "ties: " << n_tie << " --- hero wins: " << n_hero << " --- villain wins: " << n_vill << "\n\n";
	//float ratio = (n_hero + n_tie / 2) / ( N_samples);
	//std::cout << random_wins / N_samples;
	Hand my_pocket = random_hand(2);
	std::cout << "\nWelcome to command-line poker. You are the dealer. There are " << N_VILLAINS << " villains.\n\nYou draw:\n" << my_pocket;
	float preflop_equity = equity(my_pocket, Hand(), N_VILLAINS);
	std::cout << "\nYour preflop equity is " << preflop_equity << ".\n";

	//std::vector<Hand> villain_draws = random_table(N_VILLAINS, my_pocket);

	//Random_Player rp = Random_Player(10);
	//std::cout << rp.bet() << rp.get_stack();
}