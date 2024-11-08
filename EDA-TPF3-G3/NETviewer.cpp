#include "NETviewer.h"

#include "imgui.h"

NETviewer::NETviewer(const string& id) :
	myWindowName(id), listofFULL(), listofSPV()
{
}

void NETviewer::update(void* m)
{
	EDAcoinsNet* model = (EDAcoinsNet*)m;
	listofFULL = model->getKnownFULLdata();
	listofSPV = model->getKnownSPVdata();
}

void NETviewer::cycle() {
	ImGui::Begin(myWindowName.c_str());
	for (int i = 0; i < listofFULL.size(); i++) {
		ImGui::Text("ID: %s", listofFULL[i].getID().c_str());
		ImGui::Text("Port: %s", listofFULL[i].getSocket().getPortString().c_str());
		ImGui::SameLine();
		ImGui::Text("IP: %s", listofFULL[i].getSocket().getIPString().c_str());
	}
	for (int i = 0; i < listofSPV.size(); i++) {
		ImGui::Text("ID: %s", listofSPV[i].getID().c_str());
		ImGui::Text("Port: %s", listofSPV[i].getSocket().getPortString().c_str());
		ImGui::SameLine();
		ImGui::Text("IP: %s", listofSPV[i].getSocket().getIPString().c_str());
	}
	ImGui::End();
}

