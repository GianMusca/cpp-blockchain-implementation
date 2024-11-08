#include "FULLcontroller.h"
#include "imgui.h"
#include "imgui_stdlib.h"

#define CHILD_W	220
#define CHILD_H	330
#define BUTTON_S		100
#define BUTTONS_PER_ROW	2

void FULLcontroller::update(void*) {
	errorType newMessage;
	if (fnode->getNodeType() == NODO_MINERO) {
		while ((newMessage = ((MINERNode*)fnode)->getWindowMessage()).error == true) {
			warningHandler.check(newMessage);
		}
	}
}

void FULLcontroller::cycle()
{
	ImGui::Begin(windowID.c_str());
	ImGui::BeginChild("FULL CONTROL", ImVec2(CHILD_W, CHILD_H));
	switch (cstate) {
	case FULL_MENU:
		drawWindow();
		break;
	case FULL_MTX:
		drawMTX();
		break;
	case FULL_MBLOCK:
		drawMBlock();
		break;
	case FULL_ADDN:
		drawAddNode();
		break;
	}
	ImGui::EndChild();
	ImGui::End();

	warningHandler.draw();
}

void FULLcontroller::drawWindow()
{
	const char* BUTTON_TEXT[3] = {
		"MAKE\nTX",
		"MAKE\nBLOCK",
		"ADD NEIGHBOUR"
	};

	for (int i = 0; i < 3; i++) {
		if ((i % BUTTONS_PER_ROW) != 0)
			ImGui::SameLine();
		ImGui::PushID(i);
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 12.0f + 0.5f, 0.8f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 12.0f + 0.5f, 0.9f, 0.9f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 12.0f + 0.5f, 1.0f, 1.0f));
		if (i < 2) {
			if (ImGui::Button(BUTTON_TEXT[i], ImVec2(BUTTON_S, BUTTON_S)))
				cstate = FULLcontrolState(FULL_MTX + i);
		}
		else {
			if (ImGui::Button(BUTTON_TEXT[i], ImVec2(2 * BUTTON_S + 8, BUTTON_S)))
				cstate = FULLcontrolState(FULL_MTX + i);
		}
		ImGui::PopStyleColor(3);
		ImGui::PopID();
	}

}

void FULLcontroller::drawMBlock() {
	returnButton();
	ImGui::Text("\nMAKE BLOCK\n\n");

	if (ImGui::Button("MAKE BLOCK")) {
		warningHandler.check(fnode->makeBlock());
		cstate = FULL_MENU;
	}
}

void FULLcontroller::drawMTX() {
	returnButton();
	if (TXHandler.drawTX()) {
		warningHandler.check(fnode->makeTX(TXHandler.getVout(), TXHandler.getFee()));
		cstate = FULL_MENU;
	}
}


void FULLcontroller::drawAddNode() {
	returnButton();
	ImGui::Text("\nADD A NEIGHBOUR\n\n");
	ImGui::InputText("Neighbour ID", &newID);
	newPortSelect();
	newIpSelect();

	if (ImGui::Button("ADD NEIGHBOUR")) {
		warningHandler.check(fnode->addNeighbour(NodeData(newID, newPort, newIP[0], newIP[1], newIP[2], newIP[3])));
		cstate = FULL_MENU;
	}
}

void FULLcontroller::newPortSelect(){
	ImGui::Text("Neighbour Port:");
	ImGui::SetNextItemWidth(50);
	ImGui::DragInt("##Port", &newPort, 0.5);
}

void FULLcontroller::newIpSelect(){
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


void FULLcontroller::returnButton() {

	if (ImGui::Button("Return"))
		cstate = FULL_MENU;
}

//void FULLcontroller::neighbourSelect()
//{
//	if (fnode->getNeighbours()->size()) {
//		int curr = 0;
//		ImGui::Text("Neighbour nodes -empty-");
//		ImGui::Combo("##Neighbour nodes -empty-", &curr, "\0");
//	}
//	else {
//		ImGui::Text("Neighbour nodes");
//		ImGui::Combo("##Neighbour nodes", &currNeighbour, findNeighbourNames());
//	}
//}
//
//const char* FULLcontroller::findNeighbourNames() {
//	string neighbourNames;
//
//	for (int i = 0; i < fnode->getNeighbours()->size(); i++) {
//		neighbourNames += fnode->getNeighbours()[currNeighbour]->getData().getID();
//		neighbourNames += '\0';
//	}
//	neighbourNames += '\0';
//
//	return neighbourNames.c_str();
//}

/*errorType makeTX(const vector<Vout>& receivers);
	errorType makeBlock(); //"MINAR"
	errorType addNeighbour(NodeData neighbour); //agrega fulls*/


