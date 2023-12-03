#include <cstdint>
#include <Windows.h>
#include <cstdio>
#include "process.h"
#include "module.h"
#include "overlay.h"
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {



	AllocConsole();
	FILE* fDummy;
	freopen_s(&fDummy, "CONOUT$", "w", stdout);

	process& process = process::get();
	process.init("cs2.exe");
	process.hijack_handle();
	overlay* overlay = overlay::get();
	overlay->create_barrier(1);
	int a = 5;

}