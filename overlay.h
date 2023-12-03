#pragma once
#include "misc.h"
#include "vector.h"
#include <barrier>
#include <d3d11.h>
#include <functional>
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
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class overlay
{
public:
	void draw_rect(vector2<float> xy, vector2<float> wh, vector4<float> rgba, float thickness = 1.f, float rounding = 0.f, uint32_t flags = 0x0);
	void draw_line(vector2<float> origin, vector2<float> destination, vector4<float> rgba, float thickness = 1.f);
	void draw_text(vector2<float> xy, vector4<float> rgba, std::string text);
	void draw_circle(vector2<float> xy, float radius, vector4<float> rgba, float thickness = 1.f, uint32_t segments = 32);
	void init_font(std::string font_path, float font_size);
	void message_loop();
	void set_parameters(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPSTR lp_cmd_line, int32_t n_show_cmd, uint32_t width, uint32_t height, std::string window_name);
	bool init(uint32_t number_of_rendering_threads);
	bool cleanup();
	void add_render_task(std::function<void(overlay*)> task);
	void render_loop();
	void swap_buffers();
	void execute_render_tasks();
	void wait_for_next_frame() {
		barrier->arrive_and_wait();

		this->frame_ready = false;
	}

	inline vector2<float> dimensions() const {
		return this->true_dimensions;
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
	
	void create_barrier(uint32_t number_of_threads){
		barrier = std::make_unique<misc::barrier>(number_of_threads, [this]() {
			this->render_loop();
												   });
	}

	static overlay* get() {
		static overlay* instance = new overlay();
		return instance;
	}
private:
	overlay() {
		ZeroMemory(&wc, sizeof(WNDCLASSEX));
		ZeroMemory(&device, sizeof(ID3D11Device));
		ZeroMemory(&device_context, sizeof(ID3D11DeviceContext));
		ZeroMemory(&render_target_view, sizeof(ID3D11RenderTargetView));
		ZeroMemory(&back_buffer, sizeof(ID3D11Texture2D));
		ZeroMemory(&swap_chain, sizeof(IDXGISwapChain));
		ZeroMemory(&swap_chain_desc, sizeof(DXGI_SWAP_CHAIN_DESC));
	}
	std::unique_ptr<misc::barrier> barrier;
	overlay(const overlay&) = delete;
	overlay(overlay&&) = delete;
	overlay& operator=(const overlay&) = delete;
	overlay& operator=(overlay&&) = delete;
	~overlay() = default;
	std::queue<std::function<void(overlay*)>> front_buffer_tasks;
	std::queue<std::function<void(overlay*)>> back_buffer_tasks;
	std::thread render_thread;
	std::vector<std::thread> worker_threads;
	vector2<float> true_dimensions, screen_dimensions;
	float _text_size;
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
