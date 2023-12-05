#pragma once


class IOverlay {
	virtual void render_loop() = 0;
	virtual bool init(uint32_t number_of_rendering_threads) = 0;
	virtual bool cleanup() = 0;
	virtual void message_loop() = 0;
	virtual void draw_rect(vector2<float> xy, vector2<float> wh, vector4<float> rgba, float thickness = 1.f, float rounding = 0.f, uint32_t flags = 0x0) = 0;
	virtual void draw_line(vector2<float> origin, vector2<float> destination, vector4<float> rgba, float thickness = 1.f) = 0;
	virtual void draw_text(vector2<float> xy, vector4<float> rgba, std::string text) = 0;
	virtual void draw_circle(vector2<float> xy, float radius, vector4<float> rgba, float thickness = 1.f, uint32_t segments = 32) = 0;
	virtual void render(std::function<void(IOverlay*)> fn) = 0;
};