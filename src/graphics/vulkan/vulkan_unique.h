#pragma once

#include <vulkan/vulkan_core.h>

#include <functional>
#include <utility>

namespace moth_graphics::graphics::vulkan {
    /// Move-only RAII wrapper for a Vulkan-style handle. The deleter is type-erased
    /// so any of the destroy/free/Vma* functions can be supplied at construction
    /// (typically as a small lambda capturing the device or allocator).
    ///
    /// The wrapped handle is stored as type Handle{} when empty, which maps to
    /// VK_NULL_HANDLE for non-dispatchable handles and nullptr for pointer-typed
    /// handles such as VmaAllocation/VmaAllocator.
    template <typename Handle>
    class UniqueHandle {
    public:
        using Deleter = std::function<void(Handle)>;

        UniqueHandle() = default;
        UniqueHandle(Handle handle, Deleter deleter)
            : m_handle(handle)
            , m_deleter(std::move(deleter)) {
        }

        ~UniqueHandle() { Reset(); }

        UniqueHandle(UniqueHandle const&) = delete;
        UniqueHandle& operator=(UniqueHandle const&) = delete;

        UniqueHandle(UniqueHandle&& other) noexcept
            : m_handle(other.m_handle)
            , m_deleter(std::move(other.m_deleter)) {
            other.m_handle = Handle{};
        }

        UniqueHandle& operator=(UniqueHandle&& other) noexcept {
            if (this != &other) {
                Reset();
                m_handle = other.m_handle;
                m_deleter = std::move(other.m_deleter);
                other.m_handle = Handle{};
            }
            return *this;
        }

        Handle Get() const { return m_handle; }
        operator Handle() const { return m_handle; } // NOLINT(google-explicit-constructor)
        explicit operator bool() const { return m_handle != Handle{}; }

        Handle Release() {
            Handle h = m_handle;
            m_handle = Handle{};
            return h;
        }

        void Reset() {
            if (m_handle != Handle{} && m_deleter) {
                m_deleter(m_handle);
            }
            m_handle = Handle{};
        }

    private:
        Handle m_handle{};
        Deleter m_deleter;
    };
}
