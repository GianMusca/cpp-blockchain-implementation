#include "TreeWindowHandler.h"

TreeWindowHandler::TreeWindowHandler() :
	windowList(), nodeId("NO_DATA")
{
}

void TreeWindowHandler::setId(const string& id)
{
	nodeId = id;
}

void TreeWindowHandler::createWindow(const Block& block)
{
	bool found = false;
	size_t foundV = 0;
	size_t i;

	for (i = 0; (found == false) && (i < windowList.size()); i++) {
		if (windowList[i].orden == block.getHeight()) {
			found = true;
			foundV = i;
		}
	}

	if (found) {
		windowList[foundV].window.replaceBlock(block, nodeId);
	}
	else {
		TreeWindow temp(block, nodeId);

		windowList.emplace_back(block.getHeight(), temp);
	}

	//auto it = windowList.find(block.getHeight()); //because iterators are a pain in the... neck
	//if (it == windowList.end())
	//	windowList.emplace(block.getHeight(), (block, nodeId));
	//else
	//	it->second.replaceBlock(block,nodeId);
}

void TreeWindowHandler::draw()
{
	//if (windowList.empty() == false) {
	//	map<unsigned int, TreeWindow>::iterator closing, it; //auto no me salvo de esta
	//	bool closeWindow = false;

	//	for (it = windowList.begin(); it != windowList.end(); it++) {
	//		it->second.draw();
	//		if (it->second.isOpen() == false) {
	//			closeWindow = true;
	//			closing = it;
	//		}
	//	}

	//	if (closeWindow == true) {
	//		windowList.erase(closing);
	//	}

	//}

	if (windowList.empty() == false) {
		size_t closing, it;
		bool closeWindow = false;

		for (it = 0; it < windowList.size(); it++) {
			windowList[it].window.draw();
			if (windowList[it].window.isOpen() == false) {
				closeWindow = true;
				closing = it;
			}
		}

		if (closeWindow == true)
			windowList.erase(windowList.begin() + closing);
	}
}
