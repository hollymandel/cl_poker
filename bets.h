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
		for(int i = 0; i != ps.size(); i++){
			if(ps[i]->check_alive()){std::cout << "Player " << i << " wins!\n\n\n\n";}
		}
	}
};

class Chips{
private:
	std::vector<int> stacks;
	int pot;
public:
	Chips(int N_Players, int N_lb_buyin){
		for(int i = 0; i != N_Players + 1; i++){stacks.push_back(N_lb_buyin);}
		pot = 0;
	}
	void bet(int player, int n_chips){
		if(n_chips > stacks[player]){
			std::cout << "Invalid bet: insufficient chips.\n";
			return;
		}
		stacks[player] -= n_chips;
		pot += n_chips;
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
		pot = lb * 3;

		try{std::cout << betting_round();}
		catch(Attrition){return;}

		Hand flop = random_hand(3, exclude);
	}
	int attrition();
	int betting_round();
	void pre_flop();
	//void flop();
	//void turn();
	//void river();
};


int Round::betting_round(){
	int N_calls = 0;
	int bet = 2*little_blind;
	int decision = 0;
	while(N_calls <= N_alive){
		std::cout << "\n\n\n\naction: " << action << " - pot size: " 
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
	return 0;
}

#endif