#pragma once

#include <mutex>
#include <vector>
#include <condition_variable>

template<class T> class TCGBorrowedObject;

/**
* A pool of objects that can be borrowed and returned.
*
* This class is threadsafe.
* You can add and borrow objects from different threads concurrently.
*/
template<class T>
class TCGObjectPool final {
public:
	/**
	* Borrow an object from the pool. It will be removed from the pool
	* and you can use it. Once the TCGBorrowedObject is destructed,
	* it will automatically be put back into the object pool.
	*
	* If there is no object available in the pool, this will block
	* until there is one available.
	*/
	TCGBorrowedObject<T> Borrow(std::function<bool ()> shouldContinueToBlock) {
		return TCGBorrowedObject<T>(impl_, impl_->Borrow(std::move(shouldContinueToBlock)));
	}

	/**
	* Add a new object from the pool. After adding it, it can be borrowed.
	*/
	void Add(T* object) {
		impl_->Add(object);
	}

	// Copying and moving is forbidden. Because of Impl, this would
	// follow reference semantics and could be confusing.
	TCGObjectPool(const TCGObjectPool&) = delete;
	TCGObjectPool(TCGObjectPool&&) noexcept = delete;
	TCGObjectPool& operator=(const TCGObjectPool&) = delete;
	TCGObjectPool& operator=(TCGObjectPool&&) noexcept = delete;

	TCGObjectPool()
		: impl_(std::make_shared<Impl>()) {}

private:
	class Impl final {
	public:
		Impl() = default;

		// copying and moving is forbidden because there might be objects
		// borrowed from this pool that would then be put back into the wrong pool
		Impl(const Impl&) = delete;
		Impl(Impl&&) noexcept = delete;
		Impl& operator=(const Impl&) = delete;
		Impl& operator=(Impl&&) noexcept = delete;

		void Add(T* object) {
			std::lock_guard<std::mutex> lock(mutex_);
			freeObjects_.push_back(object);
			cv_.notify_one();
		}

		T* Borrow(std::function<bool()> shouldContinueToBlock) {
			std::unique_lock<std::mutex> lock(mutex_);
			do {
				// Block until an object becomes available.
				// Every 100ms, we check if shouldContinueToBlock still returns true. If not, we abort.
				if (cv_.wait_for(lock, std::chrono::milliseconds(100), [&]() { return !freeObjects_.empty(); })) {
					// We found an object. Borrow and return it.
					T* result = freeObjects_.back();
					freeObjects_.pop_back();
					return result;
				}
			} while (shouldContinueToBlock());

			// We didn't find an object and shouldContinueToBlock() returned false. Abort.
			throw std::runtime_error("Failed to borrow object from pool");
		}
	private:
		std::mutex mutex_;
		std::condition_variable cv_;
		std::vector<T*> freeObjects_;
	};

	friend class TCGBorrowedObject<T>;

	std::shared_ptr<Impl> impl_;
};

/**
* A handle to an object borrowed from the object pool.
* This class is *not* threadsafe. You cannot share TCGBorrowedObjects between threads.
*/
template<class T>
class TCGBorrowedObject final {
public:
	/**
	* Get a pointer to the borrowed object.
	*/
	T* Get() {
		T* result = impl_->Get();
		check(nullptr != result && "TCGBorrowedObject instance does not contain a borrowed object");
		return result;
	}

	T* operator->() {
		return Get();
	}

	/**
	* Return true if this instance contains a valid object.
	*/
	bool IsValid() const {
		return nullptr != impl_->Get();
	}

	/**
	* Return the borrowed object to the pool.
	*/
	void Release() {
		impl_->Release();
	}

	TCGBorrowedObject()
		: impl_(std::make_shared<BorrowedObjectImpl>(std::weak_ptr<typename TCGObjectPool<T>::Impl>(), nullptr)) {
	}

private:
	using ObjectPoolImpl = typename TCGObjectPool<T>::Impl;

	explicit TCGBorrowedObject(std::shared_ptr<ObjectPoolImpl> pool, T* object)
		: impl_(std::make_shared<BorrowedObjectImpl>(std::move(pool), object)) {
	}

	class BorrowedObjectImpl final {
	private:
		std::weak_ptr<ObjectPoolImpl> pool_;
		T* object_;
	public:
		T* Get() {
			return object_;
		}

		explicit BorrowedObjectImpl(std::weak_ptr<ObjectPoolImpl> pool, T* object)
			: pool_(std::move(pool)), object_(object) {
		}

		/**
		* Return the borrowed object to the pool.
		*/
		void Release() {
			if (nullptr != object_) {
				if (auto pool = pool_.lock()) {
					pool->Add(object_);
				}
				pool_.reset();
				object_ = nullptr;
			}
			check(pool_.expired() && "Class invariant: If object_ is nullptr, so must be pool_.");
		}

		/**
		* On destruction, we put the object back into the pool.
		*/
		~BorrowedObjectImpl() {
			Release();
		}

		// copying is forbidden because it would break the RAII pattern of putting
		// objects back into the pool.
		BorrowedObjectImpl(const BorrowedObjectImpl&) = delete;
		BorrowedObjectImpl& operator=(const BorrowedObjectImpl&) = delete;

		BorrowedObjectImpl(BorrowedObjectImpl&& rhs) noexcept
			: pool_(rhs.pool_), object_(rhs.object_) {
			// make sure the old BorrowedObjectImpl doesn't put anything back into the pool
			rhs.pool_ = nullptr;
			rhs.object_ = nullptr;
		}

		BorrowedObjectImpl& operator=(BorrowedObjectImpl&& rhs) noexcept {
			pool_ = rhs.pool_;
			object_ = rhs.object_;
			// make sure the old BorrowedObjectImpl doesn't put anything back into the pool
			rhs.pool_ = nullptr;
			rhs.object_ = nullptr;
		}
	};

	// We use shared_ptr to get refcounting when TCGBorrowedObjects are copied around
	std::shared_ptr<BorrowedObjectImpl> impl_;

	friend class TCGObjectPool<T>;
};
