#include <iostream>
#include <cassert>

template <typename T>
class MyUniquePtr {
    T* ptr;

public:
    explicit MyUniquePtr(T* p = nullptr) : ptr(p) {}

    ~MyUniquePtr() {
        delete ptr;
    }

    MyUniquePtr(const MyUniquePtr&) = delete;
    MyUniquePtr& operator=(const MyUniquePtr&) = delete;

    MyUniquePtr(MyUniquePtr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    MyUniquePtr& operator=(MyUniquePtr&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    T* get() const { return ptr; }

    T* release() {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    void reset(T* p = nullptr) {
        delete ptr;
        ptr = p;
    }
};

template <typename T>
class MySharedPtr {
    T* ptr;
    int* ref_count;

public:
    explicit MySharedPtr(T* p = nullptr) : ptr(p), ref_count(new int(1)) {}

    MySharedPtr(const MySharedPtr& other) : ptr(other.ptr), ref_count(other.ref_count) {
        if (ptr) {
            (*ref_count)++;
        }
    }

    MySharedPtr& operator=(const MySharedPtr& other) {
        if (this != &other) {
            release();
            ptr = other.ptr;
            ref_count = other.ref_count;
            if (ptr) {
                (*ref_count)++;
            }
        }
        return *this;
    }
    MySharedPtr(MySharedPtr&& other) noexcept : ptr(other.ptr), ref_count(other.ref_count) {
        other.ptr = nullptr;
        other.ref_count = nullptr;
    }
    MySharedPtr& operator=(MySharedPtr&& other) noexcept {
        if (this != &other) {
            release();
            ptr = other.ptr;
            ref_count = other.ref_count;
            other.ptr = nullptr;
            other.ref_count = nullptr;
        }
        return *this;
    }

    ~MySharedPtr() {
        release();
    }

    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    T* get() const { return ptr; }

    int use_count() const { return (ref_count) ? *ref_count : 0; }

private:
    void release() {
        if (ref_count && --(*ref_count) == 0) {
            delete ptr;
            delete ref_count;
        }
    }
};

int main() {
    {
        MyUniquePtr<int> uptr1(new int(5));
        assert(*uptr1 == 5);
        std::cout << "MyUniquePtr value: " << *uptr1 << std::endl;

        MyUniquePtr<int> uptr2 = std::move(uptr1);
        assert(uptr1.get() == nullptr);
        assert(*uptr2 == 5);
    }
r
    {
        MySharedPtr<int> sptr1(new int(10));
        assert(sptr1.use_count() == 1);
        std::cout << "MySharedPtr initial value: " << *sptr1 << ", use count: " << sptr1.use_count() << std::endl;

        {
            MySharedPtr<int> sptr2 = sptr1;
            assert(sptr1.use_count() == 2);
            assert(sptr2.use_count() == 2);
            std::cout << "MySharedPtr value after copy: " << *sptr2 << ", use count: " << sptr2.use_count() << std::endl;
        }

        assert(sptr1.use_count() == 1);
    }

    std::cout << "All tests passed successfully!" << std::endl;
    return 0;
}
