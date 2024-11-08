#pragma once

#include "Layout.h"
#include "imgui.h"

typedef size_t index;

class layoutWindowHandler
{
public:
	layoutWindowHandler();
	void createWindow(const NodeData& callingOne, const Layout& layout, const vector<NodeData>& neighbourhood);
	void draw();
	void close();
private:
	string windowName;
	bool open;

	const NodeData* callingOne;
	const Layout* layout;
	const vector<NodeData>* neighbourhood;

	Layout formattedLayout;
	vector<NodeData> formattedNeighbourhood;

	int ratio;
	float lineBegin;

	ImDrawList* draw_list;
	ImU32 color;
	ImVec2 WindowPos;


	void formatLayout(); //asumo q quien llama es parte del Layout
	void formatNeighbourhood();



	ImVec2 Cindex2pos(index i);
	const NodeData& Cindex2data(index i);

	ImVec2 Vindex2pos(index i);
	const NodeData& Vindex2data(index i);

	void drawConnection(ImVec2 begin, ImVec2 end);
	void drawNode(const NodeData& node, ImVec2 pos);

	index findInFLayout(const NodeData* data);
	void replaceInFLayout(index a, index b);
	bool existInFLayout(const NodeData* data);


	void drawNodesInLayout();
	void drawNodesOutsideLayout();
};

