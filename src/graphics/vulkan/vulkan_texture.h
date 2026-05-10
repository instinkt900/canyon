#pragma once

#include "moth_graphics/graphics/itexture.h"
#include "moth_graphics/graphics/vulkan/vulkan_surface_context.h"
#include "vulkan_unique.h"

#include <vulkan/vulkan_core.h>

#include <memory>
#include <filesystem>
#include <cstdint>

namespace moth_graphics::graphics::vulkan {
    class Texture : public ITexture {
    public:
        static std::unique_ptr<Texture> FromFile(SurfaceContext& context, std::filesystem::path const& path);
        static std::unique_ptr<Texture> FromRGBA(SurfaceContext& context, int width, int height, unsigned char const* pixels);
        Texture(SurfaceContext& context);
        Texture(SurfaceContext& context, VkImage image, VkImageView view, VkExtent2D extent, VkFormat format, bool owning = true);
        Texture(SurfaceContext& context, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties = 0, bool owning = true);
        ~Texture();

        uint32_t GetId() const { return m_id; }

        VkImage GetVkImage() const { return m_vkImage; }
        VkExtent2D GetVkExtent() const { return m_vkExtent; }
        VkFormat GetVkFormat() const { return m_vkFormat; }
        VkImageView GetVkView() const;
        VkSampler GetVkSampler() const;
        VkDescriptorSet GetDescriptorSet() const;

        void* Map();
        void Unmap();

        Texture(Texture const&) = delete;
        Texture& operator=(Texture const&) = delete;

        int GetWidth() const override { return m_vkExtent.width; }
        int GetHeight() const override { return m_vkExtent.height; }
        void SetFilter(TextureFilter minFilter, TextureFilter magFilter) override;
        void SetAddressMode(TextureAddressMode u, TextureAddressMode v) override;
        void DrawImGui(IntVec2 const& size, FloatVec2 const& uv0, FloatVec2 const& uv1) const override;
        void SaveToPNG(std::filesystem::path const& path, IntRect const& sourceRect) override;

    protected:
        uint32_t m_id;
        SurfaceContext& m_context;
        VkExtent2D m_vkExtent;
        VkFormat m_vkFormat;

        // Image+VMA allocation are paired and freed together in the destructor;
        // they stay raw because vmaFreeMemory + vkDestroyImage operate as a unit
        // gated by m_owningImage.
        VkImage m_vkImage = VK_NULL_HANDLE;
        VmaAllocation m_vmaAllocation = VK_NULL_HANDLE;
        mutable UniqueHandle<VkImageView> m_vkView;
        // One persistent sampler per filter mode, lazily created. Never destroyed
        // during a frame — only in the destructor — so command buffers in flight are
        // never invalidated by a mid-frame SetFilter call.
        mutable UniqueHandle<VkSampler> m_vkSamplerLinear;
        mutable UniqueHandle<VkSampler> m_vkSamplerNearest;
        // ImGui-managed descriptor set and the sampler it was created with.
        mutable UniqueHandle<VkDescriptorSet> m_vkDescriptorSet;
        mutable VkSampler m_vkDescriptorSetSampler = VK_NULL_HANDLE;
        VkFilter m_minFilter = VK_FILTER_LINEAR;
        VkFilter m_magFilter = VK_FILTER_LINEAR;
        VkSamplerAddressMode m_addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerAddressMode m_addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        bool m_owningImage = true;

        void CreateResource(VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
        void CreateView() const;
        VkSampler CreateSampler(VkFilter min, VkFilter mag) const;
    };
}
