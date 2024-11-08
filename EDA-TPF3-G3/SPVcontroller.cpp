#include "SPVcontroller.h"
#include "imgui.h"
#include "imgui_stdlib.h"

#define CHILD_W	220
#define CHILD_H	330
#define BUTTON_S	100
#define BUTTONS_PER_ROW 2

void SPVcontroller::update(void* m)
{
	errorType newMessage;
	while ((newMessage = snode->getVerificationError()).error == true) {
		warningHandler.check(newMessage);
	}
}

void SPVcontroller::cycle()
{
	ImGui::Begin(windowID.c_str());
	ImGui::BeginChild("SPV CONTROL", ImVec2(CHILD_W, CHILD_H));
	switch (cstate) {
	case SPV_MENU:
		drawWindow();
		break;
	case SPV_MTX:
		drawMTX();
		break;
	case SPV_PFILTER:
		drawPFilter();
		break;
	case SPV_CHANGEFN:
		drawChangeFN();
		break;
	case SPV_CHANGEHN:
		drawChangeHN();
		break;
	}
	ImGui::EndChild();
	ImGui::End();

	warningHandler.draw();
}

void SPVcontroller::drawWindow()
{
	const char* BUTTON_TEXT[4] = {
		"MAKE\nTX",
		"POST\nFILTER",
		"CHANGE\nFILTER NODE",
		"CHANGE\nHEADER NODE"
	};

	for (int i = 0; i < 4; i++) {
		if ((i % BUTTONS_PER_ROW) != 0)
			ImGui::SameLine(); 
		ImGui::PushID(i);
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 12.0f + 0.5f, 0.8f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 12.0f + 0.5f, 0.9f, 0.9f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 12.0f + 0.5f, 1.0f, 1.0f));
		if (ImGui::Button(BUTTON_TEXT[i], ImVec2(BUTTON_S, BUTTON_S)))
			cstate = SPVcontrolState(SPV_MTX + i);
		ImGui::PopStyleColor(3);
		ImGui::PopID();
	}

}

void SPVcontroller::drawMTX() {
	returnButton();
	if (TXHandler.drawTX()) {
		warningHandler.check(snode->makeTX(TXHandler.getVout(), TXHandler.getFee()));
		cstate = SPV_MENU;
	}
}

void SPVcontroller::drawPFilter() {
	returnButton();
	ImGui::Text("\nPOST FILTER\n\n");

	if (ImGui::Button("POST FILTER")) {
		warningHandler.check(snode->postFilter());
		cstate = SPV_MENU;
	}
}

void SPVcontroller::drawChangeFN() {
	returnButton();
	ImGui::Text("\nCHANGE FILTER NODE");

	ImGui::InputText("Filter Node ID", &newID);
	newPortSelect();
	newIpSelect();

	if (ImGui::Button("CHANGE FILTER NODE")) {
		warningHandler.check(snode->changeFilterNode(NodeData(newID, newPort, newIP[0], newIP[1], newIP[2], newIP[3])));
		cstate = SPV_MENU;
	}
}

void SPVcontroller::drawChangeHN() {
	returnButton();
	ImGui::Text("\nCHANGE HEADER NODE");

	ImGui::InputText("Header Node ID", &newID);
	newPortSelect();
	newIpSelect();

	if (ImGui::Button("CHANGE HEADER NODE")) {
		warningHandler.check(snode->changeHeaderNode(NodeData(newID, newPort, newIP[0], newIP[1], newIP[2], newIP[3])));
		cstate = SPV_MENU;
	}
}

void SPVcontroller::newPortSelect(){
	ImGui::Text("Neighbour Port:");
	ImGui::SetNextItemWidth(50);
	ImGui::DragInt("##Port", &newPort, 0.5);
}

void SPVcontroller::newIpSelect(){
	ImGui::Text("Neighbour IP:");
	ImGui::PushItemWidth(25);
	ImGui::DragInt(".##1", newIP, 0.5);
	ImGui::SameLine();
	ImGui::DragInt(".##2", newIP + 1, 0.5);
	ImGui::SameLine();
	ImGui::DragInt(".##3", newIP + 2, 0.5);
	ImGui::SameLine();
	ImGui::DragInt("##4", newIP + 3, 0.5);
	ImGui::PopItemWidth();
}

void SPVcontroller::returnButton() {

	if (ImGui::Button("Return"))
		cstate = SPV_MENU;
}



