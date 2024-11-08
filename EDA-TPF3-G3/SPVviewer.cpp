#include "SPVviewer.h"
#include "imgui.h"
#include "printTXroutine.h"

SPVviewer::SPVviewer() : NODEviewer(),
	headernodedata(NOT_DATA), filternodedata(NOT_DATA)
{}

void SPVviewer::update(void* n)
{
	SPVNode* node = (SPVNode*)n;

	nodedata = node->getData();
	headernodedata = node->getHeaderNodeData();
	filternodedata = node->getFilterNodeData();
	money = node->getMyMoney();
	type = node->getNodeType();

	windowName = nodedata.getID();
}

void SPVviewer::cycle() {
	ImGui::Begin(windowName.c_str());
	ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + CCHILD_W, ImGui::GetCursorPosY() - CHILD_H));
	ImGui::BeginChild("SPV VIEW", ImVec2(CHILD_W, CHILD_H));

	showNodeType();
	ImGui::Text("My EdaCoins: %d", money);
	printNodeData();
	ImGui::NewLine();
	if (ImGui::CollapsingHeader("Filter Node")) {
		ImGui::Text("Filter Node ID: %s", filternodedata.getID().c_str());
		ImGui::Text("Filter Node Port: %d", filternodedata.getSocket().getPort());
		ImGui::Text("Filter Node IP: %s", filternodedata.getSocket().getIPString().c_str());
	}
	if (ImGui::CollapsingHeader("Header Node")) {
		ImGui::Text("Header Node ID: %s", headernodedata.getID().c_str());
		ImGui::Text("Header Node Port: %d", headernodedata.getSocket().getPort());
		ImGui::Text("Header Node IP: %s", headernodedata.getSocket().getIPString().c_str());
	}

	ImGui::EndChild();
	ImGui::End();
}
