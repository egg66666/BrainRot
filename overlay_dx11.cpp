#include "overlay_dx11.h"
#include <dwmapi.h>

LRESULT CALLBACK WndProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
		return 0;
	}

	switch (uMsg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void overlay_dx11::init_font(std::string font_path, float font_size)
{
}

void overlay_dx11::message_loop()
{
	MSG msg{};
	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				cleanup();
				break;
			}
		}
	}

}

void overlay_dx11::set_parameters(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPSTR lp_cmd_line, int32_t n_show_cmd, uint32_t width, uint32_t height, std::string window_name)
{
	this->h_instance = h_instance;
	this->h_prev_instance = h_prev_instance;
	this->lp_cmd_line = lp_cmd_line;
	this->n_show_cmd = n_show_cmd;
	this->screen_dimensions = vector2<uint32_t>(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	this->overlay_dimensions = vector2<uint32_t>(width, height);
	this->window_name = window_name;
}

bool overlay_dx11::init(uint32_t number_of_rendering_threads)
{
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	ZeroMemory(&device, sizeof(ID3D11Device));
	ZeroMemory(&device_context, sizeof(ID3D11DeviceContext));
	ZeroMemory(&render_target_view, sizeof(ID3D11RenderTargetView));
	ZeroMemory(&back_buffer, sizeof(ID3D11Texture2D));
	ZeroMemory(&swap_chain, sizeof(IDXGISwapChain));
	ZeroMemory(&swap_chain_desc, sizeof(DXGI_SWAP_CHAIN_DESC));
	this->create_barrier(number_of_rendering_threads);
	this->wc.cbSize = sizeof(WNDCLASSEX);
	this->wc.style = CS_HREDRAW | CS_VREDRAW;
	this->wc.lpfnWndProc = WndProcess;
	this->wc.hInstance = this->h_instance;
	this->wc.lpszClassName = L"brainrot";

	if (this->create_window() && this->initialize_context()) {
		this->create_render_thread();
		this->create_thread_without_tasks([this]() {
			this->set_top_most();
			});
		return true;
	}

	return false;
}
bool overlay_dx11::initialize_context()
{
	DXGI_SWAP_CHAIN_DESC sd{};
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferDesc.RefreshRate.Numerator = 120;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.Width = this->overlay_dimensions.x;
	sd.BufferDesc.Height = this->overlay_dimensions.y;
	sd.SampleDesc.Count = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2;
	sd.OutputWindow = this->hwnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	constexpr D3D_FEATURE_LEVEL feature_levels[2] = { D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_11_1 };

	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, feature_levels, 2, D3D11_SDK_VERSION, &sd, &this->swap_chain, &this->device, &this->feature_level, &this->device_context);

	this->swap_chain->GetBuffer(0, IID_PPV_ARGS(&this->back_buffer));


	this->device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
	back_buffer->Release();

	ShowWindow(this->hwnd, this->n_show_cmd);
	UpdateWindow(this->hwnd);

	ImGui::SetCurrentContext(ImGui::CreateContext());

	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();
	this->_text_size = 16.f;
	ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", _text_size);

	io.Fonts->Build();

	ImGui_ImplWin32_Init(this->hwnd);
	ImGui_ImplDX11_Init(this->device, this->device_context);
	return true;
}

bool overlay_dx11::create_window()
{
	RegisterClassExW(&this->wc);
	this->hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
								 this->wc.lpszClassName, util::string::string_to_wstring(this->window_name).data(), WS_POPUP, (this->screen_dimensions.x - this->overlay_dimensions.x) / 2, (this->screen_dimensions.y - this->overlay_dimensions.y) / 2,
								 this->overlay_dimensions.x, this->overlay_dimensions.y, nullptr, nullptr,
								 this->wc.hInstance, nullptr);

	if (this->hwnd == nullptr) {
		return false;
	}
	SetWindowDisplayAffinity(this->hwnd, WDA_EXCLUDEFROMCAPTURE);
	SetLayeredWindowAttributes(this->hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);

	RECT client_area, window_area{};
	GetClientRect(this->hwnd, &client_area);
	GetWindowRect(this->hwnd, &window_area);
	POINT diff{};
	ClientToScreen(this->hwnd, &diff);
	const MARGINS margin = { window_area.left + (diff.x - window_area.left), window_area.top + (diff.y - window_area.top),
		client_area.right, client_area.bottom };

	DwmExtendFrameIntoClientArea(this->hwnd, &margin);
	return true;

}

bool overlay_dx11::cleanup()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	if (render_thread.joinable()) {
		render_thread.join();
	}
	for (auto& thread : this->worker_threads) {
		if (thread.joinable()) {
			thread.join();
		}
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	if (ImGui::GetCurrentContext() != nullptr) {
		ImGui::DestroyContext();
	}

	if (device)
		device->Release();
	if (wc.lpszClassName)
		UnregisterClass(wc.lpszClassName, h_instance);
}

void overlay_dx11::add_render_task(std::function<void(overlay_dx11*)> task)
{
	std::unique_lock<std::mutex> lock(this->task_mutex);
	this->back_buffer_tasks.push(task);
}

void overlay_dx11::render_loop()
{
	
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame();

	swap_buffers();
	execute_render_tasks();



	ImGui::Render();

	constexpr float clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	this->device_context->OMSetRenderTargets(1, &this->render_target_view, nullptr);
	device_context->ClearRenderTargetView(this->render_target_view, clear_color);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	this->swap_chain->Present(1, 0);
	ImGui::EndFrame();


	if (ImGui::GetIO().KeysDown[VK_END]) {
		_running = false;
		PostMessage(hwnd, WM_QUIT, 0, 0);
	}
}


void overlay_dx11::swap_buffers()
{
	std::unique_lock<std::mutex> lock(this->task_mutex);
	std::swap(this->front_buffer_tasks, this->back_buffer_tasks);
}

void overlay_dx11::execute_render_tasks()
{
	while (!this->front_buffer_tasks.empty()) {
		this->front_buffer_tasks.front()(this);
		this->front_buffer_tasks.pop();
	}
}


void overlay_dx11::create_render_thread()
{
}
