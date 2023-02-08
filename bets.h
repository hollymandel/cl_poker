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

class Chips{
private:
	std::vector<int> stacks;
	int pot;
public:
	Chips(int N_villains, int N_lb_buyin){
		for(int i = 0; i != N_villains + 1; i++){stacks.push_back(N_lb_buyin);}
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
	std::vector<Villain *> villains;
	int N_villains;
	int pot;
	int action;
	int little_blind;
	Hand table;
public:
	Round(std::vector<Villain *> villains, int little_blind, int dealer){
		villains = villains;
		N_villains = villains.size();
		action = dealer % (N_villains + 1);
		little_blind = little_blind;
	}
	void pre_flop();
	void flop();
	void turn();
	void river();
};

void Round::pre_flop(){
	int N_calls = 0;
	int bet = 0;
	while(N_calls < N_villains){
		if(action==0){
			std::cout << "How much would you like to bet?\n\n";
			std::cin >> bet;
			pot += bet;
		}
		else{
			villains[action]->act();
		}
		N_calls++;
		action = (action + 1) % (N_villains + 1);
	}
}

#endif //__bets__