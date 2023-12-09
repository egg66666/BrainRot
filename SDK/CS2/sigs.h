#pragma once
#include "../signature.h"
namespace sigs {

	namespace client {
		const char* CGameEntitySystem = "48 8B 0D ? ? ? ? 48 8D 54 24 68 E8 ? ? ? ? 48 85 C0";
		const char* CCSPlayerController = "48 8B 05 ? ? ? ? 48 85 C0 74 4F";
		const char* C_CsPlayerPawn = "48 89 0D ? ? ? ? 48 89 0D ? ? ? ? 89 0D ? ? ? ? 48 89 0D ? ? ? ? 89 0D";
		const char* dwViewMatrix = "48 8D 0D ? ? ? ? 48 C1 E0 06";

	} // namespace client
} // namespace sigs