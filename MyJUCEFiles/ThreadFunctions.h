#pragma once

#include <atomic>

// source: https://www.youtube.com/watch?v=Q0vrQFyAdWI&t=130s&ab_channel=JUCE
class spin_lock {
public:
	void lock() noexcept {
		while (flag.test_and_set()); //spins
	}

	void unlock() noexcept {
		flag.clear();
	}

	bool try_lock() noexcept {
		return !flag.test_and_set();
	}

private:
	std::atomic_flag flag = ATOMIC_FLAG_INIT;
};