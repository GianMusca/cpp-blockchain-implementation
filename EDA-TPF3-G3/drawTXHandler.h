#pragma once
#include "observer.h"
#include "Node.h"

class drawTXHandler {
public:
	drawTXHandler();
	bool drawTX();
	unsigned long int getFee();
	vector<Vout> getVout();
	
private:
	bool drawV(Vout&, int);
	bool isAllDigits(string);
	vector<string> auxstr;
	vector<Vout> txVout;
	string feeStr;
	int currTX;
	bool errfee;
	bool erramount;
};



