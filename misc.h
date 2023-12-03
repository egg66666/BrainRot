#pragma once
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>


// anything that has no real place in the project goes in here. usually stuff that is used in multiple places and doesnt really fit into util.h
namespace misc {
	// do you hate your fucking life? you do? did you also try to rape a std::barrier into accepting any function for 2 hours and it kept fucking you with its comptime checks? well this is the shit u been waiting for. my superior version that shifts all the blame of fucking up on u
	class barrier {
	public:
		barrier(int count, std::function<void()> func)
			: thread_count(count), call_function(func), counter(0),
			is_function_called(false), is_barrier_active(true) {}

		void arrive_and_drop() {
			std::unique_lock<std::mutex> lock(mtx);
			is_barrier_active = false;
			if (!is_function_called) {
				call_function();
				is_function_called = true;
			}
			counter = 0;
			cv.notify_all();
		}

		void arrive_and_wait() {
			std::unique_lock<std::mutex> lock(mtx);

			if (!is_barrier_active) {
				return;
			}

			counter++;
			if (counter == thread_count) {
				call_function();
				is_function_called = true;
				counter = 0;
				cv.notify_all();
			}
			else {
				cv.wait(lock, [this] { return is_function_called || !is_barrier_active; });
				if (is_barrier_active && thread_count != counter) {
					is_function_called = false;
				}
			}
		}

	private:
		std::mutex mtx;
		std::condition_variable cv;
		int thread_count;
		int counter;
		std::function<void()> call_function;
		bool is_function_called;
		bool is_barrier_active;
	};
}