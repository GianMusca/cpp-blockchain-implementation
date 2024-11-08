#include "ImGuiHandler.h"

#define SCREEN_WIDTH	600
#define SCREEN_HEIGHT	300

ImGuiHandler::ImGuiHandler(ImVec4 bColor) : backColor(bColor)
{
	al_init();
	al_install_keyboard();
	al_install_mouse();
	al_init_primitives_addon();
	al_init_image_addon();
	al_set_new_display_flags(ALLEGRO_RESIZABLE);
	display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
	al_set_window_title(display, "TP FINAL PARTE 3");
	queue = al_create_event_queue();
	al_register_event_source(queue, al_get_display_event_source(display));
	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_mouse_event_source());

	//Inicializa ImGui
	IMGUI_CHECKVERSION(); //?
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.Fonts->AddFontFromFileTTF("BebasNeue_Regular.ttf", 20.0f);

	ImGui::StyleColorsDark();
	ImGui_ImplAllegro5_Init(display);
}

ImGuiHandler::~ImGuiHandler()
{
	ImGui_ImplAllegro5_Shutdown();
	ImGui::DestroyContext();
	al_destroy_event_queue(queue);
	al_destroy_display(display);
}

void ImGuiHandler::start_frame(bool& running)
{
	ALLEGRO_EVENT ev;
	while (al_get_next_event(queue, &ev)) {
		ImGui_ImplAllegro5_ProcessEvent(&ev);
		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			running = false;
		if (ev.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
			ImGui_ImplAllegro5_InvalidateDeviceObjects();
			al_acknowledge_resize(display);
			ImGui_ImplAllegro5_CreateDeviceObjects();
		}
	}

	ImGui_ImplAllegro5_NewFrame();
	ImGui::NewFrame();
}

void ImGuiHandler::end_frame()
{
	ImGui::Render();
	al_clear_to_color(al_map_rgba_f(backColor.x, backColor.y, backColor.z, backColor.w));
	ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
	al_flip_display();
}
