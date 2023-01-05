#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <set>
#include <vector>
#include <map>
#include <cstdlib>
#include <time.h>
#include <string>

int DRAW_LEN = 7;
int HAND_LEN = 5;
int TABLE_SIZE = 5;
int N_SUITS = 4;
int MIN_VAL = 2;
int MAX_VAL = 14;

bool is_card(int v, int s){
	if (v < MIN_VAL || v > MAX_VAL) return false;
	if (s < 0 || s >= N_SUITS) return false;
	return true;
}

std::map<int,std::string> suit_map = {{0, "clubs"}, {1, "diamonds"}, {2, "hearts"}, {3, "spades"}};

std::map<int,char> rank_encode = {
	{14, 'a'}, {13, 'b'}, {12, 'c'}, {11, 'd'},
	{10, 'e'}, {9, 'f'}, {8, 'g'}, {7, 'h'},
	{6, 'i'}, {5, 'j'}, {4, 'k'}, {3, 'l'},
	{2, 'm'}
};

class Card{
private:
	int value;
	int suit;
public:
	class InvalidCard{};
	Card()
	{
	}
	Card(int v, int s)
	{	
		if (!is_card(v,s)) throw InvalidCard();
		value = v;
		suit = s;
	}
	int get_value() const {return value;}
	int get_suit() const {return suit;}
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
	std::set<int> suit_set;
public:
	class InvalidHand{};
	Hand(std::vector<Card> cs)
	{
		for (auto itr=cs.begin(); itr < cs.end(); itr++){
			if (cc.count(*itr)) { throw InvalidHand(); }
			cc.insert(*itr); 
			num_set.insert(itr->get_value());
			suit_set.insert(itr->get_suit());
		}
	}
	Hand(std::vector<int> v, std::vector<int> c)
	{
		int num_cards = v.size();
		assert(c.size() == num_cards);
		for (int i=0; i < num_cards; i++) {
			Card this_card(v[i],c[i]);
			if (cc.count(this_card)) { throw InvalidHand(); } 
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
	const std::set<int> *get_suit_set() const{return &suit_set;}
	const std::vector<char> best_hand() const; 
	const int high_card_value() const{return *(num_set.rbegin());}
	
	void append(Hand h){
		std::vector<Card> joined_vect(h.get_set()->begin(), h.get_set()->end());
		joined_vect.insert(joined_vect.begin(),this->cc.begin(), this->cc.end());
		*this = Hand(joined_vect);
	}
};


struct Tuple{
	int mult;
	int value;
	Tuple(int m, int v){mult = m; value = v;};	
};

bool operator<(const Tuple a, const Tuple b){
	return ((a.mult < b.mult) || (a.mult == b.mult) & (a.value < b.value));
};


int flush(const Hand h) {
	int active_suit = 0;
	int run_len = 0;
	std::set<Card>::iterator itr;
	for (itr = h.get_set()->begin(); itr != h.get_set()->end(); itr++){
		if(itr->get_suit() == active_suit){run_len++;}
		else{active_suit = itr->get_suit(); int run_len = 1;}
		if (run_len == 5){return active_suit;}
	}
	return -1;
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

std::vector<int> kickers(const Hand h) {
	std::vector<Tuple> tups = tuples(h);
	std::vector<int> kickers;
	for (auto itr = tups.rbegin(); itr != tups.rend(); itr++){
		if (itr->mult == 1){kickers.push_back(itr->value);};
	}
	return kickers;
	// note that in standard play you probably only want the first 1-3 elements of kickers
}


const std::vector<char> Hand::best_hand() const{
	// is there a flush?
	const int get_flush = flush(*this);
	if(get_flush != -1){
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

	// four of a kind? 
	std::vector<Tuple> tups = tuples(*this);
	if(tups.rbegin()->mult == 4){
		char tup_value = tups.rbegin()->value;
		return { 'b', rank_encode[tups.rbegin()->value], rank_encode[kickers(*this)[0]] };
	}
	
	// full house?
	if((tups.rbegin()->mult == 3) & ((tups.rbegin()+1)->mult == 2)){
		return { 'c', rank_encode[tups.rbegin()->value], rank_encode[(tups.rbegin()+1)->value] };
	}

	// unflush straight? 
	int strt = straight(*this);
	if(strt != 0){ return { 'e', rank_encode[strt] }; };
	
	// three of a kind?
	if(tups.rbegin()->mult == 3){
		char tup_value = tups.rbegin()->value;
		return { 'f', rank_encode[tups.rbegin()->value], rank_encode[kickers(*this)[0]], rank_encode[kickers(*this)[1]] };
	}

	// two pair?
	if((tups.rbegin()->mult == 2) & ((tups.rbegin()+1)->mult == 2)){
		return { 
			'g', 
			rank_encode[tups.rbegin()->value], 
			rank_encode[(tups.rbegin()+1)->value],
			rank_encode[kickers(*this)[0]],	
		};
	}
	
	// pair?
	if(tups.rbegin()->mult == 2){
		return {
			'h',
			rank_encode[tups.rbegin()->value],
			rank_encode[kickers(*this)[0]],
			rank_encode[kickers(*this)[1]],
			rank_encode[kickers(*this)[2]],
		};
	}

	// kickers only
	if(tups.rbegin()->mult == 1){
		std::vector<int> kicks = kickers(*this);
		std::vector<char> kicks_encode = {'i'};
		for (int i = 0; i < 5; i++){
			kicks_encode.push_back(rank_encode[kicks[i]]);
		}
		return kicks_encode;
	}

	return {'x'};
}

std::vector<Card> random_distinct_cards(int n){
	std::vector<Card> cs;
	for(int i = 0; i < n; i++){
		cs.push_back(Card(rand() % 13 + 2, rand() % 4));
	}
	try{Hand h = Hand(cs);}
	catch(Hand::InvalidHand){return random_distinct_cards(n);}
	return cs;
};

Hand random_hand(int n){
	std::vector<Card> cs = random_distinct_cards(n);
	try{return Hand(cs);}
	catch(Hand::InvalidHand){return random_hand(n);}
};


std::vector<Card> villain_hands(int n_villains, std::vector<Card> shared_cards){
	assert(shared_cards.size() <= TABLE_SIZE);
	Hand h = Hand(shared_cards); // catch repeats in input shared_cards


	while(TABLE_SIZE > shared_cards.size()){
		std::vector<Card> full_table(shared_cards); 
		std::vector<Card> table_remainder = random_distinct_cards(TABLE_SIZE - shared_cards.size()); 
		full_table.insert(full_table.end(), table_remainder.begin(), table_remainder.end());
		try{Hand h = Hand(full_table); shared_cards = full_table;}
		catch(Hand::InvalidHand){std::cout << "oops";};
	}

	//std::vector<Hand> villain_pockets;
	//for(int i = 0; i < n_villains; i++){
	//	Hand pocket_draw = random_hand(2);
	//	try{
	//	villain_pockets.push_back(random_hand(2)
	//}

	return shared_cards;
}

int main(){
	for(int i = 0; i < 0; i++){
		std::vector<Card> shared_cards = random_distinct_cards(3);
		std::vector<Card> vils = villain_hands(4, shared_cards);
		for(auto itr = shared_cards.begin(); itr != shared_cards.end(); itr++){std::cout << itr->get_value() << itr->get_suit() << " ";}
		std::cout << "\n";
		for(auto itr = vils.begin(); itr != vils.end(); itr++){std::cout << itr->get_value() << itr->get_suit() << " ";}
	}
}
