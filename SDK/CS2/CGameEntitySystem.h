#pragma once
#include "../SDKBase.h"
#include "offsets.h"
#include "sigs.h"
#include <vector>
class c_player_pawn;
class c_local_player;
class c_player_controller;

class c_entity_system : public sdk_base
{
private:
	signature entity_system = signature(_process["client.dll"], sigs::client::CGameEntitySystem);
	process& _process = process::get();
	std::vector<std::unique_ptr<c_player_pawn>> pawns{ 12 };
	std::unique_ptr<c_local_player> local_player = std::make_unique<c_local_player>();
	std::vector<std::unique_ptr<c_player_controller>> controllers{ 12 };
	bool update() override;
	c_entity_system();
	c_entity_system(const c_entity_system&) = delete;
	bool init() override;
public:
	static c_entity_system* get();
	c_local_player* get_local_player();
	uint8_t get_player_count();
	c_player_pawn* get_pawn_by_idx(uint8_t idx);
	c_player_controller* get_controller_by_idx(uint8_t idx);
};