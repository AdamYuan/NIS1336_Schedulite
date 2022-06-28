#ifndef SCHEDULITE_SEMAPHORE_HPP
#define SCHEDULITE_SEMAPHORE_HPP

#include <condition_variable>
#include <mutex>

class BinarySemaphore {
public:
	inline BinarySemaphore() : m_state(false) {}
	inline explicit BinarySemaphore(bool state) : m_state(state) {}

	inline void Wait() {
		std::unique_lock<std::mutex> lk(m_mtx);
		m_cv.wait(lk, [=] { return m_state; });
		m_state = false;
	}
	inline bool TryWait() {
		std::lock_guard<std::mutex> lk(m_mtx);
		if (m_state) {
			m_state = false;
			return true;
		} else {
			return false;
		}
	}
	inline void Signal() {
		std::lock_guard<std::mutex> lk(m_mtx);
		if (!m_state) {
			m_state = true;
			m_cv.notify_one();
		}
	}

private:
	bool m_state;
	std::mutex m_mtx;
	std::condition_variable m_cv;
};

#endif
