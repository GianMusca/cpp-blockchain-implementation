#pragma once

#include "allegro5/allegro.h"
#include "allegro5/allegro_primitives.h"
#include "allegro5/allegro_image.h"
#include "imgui.h"
#include "imgui_impl_allegro5.h"

class ImGuiHandler
{
public:
	ImGuiHandler(ImVec4 backColor = ImVec4(1.0f,1.0f,1.0f,1.0f));
	~ImGuiHandler();
	void start_frame(bool& running);
	void end_frame();
private:
	ALLEGRO_DISPLAY* display;
	ALLEGRO_EVENT_QUEUE* queue;
	ImVec4 backColor;
};

