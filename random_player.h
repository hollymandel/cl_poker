#include "all_in_equity.h"
#include <math.h> 

class Villain{
private: 
	int stack;
public: 
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
	void act(Hand draw, Hand table, int pot, int bet){
		double get_equity = equity(draw, table);
		if(bet / (pot + bet) > get_equity){
			if(2*bet / (pot+bet) > get_equity){
				raise(bet, round(get_equity * pot / (1-get_equity));
				return;
			}
			
		}
	}
	
}

class Random_Player{
private:
	int stack;
public:
	Random_Player(int buy_in){stack = buy_in;}
	bool call(){
		if((rand() % 2)	> 0){return true;}
		return false;
	}
	int bet(){
		int bet_size = rand() % stack;
		stack -= bet_size;
		return bet_size;
	}
	int get_stack(){return stack;}
};

