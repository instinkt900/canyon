#include "common.h"
#include "canyon/graphics/vulkan/vulkan_texture.h"
#include "canyon/graphics/vulkan/vulkan_command_buffer.h"
#include "canyon/graphics/vulkan/vulkan_utils.h"
#include "canyon/graphics/stb_image.h"

namespace {
    static uint32_t NextTextureId = 1;
}

namespace canyon::graphics::vulkan {
    std::unique_ptr<Texture> Texture::FromFile(SurfaceContext& context, std::filesystem::path const& path) {
        if (std::filesystem::exists(path)) {
            int texWidth;
            int texHeight;
            int texChannels;
            stbi_uc* pixels = stbi_load(path.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
            VkDeviceSize imageSize = texWidth * texHeight * 4;
            if (pixels != nullptr) {
                auto stagingBuffer = std::make_unique<Buffer>(context, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

                void* data = stagingBuffer->Map();
                memcpy(data, pixels, static_cast<size_t>(imageSize));
                stagingBuffer->Unmap();

                stbi_image_free(pixels);

                VkFormat const format = VK_FORMAT_R8G8B8A8_UNORM;
                auto newImage = std::make_unique<Texture>(context, texWidth, texHeight, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

                auto commandBuffer = std::make_unique<CommandBuffer>(context);
                commandBuffer->BeginRecord();
                commandBuffer->TransitionImageLayout(*newImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
                commandBuffer->CopyBufferToImage(*newImage, *stagingBuffer);
                commandBuffer->TransitionImageLayout(*newImage, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                commandBuffer->SubmitAndWait();

                return newImage;
            }
        }
        return nullptr;
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
        : m_id(NextTextureId++)
        , m_context(context) {
    }

    Texture::Texture(SurfaceContext& context, VkImage image, VkImageView view, VkExtent2D extent, VkFormat format, bool owning)
        : m_id(NextTextureId++)
        , m_context(context)
        , m_vkExtent(extent)
        , m_vkFormat(format)
        , m_vkImage(image)
        , m_vkView(view)
        , m_owningImage(owning) {
    }

    Texture::Texture(SurfaceContext& context, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, bool owning)
        : m_id(NextTextureId++)
        , m_context(context)
        , m_vkExtent{ width, height }
        , m_vkFormat(format)
        , m_owningImage(owning) {
        CreateResource(tiling, usage, properties);
        //CreateView();
        //CreateDefaultSampler();
    }

    Texture::~Texture() {
        if (m_vkSampler != VK_NULL_HANDLE) {
            vkDestroySampler(m_context.GetVkDevice(), m_vkSampler, nullptr);
        }
        if (m_vkView != VK_NULL_HANDLE) {
            vkDestroyImageView(m_context.GetVkDevice(), m_vkView, nullptr);
        }
        if (m_owningImage && m_vkImage != VK_NULL_HANDLE) {
            vmaFreeMemory(m_context.GetVmaAllocator(), m_vmaAllocation);
            vkDestroyImage(m_context.GetVkDevice(), m_vkImage, nullptr);
        }
    }

    VkImageView Texture::GetVkView() {
        if (m_vkView == VK_NULL_HANDLE) {
            CreateView();
        }
        return m_vkView;
    }
    VkSampler Texture::GetVkSampler() {
        if (m_vkSampler == VK_NULL_HANDLE) {
            CreateDefaultSampler();
        }
        return m_vkSampler;
    }

    VkDescriptorSet Texture::GetDescriptorSet() {
        if (m_vkDescriptorSet == VK_NULL_HANDLE) {
            m_vkDescriptorSet = ImGui_ImplVulkan_AddTexture(m_vkSampler, m_vkView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
        return m_vkDescriptorSet;
    }

    void* Texture::Map() {
        void* data;
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
        if (properties & (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        }
        CHECK_VK_RESULT(vmaCreateImage(m_context.GetVmaAllocator(), &info, &allocInfo, &m_vkImage, &m_vmaAllocation, nullptr));
    }

    void Texture::CreateView() {
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

    void Texture::CreateDefaultSampler() {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;

        // todo can probably cache this
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(m_context.GetVkPhysicalDevice(), &properties);

        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        CHECK_VK_RESULT(vkCreateSampler(m_context.GetVkDevice(), &samplerInfo, nullptr, &m_vkSampler));
    }
}
