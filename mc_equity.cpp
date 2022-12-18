#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <set>
#include <map>


bool is_card(int v, char s){
	if (v < 1 || v > 13) return false;
	if (!(s=='s'||s=='h'||s=='d'||s=='c')) return false;
	return true;
}


class Card{
private:
	int value;
	char suit;
public:
	class InvalidCard{};
	Card()
	{
	}
	Card(int v, char s)
	{	
		if (!is_card(v,s)) throw InvalidCard();
		value = v;
		suit = s;
	}
	int get_value() const {return value;}
	char get_suit() const {return suit;}
};

bool operator<=(const Card& a, const Card& b){
	return (a.get_value() <= b.get_value()) 
		|| ((a.get_value() == b.get_value()) && (a.get_suit() < b.get_suit()));
}

bool operator==(const Card& a, const Card& b){
	return (a.get_value() == b.get_value()) && (a.get_suit() == b.get_suit());
}

bool operator <(const Card& a, const Card& b){
	return (a <= b) && !(a ==b);
}


class Hand{
private:
	static const int num_cards = 7; // < 10 please
	std::set<Card> cc;
public:
	class InvalidHand{};
	Hand(int v[num_cards], char c[num_cards])
	{
		for (int i=0; i < num_cards; i++) {
			Card this_card(v[i],c[i]);
			if (cc.count(this_card)) { throw InvalidHand{}; } 
			else { cc.insert(this_card); }
		}
	}
	Card get_card(int i) const{ 
		return *std::next(cc.begin(), i); 
	}
	const std::set<Card> *get_set() const{
		return &cc;
	}
};


char flush(const Hand h) {
	char active_suit = 'c';
	int run_len = 0;
	std::set<Card>::iterator itr;
	for (itr = h.get_set()->begin(); itr != h.get_set()->end(); itr++){
		if(itr->get_suit() == active_suit){run_len++;}
		else{active_suit = itr->get_suit(); int run_len = 1;}
		if (run_len == 5){return active_suit;}
	}
	return 'n';
}
	

//bool operator<=(const Hand& h):
	


int main(){
	int a[] = {10,1,4,5,6,2,3};
	char b[] = "hcchshd";
	Hand x = Hand(a,b);
	for(int i = 0; i < 7; i ++){
		std::cout << x.get_card(i).get_value() << x.get_card(i).get_suit() << "\n";
	};
	std::cout << flush(x);
}
