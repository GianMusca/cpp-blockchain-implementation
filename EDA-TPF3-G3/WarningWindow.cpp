#include "WarningWindow.h"

WarningWindow::WarningWindow(const errorType& err, const string& identity) :
	message(err.datos), open(true)
{
	myWindow = "ERROR##" + identity;
}

bool WarningWindow::draw()
{
	ImGui::Begin(myWindow.c_str(),&open);
	ImGui::Text(message.c_str());
	ImGui::End();
	return open;
}
