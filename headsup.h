#ifndef __heads_up__
#define __heads_up__

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <set>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <string>
#include "random_player.h"

int N_players = 2;
int N_lb_buyin = 100;

class Attrition{
public:
	Attrition(std::vector<Player *> ps){
		for(auto itr = ps.begin(); itr != ps.end(); itr++){
			if((*itr)->check_alive()){
				std::cout << (*itr)->get_name() << " wins!\n\n\n\n";
			}
		}
	}
};

class Heads_Up{
private:
	int pot;
	int dealer;
	int little_blind;
	bool all_in;
	Hand table;
	Hand exclude;
	std::vector<Player *> players;
public:

	Heads_Up(std::vector<Player *> ps, int lb, int dlr){
		assert(ps.size() == N_players);

		pot = 0;
		players = ps;
		dealer = dlr % N_players;
		Hand exclude = Hand();
		little_blind = lb;
		all_in = false;

		int loop_variable = dealer + 1;
		for(auto itr = ps.begin(); itr != ps.end(); itr++){
			exclude = exclude.append((*itr)->get_draw());
			pot += (*itr)->ante(loop_variable, little_blind);
			loop_variable = (loop_variable+1) % N_players;
		}

		Hand this_deal;
		std::vector<int> deal_sizes = {0,3,1,1};
		for(auto itr = deal_sizes.begin(); itr != deal_sizes.end(); itr++){
			this_deal = random_hand(*itr, exclude);
			table = table.append(this_deal);
			exclude = exclude.append(this_deal);
			if(!all_in){
				try{betting_round();}
				catch(All_In){all_in = true;}
				catch(Attrition){return;}
			}
		}
		std::vector<std::string> winners = table_winner();
		std::cout << "The winner is ";
		for(auto itr = winners.begin(); itr != winners.end(); itr++){std::cout << *itr << ", ";}		
	}
	void betting_round();
	void best_hand();
	std::vector<std::string> table_winner();
	void showdown();
};


std::vector<std::string> Heads_Up::table_winner(){
	std::string best_hand = "Z";
	std::vector<std::string> winner;
	for(auto itr = players.begin(); itr != players.end(); itr++){
		Hand this_hand = table.append((*itr)->get_draw());
		std::string this_best = this_hand.best_hand();
		if(this_best.compare(best_hand) > 0){
			winner =  { (*itr)->get_name() };
			best_hand = this_best;
		}
		if(this_best.compare(best_hand) == 0){
			winner.push_back((*itr)->get_name());
		}
	}	
	return winner;
}

void Heads_Up::betting_round(){
	
	int total_bet = 0;
	int last_raise = 0;
	int decision = 0;
	int action = (dealer + 1) % N_players;
	while(true){
		std::cout << "\n\n\n\ntable: " << table << " - action: " << action << " - pot size: " 
			<< pot << " - total round size: " << total_bet;
		decision = players[action]->act(players[action]->get_draw(), table, pot, total_bet, last_raise, N_players-1);
		pot += decision;
		if(decision == last_raise){break;}
		last_raise = decision-last_raise;
		action = (action + 1) % N_players;
	}
}


#endif