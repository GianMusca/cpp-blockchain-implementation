#include "WarningWindowHandler.h"

WarningWindowHandler::WarningWindowHandler(const string& _id) :
	id(_id), warningC(0), warnings()
{
}

void WarningWindowHandler::check(const errorType& err)
{
	if (err.error == true) {
		string identity = id + to_string(warningC);
		warnings.emplace_back(err, identity);
		warningC++;
	}
}

void WarningWindowHandler::draw()
{
	bool erase = false;
	int closingOne = 0;
	for (int i = 0; i < warnings.size(); i++) {
		if (warnings[i].draw() == false) {
			erase = true;
			closingOne = i;
		}
	}
	if (erase == true) {
		warnings.erase(warnings.begin() + closingOne);
	}
}
