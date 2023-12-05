#include "module.h"
#include "overlay_dx11.h"
#include "process.h"
#include <cstdint>
#include <cstdio>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <chrono>

void render_thread() {
	overlay_dx11* overlay = overlay_dx11::get();
	float x = 0;
	while (true) {

		overlay->add_render_task([x](overlay_dx11* overlay) {
			for (float idx = 0.f; idx < 1000.f; idx += 0.1f) {
				float i = rand() % 1000;
				overlay->draw_rect({ x + i, 0 }, { 100 + i, 100 + idx }, { 255, 0, 0, 255 });
			}
								 }
		);
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
	process.hijack_handle();
	overlay_dx11* overlay = overlay_dx11::get();
	overlay->set_parameters(hInstance, hPrevInstance, lpCmdLine, nShowCmd, 1918, 1078, "brainrot");
	overlay->init(1);
	overlay->create_thread_with_tasks(render_thread);
	overlay->message_loop();
	int a = 5;

}