#pragma once
#include "misc.h"
#include "vector.h"
#include <barrier>
#include <functional>
#include <d3d11.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
#include "process.h"
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>
#include <Windows.h>
#include <algorithm>
#include <map>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class overlay_dx11
{
public:
	void draw_rect(vector2<float> xy, vector2<float> wh, vector4<float> rgba, float thickness = 1.f, float rounding = 0.f, uint32_t flags = 0) {
		if (xy.x < 0 || xy.y < 0 || xy.x + wh.x > overlay_dimensions.x || xy.y + wh.y > overlay_dimensions.y)
			return;
		ImVec4 color = ImVec4(rgba.x, rgba.y, rgba.z, rgba.w);

		ImGui::GetBackgroundDrawList()->AddRect(ImVec2(xy.x, xy.y), ImVec2(xy.x + wh.x, xy.y + wh.y), ImGui::ColorConvertFloat4ToU32(color), rounding, flags, thickness);
	}

	void health_bar(ImVec2 xy, ImVec2 wh_outer, ImVec4 outer_color, ImVec4 inner_color,
					uint32_t health, uint32_t max_health = 100,
					float thickness = 1.f, float rounding = 4.f, uint32_t flags = 0) {

		if (xy.x < 0 || xy.y < 0 || xy.x + wh_outer.x > overlay_dimensions.x || xy.y + wh_outer.y > overlay_dimensions.y)
			return;

		ImU32 _outer_color = ImGui::ColorConvertFloat4ToU32(outer_color);
		ImU32 _inner_color = ImGui::ColorConvertFloat4ToU32(inner_color);

		ImVec2 outer_rect_end = ImVec2(xy.x + wh_outer.x, xy.y + wh_outer.y);
		ImGui::GetBackgroundDrawList()->AddRect(xy, outer_rect_end, _outer_color, rounding, flags, thickness);

		float inner_rect_height = (static_cast<float>(std::min<uint32_t>(health, max_health)) / max_health) * wh_outer.y;
		ImVec2 inner_rect_start = ImVec2(xy.x + thickness, xy.y + wh_outer.y - inner_rect_height + thickness - 1);
		ImVec2 inner_rect_end = ImVec2(xy.x + wh_outer.x - thickness, xy.y + wh_outer.y - thickness + 1);

		ImGui::GetBackgroundDrawList()->AddRectFilled(inner_rect_start, inner_rect_end, _inner_color, rounding);
	}

	void draw_rect_filled(vector2<float> xy, vector2<float> wh, vector4<float> rgba, float rounding = 0.f, uint32_t flags = 0) {
		if (xy.x < 0 || xy.y < 0 || xy.x + wh.x > overlay_dimensions.x || xy.y + wh.y > overlay_dimensions.y)
			return;
		ImVec4 color = ImVec4(rgba.x / 255.0f, rgba.y / 255.0f, rgba.z / 255.0f, rgba.w / 255.0f);

		ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(xy.x, xy.y), ImVec2(xy.x + wh.x, xy.y + wh.y), ImGui::ColorConvertFloat4ToU32(color), rounding, flags);
	}

	void draw_line(vector2<float> origin, vector2<float> destination, vector4<float> rgba, float thickness = 1.f)
	{
		ImVec4 color = ImVec4(rgba.x / 255.0f, rgba.y / 255.0f, rgba.z / 255.0f, rgba.w / 255.0f);

		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(origin.x, origin.y), ImVec2(destination.x, destination.y), ImGui::ColorConvertFloat4ToU32(color), thickness);

	}

	void draw_text(vector2<float> xy, vector4<float> rgba, std::string text = "brainrotlib")
	{
		if (xy.x < 0 || xy.y < 0 || xy.x > overlay_dimensions.x || xy.y > overlay_dimensions.y)
			return;


		ImVec4 color = ImVec4(rgba.x / 255.0f, rgba.y / 255.0f, rgba.z / 255.0f, rgba.w / 255.0f);

		ImGui::GetBackgroundDrawList()->AddText(ImVec2(xy.x, xy.y), ImGui::ColorConvertFloat4ToU32(color), text.c_str());
	}

	void draw_circle(vector2<float> xy, float radius, vector4<float> rgba, float thickness = 1.f, uint32_t segments = 16)
	{
		if (xy.x - radius < 0 || xy.y - radius < 0 || xy.x + radius > overlay_dimensions.x || xy.y + radius > overlay_dimensions.y)
			return;
		ImVec4 color = ImVec4(rgba.x / 255.0f, rgba.y / 255.0f, rgba.z / 255.0f, rgba.w / 255.0f);

		ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(xy.x, xy.y), radius, ImGui::ColorConvertFloat4ToU32(color), segments, thickness);
	}
	void init_font(std::string font_path, float font_size);
	void message_loop();
	void set_parameters(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPSTR lp_cmd_line, int32_t n_show_cmd, uint32_t width, uint32_t height, std::string window_name);
	bool init(uint32_t number_of_rendering_threads);
	bool cleanup();
	void add_render_task(std::function<void(overlay_dx11*)> task);
	void render_loop();
	void swap_buffers();
	void execute_render_tasks();
	void wait_for_next_frame() {
		barrier->arrive_and_wait();

		this->frame_ready = false;
	}

	inline vector2<uint32_t> dimensions() const {
		return this->overlay_dimensions;
	}

	inline float text_size() const {
		return this->_text_size;
	}

	inline bool running() const {
		return this->_running;
	}

	template<typename Func, typename... Args>
	void create_thread_with_tasks(Func&& func, Args&&... args) {
		std::lock_guard<std::mutex> lock(threads_mutex);
		this->worker_threads.emplace_back(std::forward<Func>(func), std::forward<Args>(args)...);
	}


	template<typename Func, typename... Args>
	void create_thread_without_tasks(Func&& func, Args&&... args) {
		std::lock_guard<std::mutex> lock(threads_mutex);
		this->worker_threads.emplace_back(std::forward<Func>(func), std::forward<Args>(args)...);
	}

	void create_barrier(uint32_t number_of_threads) {
		barrier = std::make_unique<misc::barrier>(number_of_threads, [this]() {
			this->render_loop();
												  });
	}

	static overlay_dx11* get() {
		static overlay_dx11* instance = new overlay_dx11();
		return instance;
	}

	
	void set_top_most() {
		while (true) {

			if (GetForegroundWindow() == process.get_process_window()) {
				SetWindowPos(this->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
			else {
				SetWindowPos(this->hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 120));
		}
	}
private:
	overlay_dx11() {

	}
	std::unique_ptr<misc::barrier> barrier;
	overlay_dx11(const overlay_dx11&) = delete;
	overlay_dx11(overlay_dx11&&) = delete;
	overlay_dx11& operator=(const overlay_dx11&) = delete;
	overlay_dx11& operator=(overlay_dx11&&) = delete;
	~overlay_dx11() = default;
	bool initialize_context();
	bool create_window();
	void create_render_thread();
	std::queue<std::function<void(overlay_dx11*)>> front_buffer_tasks;
	std::queue<std::function<void(overlay_dx11*)>> back_buffer_tasks;
	std::thread render_thread;
	std::vector<std::thread> worker_threads;
	vector2<uint32_t> overlay_dimensions, screen_dimensions;
	float _text_size;
	std::string window_name;
	std::atomic<bool> frame_ready = false;
	int32_t n_show_cmd;
	char* lp_cmd_line;
	HWND hwnd;
	WNDCLASSEX wc;
	HINSTANCE h_instance;
	HINSTANCE h_prev_instance;
	ID3D11Device* device{};
	DXGI_SWAP_CHAIN_DESC* swap_chain_desc{};
	ID3D11DeviceContext* device_context{};
	ID3D11RenderTargetView* render_target_view{};
	ID3D11Texture2D* back_buffer{};
	IDXGISwapChain* swap_chain{};
	D3D_FEATURE_LEVEL feature_level{};
	std::mutex threads_mutex;
	std::mutex task_mutex;
	bool _running = true;
	process& process = process::get();
};