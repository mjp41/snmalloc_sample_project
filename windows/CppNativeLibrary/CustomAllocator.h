// Author: Neeraj Sharma <neerajsharma.9@outlook.com>
// License MIT

#pragma once

#include "SnmallocWrapper.h"

#include <iostream>
#include <thread>

namespace NativeLibrary
{
    template <class T>
    struct CustomAllocator
    {
        typedef T value_type;

        CustomAllocator()
        {
            OutStream() << "CustomAllocator()" << std::endl;
        }

        template <class U> constexpr CustomAllocator(const CustomAllocator <U>&) noexcept
        {
            OutStream() << "CustomAllocator(const CustomAllocator <U>&)" << std::endl;
        }

        CustomAllocator(CustomAllocator const&)
        {
            OutStream() << "CustomAllocator(CustomAllocator const&)" << std::endl;
        }

        ~CustomAllocator()
        {
            OutStream() << "~CustomAllocator()" << std::endl;
        }

        [[nodiscard]] T* allocate(std::size_t n)
        {
            OutStream() << "allocate(" << n << ")" << std::endl;
            if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
            {
                throw std::bad_array_new_length();
            }

            //if (auto p = static_cast<T*>(std::malloc(n * sizeof(T)))) {
            if (auto p = static_cast<T*>(::SNMALLOC_NAME_MANGLE(malloc)(n * sizeof(T))))
            {
                report(p, n);
                OutStream() << "  allocate(" << n << ") = " << p << std::endl;
                return p;
            }

            throw std::bad_alloc();
        }

        void deallocate(T* p, std::size_t n) noexcept
        {
            OutStream() << "deallocate(" << p << ", " << n << ")" << std::endl;
            report(p, n, 0);
            //std::free(p);
            ::SNMALLOC_NAME_MANGLE(free)(p);
        }

    private:
        void report(T* p, std::size_t n, bool alloc = true) const
        {
            OutStream() << " " << (alloc ? "Alloc: " : "Dealloc: ") << sizeof(T) * n
                << " bytes at " << std::hex << std::showbase
                << reinterpret_cast<void*>(p) << std::dec << '\n';
        }

        std::ostream& OutStream()
        {
            return std::cout << "[" << GetThreadId() << ", " << this << "] ";
        }

        std::ostream& OutStream() const
        {
            return std::cout << "[" << GetThreadId() << ", " << this << "] ";
        }

        std::thread::id GetThreadId() const
        {
            return std::this_thread::get_id();
        }
    };
}