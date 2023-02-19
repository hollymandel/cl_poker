#ifndef __players__
#define __players__

#include "cards.h"
#include "headsup.h"
#include "all_in_equity.h"
#include <math.h> 

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
		assert(alive & !all_in);
		int decision = decide(draw, table, pot, total_bet, last_raise, n);
		valid_bet(decision, total_bet, last_raise);
		if(decision == 0){
			if(in_this_round==total_bet){return decision;} // call
			alive = false;
			return -1; // fold
		}
		if(decision == stack){all_in = true;}
		stack -= decision;
		in_this_round += decision;
		return decision;
	}	
    virtual int decide(Hand draw, Hand table, int pot, int total_bet, int last_raise, int n){ return 0; }
	Hand get_draw(){ return draw; }
	std::string get_name(){ return name; }
	bool check_alive(){ return alive; }
	bool check_all_in(){ return all_in; }
	int get_in_this_round(){ return in_this_round; }	
	int ante(int position, int lb){
		if(position == 0){stack -= lb; in_this_round += lb; return lb;}
		if(position == 1){stack -= 2*lb; in_this_round += 2*lb; return 2*lb;}
		return 0;
	}
	void clear_bet(){ in_this_round = 0; }
	struct InvalidBet{};
	void valid_bet(int bet, int total_bet, int last_raise){
		assert(last_raise >= 0);
		if(bet == 0){return;}
		if(bet + in_this_round < total_bet){throw InvalidBet();}
		if((bet + in_this_round) == total_bet){return;}
		if((bet+in_this_round-total_bet) < last_raise){throw InvalidBet();}
	}
};

class Hero : public Player{
public: 
	using Player::Player;
	int decide(Hand draw, Hand table, int pot, int total_bet, int last_raise, int n){
		std::cout << "Your hand is\n" << draw << "\n\n";
		std::cout << "Your equity is " << equity(draw, table, n) << "\n\n";
		int decision;
		try{
			std::cout << "How much would you like to bet?\n\n";
			std::cin >> decision;
			valid_bet(decision, total_bet, last_raise);
		}
		catch(InvalidBet){
			std::cout << "Invalid bet.\n\n";
			return decide(draw, table, pot, total_bet, last_raise, n);
		}
		return decision;
	}
};

class Calling_Station : public Player{
public:
	using Player::Player;
	int decide(Hand draw, Hand table, int pot, int total_bet, int last_raise, int n){
		return total_bet - in_this_round;
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
				decision = floor(get_equity * pot / (1-get_equity));
			}
			return decision;
		}
		return 0;
	}
	
};

#endif //__players__