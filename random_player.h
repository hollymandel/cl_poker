#ifndef __players__
#define __players__

#include "all_in_equity.h"
#include <math.h> 

class Villain{
private: 
	Hand draw;
	int stack;
	bool alive;
public: 
	Villain(int buyin, Hand draw){stack = buyin; alive = true; draw = draw;}
    	virtual int act(){ return 0; }
	int call(int bet){
		stack -= bet;
		return bet;
	}
	int raise(int previous_bet, int new_bet){
		assert(new_bet >= previous_bet); //this is not correct... 
		stack -= previous_bet + new_bet;
		return previous_bet + new_bet;
	}
	int fold(){
		return 0;
	}
		
};

class Goldfish : public Villain{
public:
	Goldfish(int buyin, Hand draw) : Villain(buyin, draw){};
	int act(Hand draw, Hand table, int pot, int bet, int n){
		double get_equity = equity(draw, table, n);
		if(bet / (pot + bet) < get_equity){
			if(2*bet / (pot+bet) < get_equity){
				return raise(bet, floor(get_equity * pot / (1-get_equity))-bet);
			}
			return call(bet);
		}
		return fold();
	}
	
};

class Random_Player : public Villain{
	int act(Hand draw, Hand table, int pot, int bet){
		return fold();
	}
};

#endif //__players__