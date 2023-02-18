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

class Attrition{};

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
		Hand this_deal;

		int loop_variable = (dealer + 1) % N_players;
		for(auto itr = ps.begin(); itr != ps.end(); itr++){
			exclude = exclude.append((*itr)->get_draw());
			pot += (*itr)->ante(loop_variable, little_blind);
			loop_variable = (loop_variable+1) % N_players;
		}
		
		std::vector<int> deal_sizes = {0,3,1,1};
		std::vector<int> lrs = {lb,0,0,0};
		for(int i = 0; i != 4; i++){
			this_deal = random_hand(deal_sizes[i], exclude);
			table = table.append(this_deal);
			exclude = exclude.append(this_deal);

			if(!all_in){
				try{betting_round(lrs[i]);}
				catch(All_In){all_in = true;}
				catch(Attrition){break;}
			}
			clear_all_bets();
		}
		std::vector<std::string> winners = table_winner();
		return winners;		
	}
	void betting_round(int lr = 0);
	void best_hand();
	std::vector<std::string> table_winner();
	void clear_all_bets(){for(auto itr = players.begin(); itr != players.end(); itr++){(*itr)->clear_bet();}}
};


std::vector<int> Heads_Up::table_winner(){
	std::string best_hand = "Z";
	std::vector<int> winner;
	for(int i = 0; i < N_players; i++){
		if(players[i]->alive){
			Hand this_hand = table.append(players[i]->get_draw());
			std::string this_best = this_hand.best_hand();
			if(this_best.compare(best_hand) > 0){
				winner =  { i };
				best_hand = this_best;
			}
			if(this_best.compare(best_hand) == 0){
				winner.push_back(i);
			}
		}
	}	
	return winner;
}

void Heads_Up::betting_round(int lr){
	
	int total_bet = 2*lr;
	int last_raise = lr;
	int call_amt = lr;
	int decision = 0;
	int action = (dealer + 1) % N_players;
	while(true){
		call_amt = total_bet - players[action]->get_in_this_round();
		std::cout << "\n\n\n\ntable:\n " << table << "\n\n\n";
		std::cout << "action: " << action << " - pot size: " << pot << " - total round size: " << total_bet << "\n\n" << 
			"- call amount: " << call_amt << " - minimum raise " << call_amt + last_raise << "\n\n";
		decision = players[action]->act(players[action]->get_draw(), table, pot, total_bet, last_raise, N_players-1);
		pot += decision;
		total_bet += decision;
		if(decision == 0){break;}
		last_raise = decision-last_raise;
		action = (action + 1) % N_players;
	}
}


#endif