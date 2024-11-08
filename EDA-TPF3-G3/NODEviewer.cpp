#pragma once
#include "NODEviewer.h"
#include "imgui.h"


NODEviewer::NODEviewer() :
	nodedata(NOT_DATA), windowName("NO_DATA"), type(NODO_UNKNOWN), money(0) {}

//void NODEviewer::update(void* n){
//	Node* node = (Node*)n;
//	nodedata = node->getData();
//	money = node->getMyMoney();
//	type = node->getNodeType();
//	windowName = nodedata.getID();
//}

void NODEviewer::printNodeData()
{
	if (ImGui::CollapsingHeader("Data")) {
		ImGui::Text("ID: %s", nodedata.getID().c_str());
		ImGui::Text("Port: %s", nodedata.getSocket().getPortString().c_str());
		ImGui::Text("IP: %s", nodedata.getSocket().getIPString().c_str());
	}
}

void NODEviewer::showNodeType() {
	switch (type) {
	case NODO_UNKNOWN: default:
		ImGui::Text("Node Type: UNKNOWN");
		break;
	case NODO_MINERO:
		ImGui::Text("Node Type: MINER");
		break;
	case NODO_FULL:
		ImGui::Text("Node Type: FULL");
		break;
	case NODO_SPV:
		ImGui::Text("Node Type: SPV");
		break;
	}
}


