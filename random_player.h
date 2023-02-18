#ifndef __players__
#define __players__

#include "cards.h"
#include "headsup.h"
#include "all_in_equity.h"
#include <math.h> 

struct InvalidBet{};
void valid_bet(int bet, int in_this_round, int total_bet, int last_raise){
	assert(last_raise >= 0);
	if(bet == 0){return;}
	if(bet + in_this_round < total_bet){throw InvalidBet();}
	if(last_raise == 0){return;}
	if(bet < last_raise){throw InvalidBet();}
}

struct All_In{};
struct Fold{};

class Player{
protected: 
	Hand draw;
	int stack;
	int in_this_round;
	bool alive;
	bool all_in;
	std::string name;
public: 
	Player(std::string nm, int buyin, Hand in_draw){
		name = nm;
		stack = buyin; 
		alive = true; 
		all_in = false;
		draw = in_draw;
		in_this_round = 0;
	}
	int act(Hand draw, Hand table, int pot, int total_bet, int last_raise, int n){
		if(!alive){return 0;}
		if(all_in){throw All_In();}
		int decision = decide(draw, table, pot, total_bet, last_raise, n);
		valid_bet(decision, in_this_round, total_bet, last_raise);
		if(decision == 0){throw Fold();}
		return stay_in(decision);

	}	
    	virtual int decide(Hand draw, Hand table, int pot, int total_bet, int last_raise, int n){ return 0; }
	int stay_in(int bet){
		assert(bet <= stack);
		if(bet == stack){all_in = true;}
		stack -= bet;
		return bet;
	}
	int fold(){
		alive = false;
		return 0;
	}
	Hand get_draw(){ return draw; }
	std::string get_name(){ return name; }
	bool check_alive(){ return alive; }
	bool check_all_in(){ return all_in; }	
	int ante(int position, int lb){
		if(position == 0){stack -= lb; return lb;}
		if(position == 1){stack -= 2*lb; return 2*lb;}
		return 0;
	}	
};

class Hero : public Player{
public: 
	using Player::Player;
	int decide(Hand draw, Hand table, int pot, int total_bet, int last_raise, int n){
		int decision;
		try{
			std::cout << "How much would you like to bet?\n\n";
			std::cin >> decision;
			valid_bet(decision, in_this_round, total_bet, last_raise);
		}
		catch(InvalidBet){
			std::cout << "Invalid bet.\n\n";
			return decide(draw, table, pot, total_bet, last_raise, n);
		}
		return decision;
	}
};

class Goldfish : public Player{
public:
	using Player::Player;
	int decide(Hand draw, Hand table, int pot, int total_bet, int last_raise, int n){
		int decision = 0;
		double get_equity = equity(draw, table, n);
		float pot_odds = last_raise / (pot + last_raise);
		if(pot_odds < get_equity){
			if(2*pot_odds < get_equity){
				decision = stay_in(floor(get_equity * pot / (1-get_equity)));
			}
			return decision;
		}
		return 0;
	}
	
};

#endif //__players__