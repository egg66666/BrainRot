#include "overlay.h"

void overlay::draw_rect(vector2<float> xy, vector2<float> wh, vector4<float> rgba, float thickness, float rounding, uint32_t flags)
{

}

void overlay::add_render_task(std::function<void(overlay*)> task)
{
	std::unique_lock<std::mutex> lock(this->task_mutex);
	this->back_buffer_tasks.push(task);
}


void overlay::swap_buffers()
{
	std::unique_lock<std::mutex> lock(this->task_mutex);
	std::swap(this->front_buffer_tasks, this->back_buffer_tasks);
}

void overlay::execute_render_tasks()
{
	while (!this->front_buffer_tasks.empty()) {
		this->front_buffer_tasks.front()(this);
		this->front_buffer_tasks.pop();
	}
}
