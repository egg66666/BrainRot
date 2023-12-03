#include "brainrot_math.h"
#include "matrix.h"
#include "vector.h"

template <multiply_type multiply>
bool __fastcall world_to_screen(const vector3<float>& world_pos, vector2<float>& screen_pos,
								const matrix<float, 4, 4>& view_matrix, const vector2<float>& screen_size) 
{

	if constexpr (multiply == multiply_type::LEFT) {
		vector4<float> clip_coords;
		clip_coords.x = world_pos.x * view_matrix[0][0] + world_pos.y * view_matrix[0][1] + world_pos.z * view_matrix[0][2] + view_matrix[0][3];
		clip_coords.y = world_pos.x * view_matrix[1][0] + world_pos.y * view_matrix[1][1] + world_pos.z * view_matrix[1][2] + view_matrix[1][3];
		clip_coords.z = world_pos.x * view_matrix[2][0] + world_pos.y * view_matrix[2][1] + world_pos.z * view_matrix[2][2] + view_matrix[2][3];
		clip_coords.w = world_pos.x * view_matrix[3][0] + world_pos.y * view_matrix[3][1] + world_pos.z * view_matrix[3][2] + view_matrix[3][3];

		if (clip_coords.w < 0.1f)
			return false;

		clip_coords.x /= clip_coords.w;
		clip_coords.y /= clip_coords.w;
		clip_coords.z /= clip_coords.w;

		screen_pos.x = (screen_size.x / 2.0f) * clip_coords.x + (clip_coords.x + screen_size.x / 2.0f);
		screen_pos.y = -(screen_size.y / 2.0f) * clip_coords.y + (clip_coords.y + screen_size.y / 2.0f);
		return true;
	}
	else {
		vector4<float> clip_coords;

		clip_coords.x = world_pos.x * view_matrix[0][0] + world_pos.y * view_matrix[1][0] + world_pos.z * view_matrix[2][0] + view_matrix[3][0];
		clip_coords.y = world_pos.x * view_matrix[0][1] + world_pos.y * view_matrix[1][1] + world_pos.z * view_matrix[2][1] + view_matrix[3][1];
		clip_coords.z = world_pos.x * view_matrix[0][2] + world_pos.y * view_matrix[1][2] + world_pos.z * view_matrix[2][2] + view_matrix[3][2];
		clip_coords.w = world_pos.x * view_matrix[0][3] + world_pos.y * view_matrix[1][3] + world_pos.z * view_matrix[2][3] + view_matrix[3][3];

		if (clip_coords.w < 0.1f)
			return false;

		clip_coords.x /= clip_coords.w;
		clip_coords.y /= clip_coords.w;
		clip_coords.z /= clip_coords.w;

		screen_pos.x = (screen_size.x / 2.0f) * clip_coords.x + (clip_coords.x + screen_size.x / 2.0f);
		screen_pos.y = -(screen_size.y / 2.0f) * clip_coords.y + (clip_coords.y + screen_size.y / 2.0f);

		return true;
	}
	return false;
}

template bool __fastcall world_to_screen<multiply_type::LEFT>(const vector3<float>&, vector2<float>&, const matrix<float>&, const vector2<float>&);
template bool __fastcall world_to_screen<multiply_type::RIGHT>(const vector3<float>&, vector2<float>&, const matrix<float>&, const vector2<float>&);
