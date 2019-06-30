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
	TCGBorrowedObject<T> Borrow() {
		std::unique_lock<std::mutex> lock(mutex_);
		cv_.wait(lock, [&]() {
			return !freeObjects_.empty();
		});
		TCGBorrowedObject<T> result(this, freeObjects_.back());
		freeObjects_.pop_back();
		return result;
	}

	/**
	* Add a new object from the pool. After adding it, it can be borrowed.
	*/
	void Add(T* object) {
		std::lock_guard<std::mutex> lock(mutex_);
		freeObjects_.push_back(object);
		cv_.notify_one();
	}

	TCGObjectPool() = default;

	// copying and moving is forbidden because there might be objects
	// borrowed from this pool that would then be put back into the wrong pool
	TCGObjectPool(const TCGObjectPool&) = delete;
	TCGObjectPool(TCGObjectPool&&) noexcept = delete;
	TCGObjectPool& operator=(const TCGObjectPool&) = delete;
	TCGObjectPool& operator=(TCGObjectPool&&) noexcept = delete;

private:
	std::mutex mutex_;
	std::condition_variable cv_;
	std::vector<T*> freeObjects_;
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
		: impl_(std::make_shared<BorrowedObjectImpl>(nullptr, nullptr)) {
	}

private:
	explicit TCGBorrowedObject(TCGObjectPool<T>* pool, T* object)
		: impl_(std::make_shared<BorrowedObjectImpl>(pool, object)) {
	}

	class BorrowedObjectImpl final {
	private:
		TCGObjectPool<T>* pool_;
		T* object_;
	public:
		T* Get() {
			return object_;
		}

		explicit BorrowedObjectImpl(TCGObjectPool<T>* pool, T* object)
			: pool_(pool), object_(object) {
		}

		/**
		* Return the borrowed object to the pool.
		*/
		void Release() {
			if (nullptr != object_) {
				pool_->Add(object_);
				pool_ = nullptr;
				object_ = nullptr;
			}
			check(nullptr == pool_ && "Class invariant: If object_ is nullptr, so must be pool_.");
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
