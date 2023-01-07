module cards;

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
int POCKET_SIZE = 2;
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

std::map<int,std::string> rank_encode = {
	{14, "14"}, {13, "13"}, {12, "12"}, {11, "11"},
	{10, "10"}, {9, "09"}, {8, "08"}, {7, "07"},
	{6, "06"}, {5, "05"}, {4, "04"}, {3, "03"},
	{2, "02"}
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

std::ostream& operator<<(std::ostream& os, const Card& c)
{
    os << c.get_value() << " of " << suit_map[c.get_suit()] << "\n";
    return os;
}


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
	Hand(){}
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
	const std::string best_hand() const; 
	const int high_card_value() const{return *(num_set.rbegin());}
	
	void append(Hand h){
		std::vector<Card> joined_vect(h.get_set()->begin(), h.get_set()->end());
		joined_vect.insert(joined_vect.begin(),this->cc.begin(), this->cc.end());
		*this = Hand(joined_vect);
	}
};

std::ostream& operator<<(std::ostream& os, const Hand& h)
{
	for(auto itr = h.get_set()->begin(); itr != h.get_set()->end(); itr++){
		std::cout << *itr;
	}
	return os;
}


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


const std::string Hand::best_hand() const{
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
		if (straight_flush){return "a" + rank_encode[straight_flush];} 

		// if not, flush is the best hand (assuming 7 cards, 5 card hands)
		int flush_high = flush_hand.high_card_value();
		return "i" + rank_encode[flush_high];
	}

	// four of a kind? 
	std::vector<Tuple> tups = tuples(*this);
	if(tups.rbegin()->mult == 4){
		char tup_value = tups.rbegin()->value;
		return "h" + rank_encode[tups.rbegin()->value] + rank_encode[kickers(*this)[0]];
	}
	
	// full house?
	if((tups.rbegin()->mult == 3) & ((tups.rbegin()+1)->mult == 2)){
		return "g" + rank_encode[tups.rbegin()->value] + rank_encode[(tups.rbegin()+1)->value];
	}

	// unflush straight? 
	int strt = straight(*this);
	if(strt != 0){ return "e" + rank_encode[strt]; };
	
	// three of a kind?
	if(tups.rbegin()->mult == 3){
		char tup_value = tups.rbegin()->value;
		return "d" + rank_encode[tups.rbegin()->value] + rank_encode[kickers(*this)[0]] + rank_encode[kickers(*this)[1]];
	}

	// two pair?
	if((tups.rbegin()->mult == 2) & ((tups.rbegin()+1)->mult == 2)){
		return "c" +  
			rank_encode[tups.rbegin()->value] + 
			rank_encode[(tups.rbegin()+1)->value] +
			rank_encode[kickers(*this)[0]];
	}
	
	// pair?
	if(tups.rbegin()->mult == 2){
		return "b" + 
			rank_encode[tups.rbegin()->value] +
			rank_encode[kickers(*this)[0]] +
			rank_encode[kickers(*this)[1]] +
			rank_encode[kickers(*this)[2]];
	}

	// kickers only
	if(tups.rbegin()->mult == 1){
		std::vector<int> kicks = kickers(*this);
		std::string kicks_encode = "a";
		for (int i = 0; i < 5; i++){
			kicks_encode.append(rank_encode[kicks[i]]);
		}
		return kicks_encode;
	}

	return "0";
}

std::vector<Card> random_distinct_cards(int n){
	assert(n >= 0);
	std::vector<Card> cs;
	for(int i = 0; i < n; i++){
		cs.push_back(Card(rand() % 13 + 2, rand() % 4));
	}
	try{Hand h = Hand(cs);}
	catch(Hand::InvalidHand){return random_distinct_cards(n);}
	return cs;
};

Hand random_hand(int n, Hand exclude){
	assert(n >= 0);
	std::vector<Card> cs = random_distinct_cards(n);
	try{
		Hand draw = Hand(cs); // catch duplicates within the draw
		exclude.append(draw); // catch duplicates with the exclude cards
		return draw;
	}
	catch(Hand::InvalidHand){return random_hand(n, exclude);}
};


std::vector<Hand> random_table(int n_villains, Hand exclude){
	std::vector<Hand> random_hands;
	Hand draw;
	for(int i = 0; i < n_villains; i++){
		while(true){
			draw = random_hand(POCKET_SIZE, exclude);
			try{exclude.append(draw); break;}
			catch(Hand::InvalidHand){}
		}
		random_hands.push_back(draw);
	}
	return random_hands;
}

int main(){
	Hand table = random_hand(TABLE_SIZE, Hand());
	std::cout << "The table is:\n" << table << "\n\n";
	Hand running_total = table;
	std::vector<Hand> vill_hands = random_table(3, running_total);
	std::string best_yet = "0";
	for(auto itr = vill_hands.begin(); itr != vill_hands.end(); itr++){
		Hand vill_hand = table;
		vill_hand.append(*itr);
		std::string bh = vill_hand.best_hand();
		std::cout << "appended villain hand:\n" << vill_hand << "\n\nbest hand: ";
		for(auto itr = bh.begin(); itr != bh.end(); itr++){std::cout << *itr;}
		std::cout << "\n\n";
		if(bh > best_yet){best_yet = bh;}
	}
	std::cout << "\n\n\n\nbest hand was ";
	for(auto itr = best_yet.begin(); itr != best_yet.end(); itr++){std::cout << *itr;}
}
