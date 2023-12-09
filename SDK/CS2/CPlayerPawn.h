#pragma once
#include "offsets.h"
#include "sigs.h"
#include "../vector.h"
#include "../SDKBase.h"
struct bone {
	vector3<float> position{};
	float radius{};
};

#pragma pack(push, 1)
struct active_weapon {
	char pad_0000[0x360];
	void* m_pCCsWeaponbasePData;
};
#pragma pack(pop)
#pragma pack(push, 1)
struct c_weapon_base {
	char pad_0000[0xC18];
	void* m_pActiveWeaponName;
};

struct weapon {
	char m_szWeaponName[32];
};
#pragma pack(push, 1)
struct skeleton_instance
{
	char pad_0x0000[0x1BC]; //0x0000
	uint32_t m_iBoneCount; //0x01BC 
	char pad_0x01C0[0x18]; //0x01C0
	uint32_t m_iMask; //0x01D8 
	char pad_0x01DC[0x4]; //0x01DC
	void* bone_cache; //0x01E0
};
#pragma pack(pop)

#pragma pack(push, 1)
struct c_player_pawn_raw {
	//DEFINE_MEMBER_N(void*, m_skeletonInstance, client::NetworkVar_m_skeletonInstance);
	//DEFINE_MEMBER_N(uint32_t, m_iHealth, client::m_iHealth);
	//DEFINE_MEMBER_N(vector3<float>, m_v3FeetPos, client::m_v3FeetPos);
	//DEFINE_MEMBER_N(void*, CCSPlayer_WeaponServices, client::CCSPlayer_WeaponServices);
	//DEFINE_MEMBER_N(void*, CCSPlayer_ItemServices, client::CCSPlayer_ItemServices);
	//DEFINE_MEMBER_N(vector3<float>, m_v3ViewAngles, client::m_v3ViewAngles);
	//DEFINE_MEMBER_N(vector3<float>*, m_v3HeadPos, client::m_v3HeadPos);
	//DEFINE_MEMBER_N(void*, m_pActiveWeapon, client::m_pActiveWeapon);
	//DEFINE_MEMBER_N(bool, m_bSpotted, client::m_bSpotted);
	//DEFINE_MEMBER_N(bool, m_bSpottedByMask, client::m_bSpottedByMask);
	//DEFINE_MEMBER_N(char*, m_szLastPlaceName, client::m_szLastPlaceName);
	//DEFINE_MEMBER_N(bool, m_bInBuyZone, client::m_bInBuyZone);
	//DEFINE_MEMBER_N(vector3<float>, m_aimPunchAngle, client::m_aimPunchAngle);
	//DEFINE_MEMBER_N(vector3<float>, m_aimPunchCache, client::m_aimPunchCache);
	char data[client::m_aimPunchCache];
};
#pragma pack(pop)

class c_player_pawn : public sdk_base {
	signature pawn_sig = signature(_process["client.dll"], sigs::client::C_CsPlayerPawn);
	void* pawn_address = pawn_sig.find_signature_from_file();
	c_player_pawn_raw pawn;
	void* raw_pawn;
	std::array<bone, 64> bones{};
	active_weapon _active_weapon{};
	c_weapon_base _weapon_base{};
	weapon _weapon{};
	MEMBER_ACCESSOR(this->pawn.data, void*, m_skeletonInstance, client::NetworkVar_m_skeletonInstance);
	MEMBER_ACCESSOR(this->pawn.data, uint32_t, m_iHealth, client::m_iHealth);
	MEMBER_ACCESSOR(this->pawn.data, vector3<float>, m_v3FeetPos, client::m_v3FeetPos);
	MEMBER_ACCESSOR(this->pawn.data, void*, CCSPlayer_WeaponServices, client::CCSPlayer_WeaponServices);
	MEMBER_ACCESSOR(this->pawn.data, void*, CCSPlayer_ItemServices, client::CCSPlayer_ItemServices);
	MEMBER_ACCESSOR(this->pawn.data, vector3<float>, m_v3ViewAngles, client::m_v3ViewAngles);
	MEMBER_ACCESSOR(this->pawn.data, vector3<float>*, m_v3HeadPos, client::m_v3HeadPos);
	MEMBER_ACCESSOR(this->pawn.data, void*, m_pActiveWeapon, client::m_pActiveWeapon);
	MEMBER_ACCESSOR(this->pawn.data, bool, m_bSpotted, client::m_bSpotted);
	MEMBER_ACCESSOR(this->pawn.data, bool, m_bSpottedByMask, client::m_bSpottedByMask);
	MEMBER_ACCESSOR(this->pawn.data, char*, m_szLastPlaceName, client::m_szLastPlaceName);
	MEMBER_ACCESSOR(this->pawn.data, bool, m_bInBuyZone, client::m_bInBuyZone);
	MEMBER_ACCESSOR(this->pawn.data, vector3<float>, m_aimPunchAngle, client::m_aimPunchAngle);
	MEMBER_ACCESSOR(this->pawn.data, vector3<float>, m_aimPunchCache, client::m_aimPunchCache);
};