#pragma once

#include <vector>
using namespace std;

typedef size_t index;

#define NUMBER_OF_CONECTIONS	2

class layoutNumericConection
{
public:
	layoutNumericConection(index myIndex);
	bool initialConection(layoutNumericConection& newConection); //with marks changed accordingly
	bool initialConection(layoutNumericConection& newConection,size_t& marks); //change marks if new marked conection
	bool isMarked();

	void mark();
	void unmark();

	const vector<index>& getConections();
	void secondaryConection(layoutNumericConection& newConection); //doesn't change marks
private:
	index myself;
	bool marked;
	vector<index> conections;

	void initialCheckMark();
	void initialCheckMark(size_t& marks); //change marks if new marked conection
	bool isAlreadyConectedToMe(layoutNumericConection& newConection);
	void forcedConection(layoutNumericConection& newConection); //doens't change marks 
};

