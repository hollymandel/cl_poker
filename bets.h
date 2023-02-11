#ifndef __bets__
#define __bets__

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <set>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <string>
#include "random_player.h"

int N_lb_buyin = 100;

class Attrition{
public:
	Attrition(std::vector<Player *> ps){
		for(auto itr = ps.begin(); itr != ps.end(); itr++){
			if((*itr)->check_alive()){std::cout << (*itr)->get_name() << " wins!\n\n\n\n";}
		}
	}
};

class Round{
private:
	std::vector<Player *> players;
	int N_players;
	int N_alive;
	int pot;
	int action;
	int little_blind;
	Hand table;
	Hand exclude;
public:
	Round(std::vector<Player *> ps, int lb, int dealer){
		players = ps;
		Hand exclude = Hand();
		for(auto itr = ps.begin(); itr != ps.end(); itr++){
			exclude = exclude.append((*itr)->get_draw());
		}

		N_players = players.size();
		N_alive = N_players;
		action = dealer % N_players;
		little_blind = lb;
		//pot = lb * 3;
		pot = 100;

		try{betting_round();}
		catch(Attrition){return;}

		Hand flop = random_hand(3, exclude);
		table = table.append(flop);
		exclude = exclude.append(flop);

		try{betting_round();}
		catch(Attrition){return;}

		Hand turn = random_hand(1, exclude);
		table = table.append(turn);
		exclude = exclude.append(turn);

		try{betting_round();}
		catch(Attrition){return;}

		Hand river = random_hand(1, exclude);
		table = table.append(river);
		exclude = exclude.append(river);

		try{betting_round();}
		catch(Attrition){return;}

		std::vector<std::string> winners = table_winner();
		std::cout << "The winner is ";
		for(auto itr = winners.begin(); itr != winners.end(); itr++){std::cout << *itr;}		
	}
	void betting_round();
	void best_hand();
	std::vector<std::string> table_winner();
};

std::vector<std::string> Round::table_winner(){
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

void Round::betting_round(){
	int N_calls = 0;
	int bet = 0;
	int decision = 0;
	while(N_calls <= N_alive){
		std::cout << "\n\n\n\ntable: " << table << " - action: " << action << " - pot size: " 
			<< pot << " - bet size: " << bet << " - calls: " << N_calls << "\n\n";
		decision = players[action]->act(players[action]->get_draw(), table, pot, bet, N_players);
		pot += decision;
		if(decision == 0){
			N_alive--;
			if(N_alive == 1){throw Attrition(players);}
		}
		if(decision == bet){N_calls++;}
		if(decision > bet){N_calls = 0;}
		action = (action + 1) % N_players;
	}
}


#endif