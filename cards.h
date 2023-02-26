#ifndef __cards__
#define __cards__

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

std::string rank_encode(int i){
	int output_len = 3; // two characters + trailing 0
    char buffer[output_len]; 
	std::snprintf(buffer, output_len, "%02d", i); // snprintf prevents overflow
	std::string str(buffer);
    return str;
}

class Card{
private:
	int value;
	int suit;
public:
	class InvalidCard{};
	Card()
	{
	}
	Card(int v, int s, bool validate = true)
	{	
		if(validate){
			if (!is_card(v,s)) throw InvalidCard();
		}
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
public:
	class InvalidHand{};
	Hand(){}
	Hand(std::vector<Card> cs)
	{
		for (auto itr=cs.begin(); itr < cs.end(); itr++){
			if (cc.count(*itr)) { throw InvalidHand(); }
			cc.insert(*itr);
		}
	}
	Hand(std::vector<int> v, std::vector<int> s)
	{
		int num_cards = v.size();
		assert(s.size() == num_cards);
		for (int i=0; i < num_cards; i++) {
			Card this_card(v[i],s[i]);
			if (cc.count(this_card)) { throw InvalidHand(); } 
			cc.insert(this_card); 
		}
	}
	Card get_card(int i) const{ 
		return *std::next(cc.begin(), i); 
	}
	const std::set<Card> *get_set() const{return &cc;}
	const std::string best_hand() const; 
	const int high_card_value() const{return cc.rbegin()->get_value();}
	
	Hand append(Hand h){
		std::vector<Card> joined_vect(h.get_set()->begin(), h.get_set()->end());
		joined_vect.insert(joined_vect.begin(),this->cc.begin(), this->cc.end());
		return Hand(joined_vect);
	}
};

std::ostream& operator<<(std::ostream& os, const Hand& h){
	for(auto itr = h.get_set()->begin(); itr != h.get_set()->end(); itr++){std::cout << *itr;}
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

bool is_suit(Card c, int s){return c.get_suit() == s;}

int flush(const Hand h) {
	for(int i = 0; i != 4; i++){
		int this_count = count_if(h.get_set()->begin(), h.get_set()->end(), [i](Card c){return is_suit(c,i);});
		if(this_count >= 5){return i;}
	}
	return -1;
}

int straight(const Hand h) {
	std::set<int> nums;
	for (auto itr = h.get_set()->begin(); itr != h.get_set()->end(); itr++){
		int this_value = itr->get_value();
		nums.insert(this_value);
		if(this_value == 14){nums.insert(1);} // allowing low aces
	}
	nums.insert(MAX_VAL + 2); // extra value to dump last real num
	int run_len = 1;
	int last_int = 0;
	for (auto num_itr = nums.begin(); num_itr != nums.end(); num_itr++){
		if(*num_itr == last_int + 1){run_len++; last_int++;}
		else{
			if(run_len > 4){return last_int;}
			run_len=1; last_int = *num_itr;
		}
		
	}
	return 0;
}

std::vector<Tuple> tuples(const Hand h) {
	int last_num = 0;
	int last_count = 0;
	std::vector<Tuple> tups;
	std::set<Card> cc(*h.get_set());
	cc.insert(Card(15, 4, false)); // "highest" card added to dump out last real tuple
	for (auto itr = cc.begin(); itr != cc.end(); itr++){
		int this_value = itr->get_value();
		if(this_value == last_num){last_count++;}
		else{
			tups.push_back(Tuple(last_count, last_num));
			last_num = this_value; 
			last_count = 1;
		}
	}
	tups.erase(tups.begin()); // first tuple was (0,0)
	sort(tups.begin(), tups.end());
	return tups;
}

std::vector<int> kickers(const Hand h, int n = 1) {
	std::vector<Tuple> tups = tuples(h);
	std::vector<int> kickers;
	for (auto itr = tups.rbegin(); itr != tups.rend(); itr++){
		if (itr->mult <= n){kickers.push_back(itr->value);}; // n = 3 for four of a kind, otherwise 1
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
		if (straight_flush){return "a" + rank_encode(straight_flush);} 

		// if not, flush is the best hand (assuming 7 cards, 5 card hands)
		int flush_high = flush_hand.high_card_value();
		return "i" + rank_encode(flush_high);
	}

	// four of a kind? 
	std::vector<Tuple> tups = tuples(*this);
	if(tups.rbegin()->mult == 4){
		char tup_value = tups.rbegin()->value;
		return "h" + rank_encode(tups.rbegin()->value) + rank_encode(kickers(*this, 3)[0]);
	}
	
	// full house?
	if((tups.rbegin()->mult == 3) & ((tups.rbegin()+1)->mult == 2)){
		return "g" + rank_encode(tups.rbegin()->value) + rank_encode((tups.rbegin()+1)->value);
	}

	// unflush straight? 
	int strt = straight(*this);
	if(strt != 0){ return "e" + rank_encode(strt); };
	
	// three of a kind?
	if(tups.rbegin()->mult == 3){
		char tup_value = tups.rbegin()->value;
		return "d" + rank_encode(tups.rbegin()->value) + rank_encode(kickers(*this)[0]) + rank_encode(kickers(*this)[1]);
	}

	// two pair?
	if((tups.rbegin()->mult == 2) & ((tups.rbegin()+1)->mult == 2)){
		return "c" +  
			rank_encode(tups.rbegin()->value) + 
			rank_encode((tups.rbegin()+1)->value) +
			rank_encode(kickers(*this)[0]);
	}
	
	// pair?
	if(tups.rbegin()->mult == 2){
		return "b" + 
			rank_encode(tups.rbegin()->value) +
			rank_encode(kickers(*this)[0]) +
			rank_encode(kickers(*this)[1]) +
			rank_encode(kickers(*this)[2]);
	}

	// kickers only
	if(tups.rbegin()->mult == 1){
		std::vector<int> kicks = kickers(*this);
		std::string kicks_encode = "a";
		for (int i = 0; i < 5; i++){
			kicks_encode.append(rank_encode(kicks[i]));
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

Hand random_hand(int n, Hand exclude = Hand()){
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
			try{exclude = exclude.append(draw); break;}
			catch(Hand::InvalidHand){}
		}
		random_hands.push_back(draw);
	}
	return random_hands;
}

#endif //__cards__
