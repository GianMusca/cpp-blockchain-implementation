#include "TreeWindow.h"
#include "imgui.h"
#include "printTXroutine.h"

#define verticalStep 25.
#define horizontalStep 25.

#define LEAF	ImVec4(0.0f,1.0f,0.0f,1.0f)
#define NODE	ImVec4(0.5f,0.5f,0.5f,1.0f)
#define ROOT	ImVec4(1.0f,0.0f,0.0f,1.0f)
#define CLEAF	ImVec4(0.0f,0.5f,0.0f,1.0f)
#define CNODE	ImVec4(0.25f,0.25f,0.25f,1.0f)

TreeWindow::TreeWindow(const TreeWindow& c) :
	windowName(c.windowName),open(true),block(c.block),
	validRoot(false), tree()
{
	tree = block.getMerkleTree();
	validateRoot();
}

TreeWindow::TreeWindow(const Block& b, const string& nodeId)
	: windowName(), open(true), block(b),
	validRoot(false), tree()
{
	windowName = "Block: " + block.getBlockID() + "##" + nodeId + "_" + to_string(block.getHeight());
	tree = block.getMerkleTree();
	validateRoot();
}

void TreeWindow::replaceBlock(const Block& b, const string& nodeId)
{
	block = b;
	windowName = block.getBlockID() + "##" + nodeId + "_" + to_string(block.getHeight());
	tree = block.getMerkleTree();
	validateRoot();
}

void TreeWindow::draw()
{
	ImGui::Begin(windowName.c_str(), &open);

	if (validRoot == false) {
		ImGui::Text("PRECAUCION! El merkle Root generado por Block (en el Tree) no coincide con el campo Merkle Root del mismo\n :<");
	}

	if (ImGui::TreeNode("Data")) {
		ImGui::Text("HEIGHT: %lu", block.getHeight());
		ImGui::Text("ID: %s", block.getBlockID().c_str());
		ImGui::Text("PREVIOUS BLOCK ID: %s", block.getPreviousBlockID().c_str());
		ImGui::Text("AMOUNT OF TX: %lu", block.getNTx());
		ImGui::Text("NONCE: %lu", block.getNonce());
		ImGui::Text("MERKLE ROOT: %s", block.getMerkleRoot());
		ImGui::TreePop();
	}

	if (ImGui::CollapsingHeader("TX")) {
		if (block.getNTx() > 0) {
			ImGui::Text(" ");
			ImGui::SameLine();
			ImGui::BeginChild("Txs",ImVec2(CCHILD_W,CCHILD_H));

			const vector<Transaction>& TXlist = block.getTransactions();

			for (int j = 0; j < TXlist.size(); j++)
				printTx(TXlist[j], j);

			ImGui::EndChild();
		}
		else {
			ImGui::Text("There are no TX in this block...\n...and that just doesn't feel right...");
		}
	}

	drawTree();

	ImGui::End();
}

bool TreeWindow::isOpen()
{
	return open;
}

void TreeWindow::validateRoot(void)
{
	if (block.getMerkleRoot() == tree.back()) {
		validRoot = true;
	}
	else {
		validRoot = false;
	}
}

void TreeWindow::drawTree(void)
{
	if (ImGui::TreeNode("Merkle Tree")) {
		ImGui::BeginChild("child", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
		{
			longN leafs = block.getNTx();
			longN height = (longN)ceil(log2(leafs)) + 1;
			longN width = 1 << (height - 1);
			longN floor;
			longN uniques = leafs;
			vector<newIDstr>::iterator it = tree.begin();

			for (floor = height; floor > 0; floor--, uniques = (uniques + (uniques & 1)) / 2) {
				drawFloor(height, floor, uniques, it);
			}
		}
		ImGui::EndChild();
		ImGui::TreePop();
	}
}

void TreeWindow::drawFloor(const longN totalHeight, const longN floor, const longN uniques, vector<newIDstr>::iterator& it)
{
	longN nodes;
	if (uniques == 1)
		nodes = 1;
	else
		nodes = uniques + (uniques & 1);
	double offset = exp2((signed long)(totalHeight - floor - 1)) * horizontalStep;
	double distance = 2 * offset;
	double verPos = verticalStep * floor;
	double horPos = offset;
	ImVec4 color;
	for (longN i = 0; i < nodes; i++, it++, horPos += distance) {

		if (floor == 1) {
			color = ROOT;
		}
		else if (floor == totalHeight) {
			if (i < uniques)
				color = LEAF;
			else
				color = CLEAF;
		}
		else {
			if (i < uniques)
				color = NODE;
			else
				color = CNODE;
		}

		ImGui::PushID(i);
		ImGui::PushStyleColor(ImGuiCol_Button, color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);

		ImGui::SetCursorPos(ImVec2(horPos, verPos));
		ImGui::Button("  ");
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Merkel: %s", it->c_str());
		}

		ImGui::PopStyleColor(3);
		ImGui::PopID();


	}
}
