#pragma once
#include "WarningWindow.h"
#include <vector>

using namespace std;

class WarningWindowHandler
{
public:
	WarningWindowHandler(const string& _id);
	void check(const errorType& err);
	void draw();
private:
	string id;
	unsigned int warningC;
	vector<WarningWindow> warnings;
};

