#ifndef __players__
#define __players__

#include "cards.h"
#include "bets.h"
#include "all_in_equity.h"
#include <math.h> 

struct InvalidBet{};
void valid_bet(int bet, int prev_bet = 0){
	assert(prev_bet >= 0);
	if(bet == 0){return;}
	if(bet < prev_bet){throw InvalidBet();}
	if(bet % prev_bet != 0){throw InvalidBet();}
}


class Player{
protected: 
	Hand draw;
	int stack;
	bool alive;
public: 
	Player(int buyin, Hand in_draw){
		stack = buyin; 
		alive = true; 
		draw = in_draw;
	}

    	virtual int act(Hand draw, Hand table, int pot, int bet, int n){ return 0; }
	int stay_in(int bet){
		stack -= bet;
		return bet;
	}
	int fold(){
		alive = false;
		return 0;
	}
	Hand get_draw(){ return draw; }
	bool check_alive(){ return alive; }
		
};

class Hero : public Player{
public: 
	using Player::Player;
	int act(Hand draw, Hand table, int pot, int bet, int n){
		if(!alive){return 0;}
		int decision;
		try{
			std::cout << "How much would you like to bet?\n\n";
			std::cin >> decision;
			valid_bet(decision, bet);
		}
		catch(InvalidBet){
			std::cout << "Invalid bet.\n\n";
			return act(draw, table, pot, bet, n);
		}
		if(decision == 0){return fold();}
		return stay_in(decision);
	}
};

class Goldfish : public Player{
public:
	using Player::Player;
	int act(Hand draw, Hand table, int pot, int bet, int n){
		if(!alive){return 0;}
		double get_equity = equity(draw, table, n);
		if(bet / (pot + bet) < get_equity){
			if(2*bet / (pot+bet) < get_equity){
				//return raise(bet, floor(get_equity * pot / (1-get_equity))-bet);
			}
			//return call(bet);
		}
		return fold();
	}
	
};

class Random_Player : public Player{
	int act(Hand draw, Hand table, int pot, int bet){
		return fold();
	}
};

#endif //__players__