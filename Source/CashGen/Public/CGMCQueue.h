#pragma once

#include <mutex>

/**
* A multi consumer threadsafe queue
*/
template<class T, EQueueMode BaseQueueMode>
class TCGMcQueueBase final {
public:
	void Enqueue(T&& job) {
		bool success = queue_.Enqueue(std::move(job));
		check(success);
	}

	bool Dequeue(T& job) {
		// TQueue is not threadsafe for multiple consumers. We need to serialize consumers using a lock.
		// There is more performant implementations of MC queues, but we don't plan to use this queue
		// on a perf critical path.
		std::lock_guard<std::mutex> lock(consumerMutex_);
		return queue_.Dequeue(job);
	}

	bool IsEmpty() const {
		return queue_.IsEmpty();
	}

private:
	std::mutex consumerMutex_;
	TQueue<T, BaseQueueMode> queue_;
};

/**
* A multi producer multi consumer threadsafe queue.
*/
template<class T> using TCGMpmcQueue = TCGMcQueueBase<T, EQueueMode::Mpsc>;

/**
* A single producer multi consumer threadsafe queue.
*/
template<class T> using TCGSpmcQueue = TCGMcQueueBase<T, EQueueMode::Spsc>;
