#pragma once

#include "errorType.h"
#include <string>
#include "imgui.h"

using namespace std;

class WarningWindow
{
public:
	WarningWindow(const errorType& err,const string& identity);
	bool draw();
private:
	string myWindow;
	bool open;
	string message;
};

