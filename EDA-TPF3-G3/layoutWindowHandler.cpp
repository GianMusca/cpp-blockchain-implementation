#include "layoutWindowHandler.h"

#define WINDOW_W    600.
#define WINDOW_H    500.

#define CIRCLE_X    200.
#define CIRCLE_Y    200.
#define CIRCLE_R    150.

#define LINE_X (CIRCLE_X+CIRCLE_R+80.)
#define LINE_D  40.

#define OFFSET_X 10.5
#define OFFSET_Y 8.

#include <cmath>
#define PI 3.14159265


layoutWindowHandler::layoutWindowHandler()
	: windowName("NO DATA"), open(false),
	layout(nullptr), neighbourhood(nullptr),
	formattedLayout(), formattedNeighbourhood(),
	callingOne(),
	ratio(1), lineBegin(CIRCLE_Y),

	draw_list(nullptr), WindowPos(0, 0),
	color(ImColor(ImVec4(1, 1, 1, 1)))
{
}

void layoutWindowHandler::createWindow(const NodeData& calling, const Layout& _layout, const vector<NodeData>& _neighbourhood)
{
	open = true;
	callingOne = &calling;
	layout = &_layout;
	neighbourhood = &_neighbourhood;
	windowName = "Red de conexiones de " + calling.getID() + "##LAYOUT";

	formatLayout();
	formatNeighbourhood();

}

void layoutWindowHandler::draw()
{
	if (open) {
		ImGui::Begin(windowName.c_str(), &open, ImVec2(WINDOW_W, WINDOW_H));

		draw_list = ImGui::GetWindowDrawList();
		WindowPos = ImGui::GetWindowPos();

		ImGui::PushID(1);
		ImGui::PushStyleColor(ImGuiCol_Button, color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);

		drawNodesInLayout();
		drawNodesOutsideLayout();

		ImGui::PopStyleColor(3);
		ImGui::PopID();


		ImGui::End();
	}
}

void layoutWindowHandler::close()
{
	open = false;
}

void layoutWindowHandler::formatLayout()
{
	formattedLayout = *layout;
	index calling = findInFLayout(callingOne);
	replaceInFLayout(calling, 0);
}

void layoutWindowHandler::formatNeighbourhood()
{
	formattedNeighbourhood.clear();
	for (size_t i = 0; i < neighbourhood->size(); i++) {
		if (existInFLayout(&(*neighbourhood)[i]) == false)
			formattedNeighbourhood.emplace_back((*neighbourhood)[i]);
		else
			formattedLayout[findInFLayout(&(*neighbourhood)[i])].myNeighbours.emplace_back(*callingOne);
	}
}

ImVec2 layoutWindowHandler::Cindex2pos(index i)
{
	return ImVec2(CIRCLE_X + CIRCLE_R * cos(2 * PI * i / ratio), CIRCLE_X + CIRCLE_R * sin(2 * PI * i / ratio));
}

const NodeData& layoutWindowHandler::Cindex2data(index i)
{
	return formattedLayout[i].ownData;
}


ImVec2 layoutWindowHandler::Vindex2pos(index i)
{
	return ImVec2(LINE_X, lineBegin + i * LINE_D);
}

const NodeData& layoutWindowHandler::Vindex2data(index i)
{
	return formattedNeighbourhood[i];
}

void layoutWindowHandler::drawConnection(ImVec2 begin, ImVec2 end)
{
	begin.x += WindowPos.x;
	begin.y += WindowPos.y;

	end.x += WindowPos.x;
	end.y += WindowPos.y;

	draw_list->AddLine(begin, end, color);
}

void layoutWindowHandler::drawNode(const NodeData& node, ImVec2 pos)
{
	pos.x -= OFFSET_X;
	pos.y -= OFFSET_Y;

	ImGui::SetCursorPos(pos);
	ImGui::Button("  ");
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("ID: %s\nPORT: %s\nIP: %s", node.getID().c_str(), node.getSocket().getPortString().c_str(), node.getSocket().getIPString().c_str());
}

index layoutWindowHandler::findInFLayout(const NodeData* data)
{
	index rta = 0;
	bool found = false;
	for (index i = 0; (found == false) && (i < formattedLayout.size()); i++) {
		if (formattedLayout[i] == *data) {
			found = true;
			rta = i;
		}
	}
	return rta;
}

void layoutWindowHandler::replaceInFLayout(index a, index b)
{
	if ((a != b) &&
		(a < formattedLayout.size()) &&
		(b < formattedLayout.size())) {

		Connection temp(formattedLayout[a]);
		formattedLayout[a] = formattedLayout[b];
		formattedLayout[b] = temp;

	}
}

bool layoutWindowHandler::existInFLayout(const NodeData* data)
{
	bool found = false;
	for (index i = 0; (found == false) && (i < formattedLayout.size()); i++) {
		if (formattedLayout[i] == *data)
			found = true;
	}
	return found;
}

void layoutWindowHandler::drawNodesInLayout()
{
	ratio = formattedLayout.size();
	if (ratio > 0) {

		for (size_t i = 0; i < formattedLayout.size(); i++) {
			drawNode(formattedLayout[i].ownData, Cindex2pos(i));
			for (size_t j = 0; j < formattedLayout[i].myNeighbours.size(); j++) {
				drawConnection(Cindex2pos(i), Cindex2pos(findInFLayout(&(formattedLayout[i].myNeighbours[j]))));
			}
		}
	}
}

void layoutWindowHandler::drawNodesOutsideLayout()
{
	if (formattedNeighbourhood.size() > 0) {
		lineBegin = CIRCLE_Y - LINE_D * (formattedNeighbourhood.size() - 1) / 2.;

		for (size_t i = 0; i < formattedNeighbourhood.size(); i++) {
			drawNode(formattedNeighbourhood[i], Vindex2pos(i));
			drawConnection(Vindex2pos(i), Cindex2pos(0));
		}

	}
}
