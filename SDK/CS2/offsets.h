#pragma once
#include <cstdint>

namespace client {
	constexpr unsigned char m_iTeamNum = 0x3BF;
	constexpr uintptr_t pawn_handle = 0x60C; // player controller
	constexpr uintptr_t m_sSanitizedPlayerName = 0x640; //player controller
	constexpr uintptr_t NetworkVar_m_skeletonInstance = 0x310; // pawn
	constexpr uintptr_t m_iHealth = 0x32C; // pawn
	constexpr uintptr_t m_v3FeetPos = 0xCD8; // pawn
	constexpr uintptr_t CCSPlayer_WeaponServices = 0x10A8; // pawn
	constexpr uintptr_t CCSPlayer_ItemServices = 0x10B0; // pawn
	constexpr uintptr_t m_v3ViewAngles = 0x114C; // pawn
	constexpr uintptr_t m_pCCsWeaponBasePData = 0x360; // m_pActiveWeapon 
	constexpr uintptr_t m_pWeaponName = 0xC18; // m_pCCsWeaponBasePData
	constexpr uintptr_t m_v3HeadPos = 0x128C; // pawn
	constexpr uintptr_t m_pActiveWeapon = 0x12A8; // pawn
	constexpr uintptr_t m_bSpotted = 0x1638; // pawn
	constexpr uintptr_t m_bSpottedByMask = 0x163C; // pawn
	constexpr uintptr_t m_szLastPlaceName = 0x16EC; // pawn
	constexpr uintptr_t m_bInBuyZone = 0x1710; // pawn
	constexpr uintptr_t m_aimPunchAngle = 0x1714; // pawn
	constexpr uintptr_t m_aimPunchCache = 0x1738; // pawn
	constexpr uintptr_t m_bPawnIsAlive = 0x7F4; //bool, player controller

	constexpr uintptr_t CCSPlayer_WeaponServices_ActiveWeapon = 0x60;
	constexpr uintptr_t CCSPlayer_WeaponServices_PreviousActiveWeapon = 0x64;
} // namespace client