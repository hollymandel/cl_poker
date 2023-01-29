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

void round(int N_villains, int lb_position){
	int stretch = 0;
	int actor = lb_position;
	std::vector<bool> fold(N_villains+1, false);
}

#endif //__bets__