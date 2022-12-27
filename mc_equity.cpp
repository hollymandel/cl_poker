#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <set>
#include <vector>
#include <map>

bool is_card(int v, char s){
	if (v < 2 || v > 14) return false;
	if (!(s=='s'||s=='h'||s=='d'||s=='c')) return false;
	return true;
}

std::map<int,char> rank_encode = {
	{14, 'a'}, {13, 'b'}, {12, 'c'}, {11, 'd'},
	{10, 'e'}, {9, 'f'}, {8, 'g'}, {7, 'h'},
	{6, 'i'}, {5, 'j'}, {4, 'k'}, {3, 'l'},
	{2, 'm'}
};

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

// a full ordering on cards is required for sorting hand set, but suit order is not relevant to play
bool operator<(const Card& a, const Card& b){
	return (a.get_value() < b.get_value()) 
		|| ((a.get_value() == b.get_value()) && (a.get_suit() < b.get_suit()));
}

bool operator==(const Card& a, const Card& b){
	return (a.get_value() == b.get_value()) && (a.get_suit() == b.get_suit());
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
	const int high_card_value() const{return *(num_set.rbegin());}
};


struct Tuple{
	int mult;
	int value;
	Tuple(int m, int v){mult = m; value = v;};	
	int kicker(Hand * h){
		for (auto itr = h->get_num_set()->begin(); itr != h->get_num_set()->end(); itr++){
			if (*itr != value){return *itr;};
		}
		return value;	
	}
};

bool operator<(const Tuple a, const Tuple b){
	return ((a.mult < b.mult) || (a.mult == b.mult) & (a.value < b.value));
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


std::vector<Tuple> tuples(const Hand h) {
	std::vector<int> nums;
	for (auto itr = h.get_set()->begin(); itr != h.get_set()->end(); itr++){
		nums.push_back(itr->get_value());
	}
	std::vector<Tuple> tups;
	for (auto num_itr = h.get_num_set()->begin(); num_itr != h.get_num_set()->end(); num_itr++){
		int this_count = std::count(nums.begin(), nums.end(), *num_itr);
		tups.push_back(Tuple(this_count, *num_itr));
	}
	sort(tups.begin(), tups.end());
	return tups;
}


const std::vector<char> Hand::best_hand() const{
	// is there a flush?
	const char get_flush = flush(*this);
	if(get_flush != 'n'){
		// is it a straight flush?
		std::vector<Card> flush_vect;
		for (auto itr = this->get_set()->begin(); itr != this->get_set()->end(); itr++){
			if (itr->get_suit() == get_flush){flush_vect.push_back(*itr);};
		}
		Hand flush_hand = Hand(flush_vect);
		const int straight_flush = straight(flush_hand);
		if (straight_flush){return {'a', rank_encode[straight_flush]};} 

		// if not, flush is the best hand (assuming 7 cards, 5 card hands)
		int flush_high = flush_hand.high_card_value();
		return { 'd', rank_encode[flush_high] };
	}

	// is there 4 of a kind? 
	std::vector<Tuple> tups = tuples(*this);
	if(tups.rbegin()->mult == 4){
		char tup_value = tups.rbegin()->value;
		return { 'b', rank_encode[tups.rbegin()->value], rank_encode[this->high_card_value()] };
	}
	
	// full house?
	if((tups.rbegin()->mult == 3) & ((tups.rbegin()+1)->mult == 2)){
		return { 'c', rank_encode[tups.rbegin()->value], rank_encode[(tups.rbegin()+1)->value] };
	}

	// unflush straight? 
	int strt = straight(*this);
	if(strt != 0){ return { 'e', rank_encode[strt] }; };
	
	// two pair?
	if((tups.rbegin()->mult == 2) & ((tups.rbegin()+1)->mult == 2)){
		return { 
			'f', 
			rank_encode[tups.rbegin()->value], 
			rank_encode[(tups.rbegin()+1)->value],
			rank_encode[
		};
	}
	
	// pair?
	if(tups.rbegin()->mult == 2)

	std::vector<char> v = {'a','b'};
	return v;
}

int main(){
	std::vector<int> a {10,11,12,13,14,2,2};
	std::vector<char> b {'c','c','d','c','c','d','h'};
	Hand x = Hand(a,b);
	std::vector<char> best_hand = x.best_hand();
	for (auto itr = best_hand.begin(); itr != best_hand.end(); itr++){std::cout << *itr;}
}
