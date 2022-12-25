#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <set>
#include <vector>


//static const int num_cards = 7; // < 10 please

bool is_card(int v, char s){
	if (v < 2 || v > 14) return false;
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
	std::set<Card> cc;
	std::set<int> num_set;
	std::set<char> suit_set;
public:
	class InvalidHand{};
	Hand(std::vector<Card> cs)
	{
		for (auto itr=cs.begin(); itr < cs.end(); itr++){
			if (cc.count(*itr)) { throw InvalidHand{}; }
			cc.insert(*itr); 
			num_set.insert(itr->get_value());
			suit_set.insert(itr->get_suit());
		}
	}
	Hand(std::vector<int> v, std::vector<char> c)
	{
		int num_cards = v.size();
		assert(c.size() == num_cards);
		for (int i=0; i < num_cards; i++) {
			Card this_card(v[i],c[i]);
			if (cc.count(this_card)) { throw InvalidHand{}; } 
			cc.insert(this_card); 
			num_set.insert(this_card.get_value());
			suit_set.insert(this_card.get_suit());
		}
	}
	Card get_card(int i) const{ 
		return *std::next(cc.begin(), i); 
	}
	const std::set<Card> *get_set() const{return &cc;}
	const std::set<int> *get_num_set() const{return &num_set;}
	const std::set<char> *get_suit_set() const{return &suit_set;}
	const std::vector<char> best_hand() const; 
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

int straight(const Hand h) {
	std::set<int> nums;
	std::set<int>::iterator num_itr;
	std::set<Card>::iterator itr;
	for (itr = h.get_set()->begin(); itr != h.get_set()->end(); itr++){
		nums.insert(itr->get_value());
	}
	int run_len = 1;
	int last_int = 0;
	for (num_itr = nums.begin(); num_itr != nums.end(); num_itr++){
		if(*num_itr == last_int + 1){
			run_len++; last_int++;
		}
		else{
			if(run_len > 4){return last_int;}
			run_len=1; last_int = *num_itr;
		}
		
	}
	if(run_len > 4){return last_int;}
	return 0;
}

std::vector<int> tuples(const Hand h) {
	std::vector<int> nums;
	for (auto itr = h.get_set()->begin(); itr != h.get_set()->end(); itr++){
		nums.push_back(itr->get_value());
	}
	std::vector<int> tups;
	for (auto num_itr = h.get_num_set()->begin(); num_itr != h.get_num_set()->end(); num_itr++){
		tups.push_back(std::count(nums.begin(), nums.end(), *num_itr));
	}
	return tups;
}

const std::vector<char> Hand::best_hand() const{
	const char get_flush = flush(*this);
	if(get_flush != 'n'){
		std::vector<Card> flush_hand;
		for (auto itr = this->get_set()->begin(); itr != this->get_set()->end(); itr++){
			if (itr->get_suit() == get_flush){flush_hand.push_back(*itr);};
			const int straight_flush = straight(flush_hand);
			if (straight_flush){ return {'a'}; }
		}
	}
		
		
	std::vector<char> v = {'a','b'};
	return v;
}

int main(){
	std::vector<int> a {10,11,12,13,14,2,3};
	std::vector<char> b {'c','c','c','c','c','d','d'};
	Hand x = Hand(a,b);
	std::cout << x.best_hand()[0];
}
