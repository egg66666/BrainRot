#include "module.h"
#include "overlay_dx11.h"
#include "process.h"
#include <cstdint>
#include <cstdio>
#include <Windows.h>
#include "SDK/signature.h"
#include "SDK/CS2/sigs.h"
#include <print>
#include <thread>
#include <chrono>

void render() {
	overlay_dx11* overlay = overlay_dx11::get();
	float x = 0;
	while (true) {

		overlay->add_render_task([x](overlay_dx11* overlay) {
			
			overlay->draw_rect({ x, 0 }, { 100, 100 }, { 255, 0, 0, 255 });
								 });
		x += 0.1f;
		overlay->wait_for_next_frame();
	}
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	AllocConsole();
	FILE* fDummy;
	freopen_s(&fDummy, "CONOUT$", "w", stdout);
	process& process = process::get();
	process.init("cs2.exe");
	_module* cs = process["client.dll"];
	//process.hijack_handle();
	//overlay_dx11* overlay = overlay_dx11::get();
	//overlay->set_parameters(hInstance, hPrevInstance, lpCmdLine, nShowCmd, 1900, 1070, "brainrot");
	//overlay->init(1);
	//overlay->create_thread_with_tasks(render);
	//
	//overlay->message_loop();
	signature c_player_pawn = signature(cs, sigs::client::CGameEntitySystem);
	void* c_player_pawn_address = c_player_pawn.find_signature_from_file();
	int a = 5;
	
}