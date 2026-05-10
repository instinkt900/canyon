#pragma once

#include "vulkan_unique.h"

#include <vulkan/vulkan_core.h>

#include <memory>
#include <map>
#include <vector>
#include <cstdint>
#include <string>
#include <stddef.h>

namespace moth_graphics::graphics::vulkan {
    class Texture;

    struct Shader {
        Shader(uint32_t hash);
        ~Shader();

        struct Stage {
            VkShaderStageFlagBits m_stageFlags;
            UniqueHandle<VkShaderModule> m_module;
            std::string m_entryPoint;
        };

        uint32_t m_hash;
        VkDevice m_device;
        VkDescriptorPool m_descriptorPool;
        std::vector<Shader::Stage> m_stages;
        UniqueHandle<VkDescriptorSetLayout> m_descriptorSetLayout;
        UniqueHandle<VkPipelineLayout> m_pipelineLayout;

        VkDescriptorSet GetDescriptorSet(Texture& image);
        VkDescriptorSet CreateDescriptorSet(Texture& image);

        // Keyed by (image_id, sampler) so that the same texture with two different
        // filter modes can coexist without any descriptor set being freed mid-frame.
        // Sets are batch-freed in the destructor before the pool/layouts go away.
        std::map<std::pair<uint32_t, VkSampler>, VkDescriptorSet> m_descriptorSets;
    };

    class ShaderBuilder {
    public:
        ShaderBuilder(VkDevice device, VkDescriptorPool descriptorPool);
        ShaderBuilder& AddPushConstant(VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size);
        ShaderBuilder& AddBinding(uint32_t binding, VkDescriptorType type, uint32_t count, VkShaderStageFlags flags);
        ShaderBuilder& AddStage(VkShaderStageFlagBits stageFlags, std::string const& entryPoint, unsigned char const* byteCode, size_t codeSize);
        std::unique_ptr<Shader> Build();

    private:
        struct Stage {
            VkShaderStageFlagBits m_stageFlags;
            std::string m_entryPoint;
            std::vector<char> m_byteCode;
        };

        VkDevice m_device;
        VkDescriptorPool m_descriptorPool;
        std::vector<Stage> m_stages;
        std::vector<VkPushConstantRange> m_pushConstants;
        std::vector<VkDescriptorSetLayoutBinding> m_layoutBindings;

        uint32_t CalculateHash() const;
    };
}
