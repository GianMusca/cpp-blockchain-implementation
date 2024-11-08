
#include "FULLviewer.h"
#include "imgui.h"
#include "printTXroutine.h"

#define NO_DATA "NO DATA",0,0,0,0,0

FULLviewer::FULLviewer() : NODEviewer(),
	neighbours(), pendingTX(), treeHandler(),
	blockchain(), layoutHandler()
{
	//ImGuiIO& io = ImGui::GetIO();
	//numberFont = io.Fonts->AddFontFromFileTTF("BebasNeue_Regular.ttf", 20.0f);
	redButton = false;
	nbutton = al_load_bitmap("Button_normal.png");
	pbutton = al_load_bitmap("Button_pressed.png");
}

FULLviewer::~FULLviewer() {
	al_destroy_bitmap(nbutton);
	al_destroy_bitmap(pbutton);

}

void FULLviewer::update(void*n)
{
	FULLNode* node = (FULLNode*)n;

	nodedata = node->getData();
	neighbours = node->getNeighbours();
	pendingTX = node->getPendingTX();
	blockchain = node->getBlockChain();
	layout = node->getLayout();
	money = node->getMyMoney();
	type = node->getNodeType();
	windowName = nodedata.getID();
	
	treeHandler.setId(nodedata.getID());
}

void FULLviewer::cycle()
{
	drawWindow();
	treeHandler.draw();
	layoutHandler.draw();
}

void FULLviewer::drawWindow() {
	ImGui::Begin(windowName.c_str());
	ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + CCHILD_W, ImGui::GetCursorPosY() - CHILD_H));
	ImGui::BeginChild("FULL VIEW", ImVec2(CHILD_W, CHILD_H));

	showNodeType();
	ImGui::Text("My EdaCoins: %d", money);
	printNodeData();
	printNeighbours();
	printBlockList();
	drawBigRedButton();

	ImGui::EndChild();
	ImGui::End();
}


void FULLviewer::printNeighbours()
{
	if (ImGui::CollapsingHeader("Neighbours")) {
		if (neighbours->size() > 0) {
			for (int i = 0; i < neighbours->size(); i++) {
				ImGui::Text("ID: %s | Port: %s | IP: %s", (*neighbours)[i].getID().c_str(), (*neighbours)[i].getSocket().getPortString().c_str(), (*neighbours)[i].getSocket().getIPString().c_str());
			}
		}
		else {
			ImGui::Text("This node has no neighbours... and that's pretty strange, isn't it?");
		}
	}
}

void FULLviewer::printPendingTX()
{
	if (ImGui::CollapsingHeader("Transactions")) {

		if (pendingTX->size() > 0) {
			ImGui::Text(" ");
			ImGui::SameLine();
			ImGui::BeginChild("Txs", ImVec2(CCHILD_W, CCHILD_H));
			for (int i = 0; i < pendingTX->size(); i++) {
				//
				if(ImGui::CollapsingHeader("TX %d", i))
					printTx((*pendingTX)[i],i);

				/* //DO NOT ERASE (YET)
				string subWindowName = (*pendingTX)[i].txId + "##" + to_string(i);
				if (ImGui::CollapsingHeader(subWindowName.c_str())) {

					ImGui::Text("ID: %s", (*pendingTX)[i].txId.c_str());
					printVin(i);
					printVout(i);
				}
				*/
				//
			}
			ImGui::EndChild();
		}
		else {
			ImGui::Text("There are no new Transactions");
		}
	}
}

void FULLviewer::printBlockList() {
	if (ImGui::CollapsingHeader("Blockchain")) {
		//vector<pair<const unsigned long, int>> tags;
		//for (int i = 0; i < blockchain->size(); i++) {
		//	pair<const unsigned long, int> par((*blockchain)[i].getHeight(), i);
		//	tags.emplace_back(par);
		//}
		//sort(tags.begin(), tags.end());
		//for (int j = 0; j < tags.size(); j++) {
		//	string blockid = "Block " + (*blockchain)[tags[j].second].getBlockID();
			//if (ImGui::Selectable(blockid.c_str()))
			//	treeHandler.createWindow((*blockchain)[tags[j].second]);
		//}

		for (int i = 0; i < blockchain->size(); i++) {
			string blockid = "Block " + (*blockchain)[i].getBlockID();
			if (ImGui::Selectable(blockid.c_str()))
				treeHandler.createWindow((*blockchain)[i]);

		}
	}
}

void FULLviewer::drawBigRedButton() {
	ImVec2 cursor = ImGui::GetCursorPos();
	cursor.x = ImGui::GetWindowWidth() / 2 - 100 / 2;
	ImGui::SetCursorPos(cursor);
	if (!redButton)
		ImGui::Image(nbutton, ImVec2(100, 100));
	else
		ImGui::Image(pbutton, ImVec2(100, 100));
	//    ImGui::PushStyleColor(1, ImVec4(250, 0, 0, 0));
  //  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 100);
	ImGui::SetCursorPos(cursor);
	if (ImGui::InvisibleButton("jejox", ImVec2(100, 100))) {
		redButton = !redButton;
		if (redButton)
			layoutHandler.createWindow(nodedata, *layout, *neighbours);
		else
			layoutHandler.close();
	}
}

/* DO NOT ERASE (YET)
void FULLviewer::printVin(int i)
{
	ImGui::Text("Number of incomes: %u", (*pendingTX)[i].nTxIn);
	ImGui::Text(" ");
	ImGui::SameLine();
	string vinwindow = "vins##" + to_string(i);
	ImGui::BeginChild(vinwindow.c_str(), ImVec2(VCHILD_W, VCHILD_H), false, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
	if (ImGui::CollapsingHeader("vIn")) {
		for (int j = 0; j < (*pendingTX)[i].vIn.size(); j++) {
			ImGui::Text("Block ID: %s\tTx ID: %s", (*pendingTX)[i].vIn[j].blockId.c_str(), (*pendingTX)[i].vIn[j].txId.c_str());
		}
	}
	ImGui::EndChild();
}

void FULLviewer::printVout(int i)
{
	ImGui::Text("Number of outcomes: %u", (*pendingTX)[i].nTxOut);
	ImGui::Text(" ");
	ImGui::SameLine();
	string voutwindow = "vout##" + to_string(i);
	ImGui::BeginChild(voutwindow.c_str(), ImVec2(VCHILD_W, VCHILD_H), false, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
	if (ImGui::CollapsingHeader("vOut")) {
		for (int j = 0; j < (*pendingTX)[i].vOut.size(); j++) {
			ImGui::Text("Public ID: %s\tTx Amount: %lu", (*pendingTX)[i].vOut[j].publicId.c_str(), (*pendingTX)[i].vOut[j].amount);
		}
	}
	ImGui::EndChild();
}
*/