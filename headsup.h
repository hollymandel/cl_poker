#ifndef __heads_up__
#define __heads_up__

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <cstdlib>
#include <string>
#include "random_player.h"

int N_players = 2;
int N_lb_buyin = 100;

class Heads_Up{
private:
	int pot;
	int dealer;
	int little_blind;
	bool all_in;
	bool verbose;
	Hand table;
	Hand exclude;
	std::vector<Player *> players;
public:
	Heads_Up(std::vector<Player *> ps, int lb, int dlr, bool vbs = true){
		assert(ps.size() == N_players);
		pot = 0;
		players = ps;
		dealer = dlr % N_players;
		Hand exclude = Hand();
		little_blind = lb;
		all_in = false;
		verbose = vbs;
	}

	std::vector<int> play(){
		for(int i = 0; i < N_players; i++){
			exclude = exclude.append(players[i]->get_draw());
			pot += players[i]->ante((i + dealer + 1) % N_players, little_blind);
		}
		
		std::vector<int> deal_sizes = {0,3,1,1};
		std::vector<int> live_blind_enabled = {true,false,false,false};
		for(int i = 0; i != 4; i++){
			Hand this_deal = random_hand(deal_sizes[i], exclude);
			table = table.append(this_deal);
			exclude = exclude.append(this_deal);
			if(!all_in){
				int state = betting_round(live_blind_enabled[i]);
				if(state == -1){return table_winner();}
				if(state == -2){all_in = true;}
			}
			clear_all_bets();
		}
		return table_winner();
	}

	int betting_round(int lr = 0);
	std::vector<int> table_winner();
	void clear_all_bets(){for(auto itr = players.begin(); itr != players.end(); itr++){(*itr)->clear_bet();}}
	void print_table(){std::cout << "\n\n" << table << "\n\n";}
	void print_state(int action, int total_bet, int call_amt){
			std::cout << "action: " << players[action]->get_name() 
				<< " - pot size: " << pot 
				<< " - total round size: " << total_bet 
				<< " - call amount: " << call_amt
				<< "\n\n";
	}
};

std::vector<int> Heads_Up::table_winner(){
	std::string best_hand = "Z";
	std::vector<int> winner;
	for(int i = 0; i < N_players; i++){
		if(players[i]->check_alive()){
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

int Heads_Up::betting_round(int live_blind_enabled){
	if(verbose){print_table();}
	int total_bet; int call_amt;
	if(live_blind_enabled){total_bet = 2*little_blind; call_amt = little_blind;}
	else{total_bet = 0; call_amt = 0;}
	int last_raise = call_amt; // heads-up only
	int decision;
	int action = (dealer + 1) % N_players;
	while(true){
		if(players[action]->check_all_in()){return -2;}
		call_amt = total_bet - players[action]->get_in_this_round();
		if(verbose){print_state(action, total_bet, call_amt);}
		decision = players[action]->act(players[action]->get_draw(), table, pot, total_bet, last_raise, N_players-1);
		if(verbose){std::cout << "\n\nPlayer " << players[action]->get_name() << " bet " << decision << ".\n\n";}
		if(decision == call_amt){
			if(live_blind_enabled & (action != dealer)){live_blind_enabled = false;}
			else{pot += call_amt; return 0;} // call ends round in heads-up 
		}
		if(decision == -1){return -1;} // fold
		pot += decision;
		total_bet += decision - call_amt;
		last_raise = decision - call_amt;
		action = (action + 1) % N_players;
	}
	return 0;
}


#endif