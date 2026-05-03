#include "common.h"
#include "moth_graphics/graphics/vulkan/vulkan_texture.h"
#include "moth_graphics/graphics/vulkan/vulkan_command_buffer.h"
#include "moth_graphics/graphics/vulkan/vulkan_utils.h"
#include "stb_image.h"
#include "stb_image_write.h"

namespace {
    uint32_t NextTextureId = 1; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
}

namespace moth_graphics::graphics::vulkan {
    std::unique_ptr<Texture> Texture::FromFile(SurfaceContext& context, std::filesystem::path const& path) {
        if (!std::filesystem::exists(path)) {
            return nullptr;
        }
        int texWidth = 0;
        int texHeight = 0;
        int texChannels = 0;
        stbi_uc* pixels = stbi_load(path.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        if (pixels == nullptr) {
            return nullptr;
        }
        auto texture = FromRGBA(context, texWidth, texHeight, pixels);
        stbi_image_free(pixels);
        return texture;
    }

    std::unique_ptr<Texture> Texture::FromRGBA(SurfaceContext& context, int width, int height, unsigned char const* pixels) {
        VkDeviceSize imageSize = width * height * 4;
        auto stagingBuffer = std::make_unique<Buffer>(context, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        void* data = stagingBuffer->Map();
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        stagingBuffer->Unmap();

        VkFormat const format = VK_FORMAT_R8G8B8A8_UNORM;
        auto newImage = std::make_unique<Texture>(context, width, height, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

        auto commandBuffer = std::make_unique<CommandBuffer>(context);
        commandBuffer->BeginRecord();
        commandBuffer->TransitionImageLayout(*newImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        commandBuffer->CopyBufferToImage(*newImage, *stagingBuffer);
        commandBuffer->TransitionImageLayout(*newImage, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        commandBuffer->SubmitAndWait();

        return newImage;
    }

    Texture::Texture(SurfaceContext& context)
        : id(NextTextureId++)
        , m_context(context)
        , m_vkExtent{}
        , m_vkFormat(VK_FORMAT_UNDEFINED) {
        Texture::SetFilter(TextureFilter::Linear, TextureFilter::Linear);
    }

    Texture::Texture(SurfaceContext& context, VkImage image, VkImageView view, VkExtent2D extent, VkFormat format, bool owning)
        : id(NextTextureId++)
        , m_context(context)
        , m_vkExtent(extent)
        , m_vkFormat(format)
        , m_vkImage(image)
        , m_vkView(view)
        , m_owningImage(owning) {
        Texture::SetFilter(TextureFilter::Linear, TextureFilter::Linear);
    }

    Texture::Texture(SurfaceContext& context, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, bool owning)
        : id(NextTextureId++)
        , m_context(context)
        , m_vkExtent{ width, height }
        , m_vkFormat(format)
        , m_owningImage(owning) {
        CreateResource(tiling, usage, properties);
        // CreateView();
        // CreateDefaultSampler();
        Texture::SetFilter(TextureFilter::Linear, TextureFilter::Linear);
    }

    Texture::~Texture() {
        if (m_vkDescriptorSet != VK_NULL_HANDLE) {
            ImGui_ImplVulkan_RemoveTexture(m_vkDescriptorSet);
        }
        if (m_vkSamplerLinear != VK_NULL_HANDLE) {
            vkDestroySampler(m_context.GetVkDevice(), m_vkSamplerLinear, nullptr);
        }
        if (m_vkSamplerNearest != VK_NULL_HANDLE) {
            vkDestroySampler(m_context.GetVkDevice(), m_vkSamplerNearest, nullptr);
        }
        if (m_vkView != VK_NULL_HANDLE) {
            vkDestroyImageView(m_context.GetVkDevice(), m_vkView, nullptr);
        }
        if (m_owningImage && m_vkImage != VK_NULL_HANDLE) {
            vmaFreeMemory(m_context.GetVmaAllocator(), m_vmaAllocation);
            vkDestroyImage(m_context.GetVkDevice(), m_vkImage, nullptr);
        }
    }

    VkImageView Texture::GetVkView() const {
        if (m_vkView == VK_NULL_HANDLE) {
            CreateView();
        }
        return m_vkView;
    }
    VkSampler Texture::GetVkSampler() const {
        bool const nearest = (m_minFilter == VK_FILTER_NEAREST || m_magFilter == VK_FILTER_NEAREST);
        VkSampler& slot = nearest ? m_vkSamplerNearest : m_vkSamplerLinear;
        if (slot == VK_NULL_HANDLE) {
            slot = CreateSampler(m_minFilter, m_magFilter);
        }
        return slot;
    }

    VkDescriptorSet Texture::GetDescriptorSet() const {
        VkSampler const currentSampler = GetVkSampler();
        if (m_vkDescriptorSet != VK_NULL_HANDLE && m_vkDescriptorSetSampler == currentSampler) {
            return m_vkDescriptorSet;
        }
        if (m_vkDescriptorSet != VK_NULL_HANDLE) {
            ImGui_ImplVulkan_RemoveTexture(m_vkDescriptorSet);
        }
        m_vkDescriptorSet = ImGui_ImplVulkan_AddTexture(currentSampler, GetVkView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        m_vkDescriptorSetSampler = currentSampler;
        return m_vkDescriptorSet;
    }

    void* Texture::Map() {
        void* data = nullptr;
        vmaMapMemory(m_context.GetVmaAllocator(), m_vmaAllocation, &data);
        return data;
    }

    void Texture::Unmap() {
        vmaUnmapMemory(m_context.GetVmaAllocator(), m_vmaAllocation);
    }

    void Texture::CreateResource(VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
        VkImageCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.imageType = VK_IMAGE_TYPE_2D;
        info.extent = { m_vkExtent.width, m_vkExtent.height, 1 };
        info.mipLevels = 1;
        info.arrayLayers = 1;
        info.format = m_vkFormat;
        info.tiling = tiling;
        info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        info.usage = usage;
        info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.samples = VK_SAMPLE_COUNT_1_BIT;
        info.flags = 0;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocInfo.requiredFlags = properties;
        if ((properties & (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) != 0u) {
            allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        }
        CHECK_VK_RESULT(vmaCreateImage(m_context.GetVmaAllocator(), &info, &allocInfo, &m_vkImage, &m_vmaAllocation, nullptr));
    }

    void Texture::CreateView() const {
        VkImageViewCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        info.format = m_vkFormat;
        info.subresourceRange = {};
        info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        info.subresourceRange.baseMipLevel = 0;
        info.subresourceRange.levelCount = 1;
        info.subresourceRange.baseArrayLayer = 0;
        info.subresourceRange.layerCount = 1;
        info.image = m_vkImage;
        CHECK_VK_RESULT(vkCreateImageView(m_context.GetVkDevice(), &info, nullptr, &m_vkView));
    }

    namespace {
        VkSamplerAddressMode ToVkAddressMode(TextureAddressMode mode) {
            switch (mode) {
            case TextureAddressMode::Repeat:
                return VK_SAMPLER_ADDRESS_MODE_REPEAT;
            case TextureAddressMode::MirroredRepeat:
                return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
            case TextureAddressMode::ClampToEdge:
                return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            case TextureAddressMode::ClampToBorder:
                return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            }
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        }
    }

    void Texture::SetFilter(TextureFilter minFilter, TextureFilter magFilter) {
        // Just record the desired filter mode. Samplers are created lazily by
        // GetVkSampler() and kept alive for the texture's entire lifetime, so
        // switching filter mid-frame is safe — no sampler or descriptor set is
        // destroyed while a command buffer may still reference it.
        m_minFilter = (minFilter == TextureFilter::Nearest) ? VK_FILTER_NEAREST : VK_FILTER_LINEAR;
        m_magFilter = (magFilter == TextureFilter::Nearest) ? VK_FILTER_NEAREST : VK_FILTER_LINEAR;
    }

    void Texture::DrawImGui(IntVec2 const& size, FloatVec2 const& uv0, FloatVec2 const& uv1) const {
        ImGui::Image(GetDescriptorSet(),
                     ImVec2(static_cast<float>(size.x), static_cast<float>(size.y)),
                     ImVec2(uv0.x, uv0.y),
                     ImVec2(uv1.x, uv1.y));
    }

    void Texture::SaveToPNG(std::filesystem::path const& path, IntRect const& sourceRect) {
        auto const targetWidth = static_cast<uint32_t>(sourceRect.w());
        auto const targetHeight = static_cast<uint32_t>(sourceRect.h());
        auto const targetFormat = VK_FORMAT_R8G8B8A8_UNORM;
        auto stagingImage = std::make_unique<Texture>(
            m_context, targetWidth, targetHeight,
            targetFormat, VK_IMAGE_TILING_LINEAR,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        VkImageLayout const srcLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        auto commandBuffer = std::make_unique<CommandBuffer>(m_context);
        commandBuffer->BeginRecord();
        commandBuffer->TransitionImageLayout(*this, m_vkFormat, srcLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        commandBuffer->TransitionImageLayout(*stagingImage, targetFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        VkImageCopy region{};
        region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.srcSubresource.layerCount = 1;
        region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.dstSubresource.layerCount = 1;
        region.srcOffset = { static_cast<int32_t>(sourceRect.topLeft.x), static_cast<int32_t>(sourceRect.topLeft.y), 0 };
        region.extent.width = targetWidth;
        region.extent.height = targetHeight;
        region.extent.depth = 1;
        vkCmdCopyImage(commandBuffer->GetVkCommandBuffer(), m_vkImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       stagingImage->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        commandBuffer->TransitionImageLayout(*stagingImage, targetFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
        commandBuffer->TransitionImageLayout(*this, m_vkFormat, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, srcLayout);
        commandBuffer->SubmitAndWait();

        VkImageSubresource subresource{};
        subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        VkSubresourceLayout subLayout{};
        vkGetImageSubresourceLayout(m_context.GetVkDevice(), stagingImage->GetVkImage(), &subresource, &subLayout);
        auto const rowPitch = static_cast<uint32_t>(subLayout.rowPitch);

        uint8_t const* data = static_cast<uint8_t const*>(stagingImage->Map());
        std::vector<uint8_t> dataCopy(targetWidth * targetHeight * 4);
        for (uint32_t row = 0; row < targetHeight; ++row) {
            uint8_t const* src = data + (row * rowPitch);
            uint8_t* dst = dataCopy.data() + (row * targetWidth * 4);
            for (uint32_t col = 0; col < targetWidth; ++col) {
                dst[(col * 4) + 0] = src[(col * 4) + 2];
                dst[(col * 4) + 1] = src[(col * 4) + 1];
                dst[(col * 4) + 2] = src[(col * 4) + 0];
                dst[(col * 4) + 3] = src[(col * 4) + 3];
            }
        }
        stbi_write_png(path.string().c_str(), static_cast<int>(targetWidth), static_cast<int>(targetHeight), 4, dataCopy.data(), static_cast<int>(targetWidth) * 4);
        stagingImage->Unmap();
    }

    void Texture::SetAddressMode(TextureAddressMode u, TextureAddressMode v) {
        VkSamplerAddressMode const newU = ToVkAddressMode(u);
        VkSamplerAddressMode const newV = ToVkAddressMode(v);
        if (newU == m_addressModeU && newV == m_addressModeV) {
            return;
        }
        m_addressModeU = newU;
        m_addressModeV = newV;
        // Address mode affects sampler state. Invalidate cached samplers so they
        // are recreated with the new mode on next use. Safe to destroy here because
        // SetAddressMode is not called during frame recording.
        if (m_vkSamplerLinear != VK_NULL_HANDLE) {
            vkDeviceWaitIdle(m_context.GetVkDevice());
            vkDestroySampler(m_context.GetVkDevice(), m_vkSamplerLinear, nullptr);
            m_vkSamplerLinear = VK_NULL_HANDLE;
        }
        if (m_vkSamplerNearest != VK_NULL_HANDLE) {
            vkDestroySampler(m_context.GetVkDevice(), m_vkSamplerNearest, nullptr);
            m_vkSamplerNearest = VK_NULL_HANDLE;
        }
        if (m_vkDescriptorSet != VK_NULL_HANDLE) {
            ImGui_ImplVulkan_RemoveTexture(m_vkDescriptorSet);
            m_vkDescriptorSet = VK_NULL_HANDLE;
            m_vkDescriptorSetSampler = VK_NULL_HANDLE;
        }
    }

    VkSampler Texture::CreateSampler(VkFilter min, VkFilter mag) const {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = mag;
        samplerInfo.minFilter = min;
        samplerInfo.addressModeU = m_addressModeU;
        samplerInfo.addressModeV = m_addressModeV;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        bool const useAnisotropy = (min != VK_FILTER_NEAREST && mag != VK_FILTER_NEAREST);
        samplerInfo.anisotropyEnable = useAnisotropy ? VK_TRUE : VK_FALSE;

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(m_context.GetVkPhysicalDevice(), &properties);

        samplerInfo.maxAnisotropy = useAnisotropy ? properties.limits.maxSamplerAnisotropy : 1.0f;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        VkSampler sampler = VK_NULL_HANDLE;
        CHECK_VK_RESULT(vkCreateSampler(m_context.GetVkDevice(), &samplerInfo, nullptr, &sampler));
        return sampler;
    }
}
