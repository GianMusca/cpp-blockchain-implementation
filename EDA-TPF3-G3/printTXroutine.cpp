#include "printTXroutine.h"
#include "imgui.h"


static void printVin(const Transaction& tx, int i);
static void printVout(const Transaction& tx, int i);

void printTx(const Transaction& tx, int i)
{
	string subWindowName = tx.txId + "##" + to_string(i);
	if (ImGui::CollapsingHeader(subWindowName.c_str())) {

		ImGui::Text("ID: %s", tx.txId.c_str());
		printVin(tx,i);
		printVout(tx,i);
	}
}

void printVin(const Transaction& tx, int i)
{
	ImGui::Text("Number of incomes: %u", tx.nTxIn);
	ImGui::Text(" ");
	ImGui::SameLine();
	string vinwindow = "vins##" + to_string(i);
	ImGui::BeginChild(vinwindow.c_str(), ImVec2(VCHILD_W, VCHILD_H), false, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
	if (ImGui::CollapsingHeader("vIn")) {
		for (int j = 0; j < tx.vIn.size(); j++) {
			ImGui::Text("Block ID: %s\tTx ID: %s", tx.vIn[j].blockId.c_str(), tx.vIn[j].txId.c_str());
		}
	}
	ImGui::EndChild();
}

void printVout(const Transaction& tx, int i)
{
	ImGui::Text("Number of outcomes: %u", tx.nTxOut);
	ImGui::Text(" ");
	ImGui::SameLine();
	string voutwindow = "vout##" + to_string(i);
	ImGui::BeginChild(voutwindow.c_str(), ImVec2(VCHILD_W, VCHILD_H), false, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
	if (ImGui::CollapsingHeader("vOut")) {
		for (int j = 0; j < tx.vOut.size(); j++) {
			ImGui::Text("Public ID: %s\tTx Amount: %lu", tx.vOut[j].publicId.c_str(), tx.vOut[j].amount);
		}
	}
	ImGui::EndChild();
}
