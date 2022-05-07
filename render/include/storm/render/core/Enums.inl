// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#define AS_AND_FROM(M, X, Y)                                                              \
    inline X fromVK(Y value) { return M.at(value); }                                      \
    inline Y toVK(X value) {                                                              \
        auto it = std::find_if(std::cbegin(M), std::cend(M), [&value](const auto &pair) { \
            return pair.second == value;                                                  \
        });                                                                               \
                                                                                          \
        return it->first;                                                                 \
    }

#define AS_AND_FROM_PROXY(M, X, Y)                                                        \
    inline X fromVK(Y value) { return M.at(value.get()); }                                \
    inline Y toVK(X value) {                                                              \
        auto it = std::find_if(std::cbegin(M), std::cend(M), [&value](const auto &pair) { \
            return pair.second == value;                                                  \
        });                                                                               \
                                                                                          \
        return core::makeNamed<Y>(it->first);                                             \
    }

#define AS_AND_FROM_FLAGS(M, X, Y)                       \
    inline X fromVK(Y vk_flag) {                         \
        auto flag = X { 0 };                             \
                                                         \
        for (const auto &[key, value] : M) {             \
            if ((vk_flag & key) == key) flag |= value;   \
        }                                                \
                                                         \
        return flag;                                     \
    }                                                    \
    inline Y toVK(X flag) {                              \
        auto vk_flag = Y { 0 };                          \
                                                         \
        for (const auto &[key, value] : M) {             \
            if ((flag & value) == value) vk_flag |= key; \
        }                                                \
                                                         \
        return vk_flag;                                  \
    }

#define AS_FLAGS_BITS(M, X, Y)                                             \
    inline Y toVKBits(X flag) {                                            \
        auto vk_flag = 0;                                                  \
                                                                           \
        for (const auto &[key, value] : M) {                               \
            if ((flag & value) == value) vk_flag |= static_cast<int>(key); \
        }                                                                  \
                                                                           \
        return static_cast<Y>(vk_flag);                                    \
    }

namespace storm::render {
    static const auto physical_device_map =
        storm::core::HashMap<vk::PhysicalDeviceType, PhysicalDeviceType> {
            { vk::PhysicalDeviceType::eCpu, PhysicalDeviceType::CPU },
            { vk::PhysicalDeviceType::eDiscreteGpu, PhysicalDeviceType::Discrete_GPU },
            { vk::PhysicalDeviceType::eVirtualGpu, PhysicalDeviceType::Virtual_GPU },
            { vk::PhysicalDeviceType::eIntegratedGpu, PhysicalDeviceType::Integrated_GPU }
        };

    AS_AND_FROM(physical_device_map, PhysicalDeviceType, vk::PhysicalDeviceType)

    std::string to_string(PhysicalDeviceType type) {
        switch (type) {
            case PhysicalDeviceType::CPU: return "CPU";
            case PhysicalDeviceType::Virtual_GPU: return "Virtual_GPU";
            case PhysicalDeviceType::Discrete_GPU: return "Discrete_GPU";
            case PhysicalDeviceType::Integrated_GPU: return "core::Integrated_GPU";
        }

        return "Unknown";
    }

    static const auto queue_flag_map = storm::core::HashMap<vk::QueueFlagBits, QueueFlag> {
        { vk::QueueFlagBits::eGraphics, QueueFlag::Graphics },
        { vk::QueueFlagBits::eCompute, QueueFlag::Compute },
        { vk::QueueFlagBits::eTransfer, QueueFlag::Transfert },
        { vk::QueueFlagBits::eSparseBinding, QueueFlag::Sparse_Binding },
    };

    AS_AND_FROM_FLAGS(queue_flag_map, QueueFlag, vk::QueueFlags)

    static const auto shader_stage_map =
        storm::core::HashMap<vk::ShaderStageFlagBits, ShaderStage> {
            { vk::ShaderStageFlagBits::eVertex, ShaderStage::Vertex },
            { vk::ShaderStageFlagBits::eFragment, ShaderStage::Fragment },
            { vk::ShaderStageFlagBits::eGeometry, ShaderStage::Geometry },
            { vk::ShaderStageFlagBits::eCompute, ShaderStage::Compute }
        };

    AS_AND_FROM_FLAGS(shader_stage_map, ShaderStage, vk::ShaderStageFlags)
    AS_FLAGS_BITS(shader_stage_map, ShaderStage, vk::ShaderStageFlagBits)

    static const auto primitive_topology =
        storm::core::HashMap<vk::PrimitiveTopology, PrimitiveTopology> {
            { vk::PrimitiveTopology::ePointList, PrimitiveTopology::Point_List },
            { vk::PrimitiveTopology::eLineList, PrimitiveTopology::Line_List },
            { vk::PrimitiveTopology::eLineStrip, PrimitiveTopology::Line_Strip },
            { vk::PrimitiveTopology::eTriangleList, PrimitiveTopology::Triangle_List },
            { vk::PrimitiveTopology::eTriangleStrip, PrimitiveTopology::Triangle_Strip },
            { vk::PrimitiveTopology::eTriangleFan, PrimitiveTopology::Triangle_Fan }
        };

    AS_AND_FROM(primitive_topology, PrimitiveTopology, vk::PrimitiveTopology)

    static const auto polygon_mode = storm::core::HashMap<vk::PolygonMode, PolygonMode> {
        { vk::PolygonMode::eFill, PolygonMode::Fill },
        { vk::PolygonMode::eLine, PolygonMode::Line },
        { vk::PolygonMode::ePoint, PolygonMode::Point }
    };

    AS_AND_FROM(polygon_mode, PolygonMode, vk::PolygonMode)

    static const auto cull_mode_map = storm::core::HashMap<vk::CullModeFlagBits, CullMode> {
        { vk::CullModeFlagBits::eFront, CullMode::Front },
        { vk::CullModeFlagBits::eBack, CullMode::Back }
    };

    AS_AND_FROM_FLAGS(cull_mode_map, CullMode, vk::CullModeFlags)

    static const auto front_face_map = storm::core::HashMap<vk::FrontFace, FrontFace> {
        { vk::FrontFace::eClockwise, FrontFace::Clockwise },
        { vk::FrontFace::eCounterClockwise, FrontFace::Counter_Clockwise }
    };

    AS_AND_FROM(front_face_map, FrontFace, vk::FrontFace)

    static const auto sample_count_map =
        storm::core::HashMap<vk::SampleCountFlagBits, SampleCountFlag> {
            { vk::SampleCountFlagBits::e1, SampleCountFlag::C1_BIT },
            { vk::SampleCountFlagBits::e2, SampleCountFlag::C2_BIT },
            { vk::SampleCountFlagBits::e4, SampleCountFlag::C4_BIT },
            { vk::SampleCountFlagBits::e8, SampleCountFlag::C8_BIT },
            { vk::SampleCountFlagBits::e16, SampleCountFlag::C16_BIT },
            { vk::SampleCountFlagBits::e32, SampleCountFlag::C32_BIT },
            { vk::SampleCountFlagBits::e64, SampleCountFlag::C64_BIT }
        };

    AS_AND_FROM_FLAGS(sample_count_map, SampleCountFlag, vk::SampleCountFlags)
    AS_FLAGS_BITS(sample_count_map, SampleCountFlag, vk::SampleCountFlagBits)

    static const auto color_component_map =
        storm::core::HashMap<vk::ColorComponentFlagBits, ColorComponent> {
            { vk::ColorComponentFlagBits::eR, ColorComponent::R },
            { vk::ColorComponentFlagBits::eG, ColorComponent::G },
            { vk::ColorComponentFlagBits::eB, ColorComponent::B },
            { vk::ColorComponentFlagBits::eA, ColorComponent::A },
        };

    AS_AND_FROM_FLAGS(color_component_map, ColorComponent, vk::ColorComponentFlags)

    static const auto blend_factor_map = storm::core::HashMap<vk::BlendFactor, BlendFactor> {
        { vk::BlendFactor::eOne, BlendFactor::One },
        { vk::BlendFactor::eZero, BlendFactor::Zero },
        { vk::BlendFactor::eSrcColor, BlendFactor::Src_Color },
        { vk::BlendFactor::eOneMinusSrcColor, BlendFactor::One_Minus_Src_Color },
        { vk::BlendFactor::eDstColor, BlendFactor::Dst_Color },
        { vk::BlendFactor::eOneMinusDstColor, BlendFactor::One_Minus_Dst_Color },
        { vk::BlendFactor::eSrcAlpha, BlendFactor::Src_Alpha },
        { vk::BlendFactor::eOneMinusSrcAlpha, BlendFactor::One_Minus_Src_Alpha },
        { vk::BlendFactor::eConstantColor, BlendFactor::Constant_Color },
        { vk::BlendFactor::eOneMinusConstantColor, BlendFactor::One_Minus_Constant_Color },
        { vk::BlendFactor::eConstantAlpha, BlendFactor::Constant_Alpha },
        { vk::BlendFactor::eOneMinusConstantAlpha, BlendFactor::One_Minus_Constant_Alpha },
        { vk::BlendFactor::eSrcAlphaSaturate, BlendFactor::Src_Alpha_Saturate },
        { vk::BlendFactor::eSrc1Color, BlendFactor::Src1_Color },
        { vk::BlendFactor::eOneMinusSrc1Color, BlendFactor::One_Minus_Src1_Color },
        { vk::BlendFactor::eSrc1Alpha, BlendFactor::Src1_Alpha },
        { vk::BlendFactor::eOneMinusSrc1Alpha, BlendFactor::One_Minus_Src1_Alpha },
    };

    AS_AND_FROM(blend_factor_map, BlendFactor, vk::BlendFactor)

    static const auto blend_operation_map = storm::core::HashMap<vk::BlendOp, BlendOperation> {
        { vk::BlendOp::eAdd, BlendOperation::Add },
        { vk::BlendOp::eSubtract, BlendOperation::Substract },
        { vk::BlendOp::eReverseSubtract, BlendOperation::Reverse_Substract },
        { vk::BlendOp::eMin, BlendOperation::Min },
        { vk::BlendOp::eMax, BlendOperation::Max },
    };

    AS_AND_FROM(blend_operation_map, BlendOperation, vk::BlendOp)

    static const auto logic_operation_map = storm::core::HashMap<vk::LogicOp, LogicOperation> {
        { vk::LogicOp::eClear, LogicOperation::Clear },
        { vk::LogicOp::eAnd, LogicOperation::And },
        { vk::LogicOp::eAndReverse, LogicOperation::And_Reverse },
        { vk::LogicOp::eCopy, LogicOperation::Copy },
        { vk::LogicOp::eAndInverted, LogicOperation::And_Inverted },
        { vk::LogicOp::eNoOp, LogicOperation::No_Operation },
        { vk::LogicOp::eXor, LogicOperation::Xor },
        { vk::LogicOp::eOr, LogicOperation::Or },
        { vk::LogicOp::eNor, LogicOperation::Nor },
        { vk::LogicOp::eEquivalent, LogicOperation::Equivalent },
        { vk::LogicOp::eInvert, LogicOperation::Invert },
        { vk::LogicOp::eOrReverse, LogicOperation::Or_Reverse },
        { vk::LogicOp::eCopyInverted, LogicOperation::Copy_Inverted },
        { vk::LogicOp::eOrInverted, LogicOperation::Or_Inverted },
        { vk::LogicOp::eNand, LogicOperation::Nand },
        { vk::LogicOp::eSet, LogicOperation::Set },
    };

    AS_AND_FROM(logic_operation_map, LogicOperation, vk::LogicOp)

    static const auto pixel_format_map = storm::core::HashMap<vk::Format, PixelFormat> {
        { vk::Format::eR8Snorm, PixelFormat::R8_SNorm },
        { vk::Format::eR8G8Snorm, PixelFormat::RG8_SNorm },
        { vk::Format::eR8G8B8Snorm, PixelFormat::RGB8_SNorm },
        { vk::Format::eR8G8B8A8Snorm, PixelFormat::RGBA8_SNorm },

        { vk::Format::eR8Unorm, PixelFormat::R8_UNorm },
        { vk::Format::eR8G8Unorm, PixelFormat::RG8_UNorm },
        { vk::Format::eR8G8B8Unorm, PixelFormat::RGB8_UNorm },
        { vk::Format::eR8G8B8A8Unorm, PixelFormat::RGBA8_UNorm },

        { vk::Format::eR16Snorm, PixelFormat::R16_SNorm },
        { vk::Format::eR16G16Snorm, PixelFormat::RG16_SNorm },
        { vk::Format::eR16G16B16Snorm, PixelFormat::RGB16_SNorm },
        { vk::Format::eR16G16B16A16Snorm, PixelFormat::RGBA16_SNorm },

        { vk::Format::eR16Unorm, PixelFormat::R16_UNorm },
        { vk::Format::eR16G16Unorm, PixelFormat::RG16_UNorm },
        { vk::Format::eR16G16B16Unorm, PixelFormat::RGB16_UNorm },
        { vk::Format::eR16G16B16A16Unorm, PixelFormat::RGBA16_UNorm },

        { vk::Format::eA2B10G10R10UnormPack32, PixelFormat::RGB10_A2_UNorm },
        { vk::Format::eR4G4B4A4UnormPack16, PixelFormat::RGBA4_UNorm },
        { vk::Format::eR5G5B5A1UnormPack16, PixelFormat::RGB5_A1_UNorm },
        { vk::Format::eR5G6B5UnormPack16, PixelFormat::RGB_5_6_5_UNorm },

        { vk::Format::eB8G8R8Unorm, PixelFormat::BGR8_UNorm },
        { vk::Format::eB8G8R8A8Unorm, PixelFormat::BGRA8_UNorm },

        { vk::Format::eR8Sint, PixelFormat::R8I },
        { vk::Format::eR8G8Sint, PixelFormat::RG8I },
        { vk::Format::eR8G8B8Sint, PixelFormat::RGB8I },
        { vk::Format::eR8G8B8A8Sint, PixelFormat::RGBA8I },

        { vk::Format::eR8Uint, PixelFormat::R8U },
        { vk::Format::eR8G8Uint, PixelFormat::RG8U },
        { vk::Format::eR8G8B8Uint, PixelFormat::RGB8U },
        { vk::Format::eR8G8B8A8Uint, PixelFormat::RGBA8U },

        { vk::Format::eR16Sint, PixelFormat::R16I },
        { vk::Format::eR16G16Sint, PixelFormat::RG16I },
        { vk::Format::eR16G16B16Sint, PixelFormat::RGB16I },
        { vk::Format::eR16G16B16A16Sint, PixelFormat::RGBA16I },

        { vk::Format::eR16Uint, PixelFormat::R16U },
        { vk::Format::eR16G16Uint, PixelFormat::RG16U },
        { vk::Format::eR16G16B16Uint, PixelFormat::RGB16U },
        { vk::Format::eR16G16B16A16Uint, PixelFormat::RGBA16U },

        { vk::Format::eR32Sint, PixelFormat::R32I },
        { vk::Format::eR32G32Sint, PixelFormat::RG32I },
        { vk::Format::eR32G32B32Sint, PixelFormat::RGB32I },
        { vk::Format::eR32G32B32A32Sint, PixelFormat::RGBA32I },

        { vk::Format::eR32Uint, PixelFormat::R32U },
        { vk::Format::eR32G32Uint, PixelFormat::RG32U },
        { vk::Format::eR32G32B32Uint, PixelFormat::RGB32U },
        { vk::Format::eR32G32B32A32Uint, PixelFormat::RGBA32U },

        { vk::Format::eA2B10G10R10UintPack32, PixelFormat::RGB10_A2U },

        { vk::Format::eR16Sfloat, PixelFormat::R16F },
        { vk::Format::eR16G16Sfloat, PixelFormat::RG16F },
        { vk::Format::eR16G16B16Sfloat, PixelFormat::RGB16F },
        { vk::Format::eR16G16B16A16Sfloat, PixelFormat::RGBA16F },

        { vk::Format::eR32Sfloat, PixelFormat::R32F },
        { vk::Format::eR32G32Sfloat, PixelFormat::RG32F },
        { vk::Format::eR32G32B32Sfloat, PixelFormat::RGB32F },
        { vk::Format::eR32G32B32A32Sfloat, PixelFormat::RGBA32F },

        { vk::Format::eB10G11R11UfloatPack32, PixelFormat::BGR_11_11_10F },

        { vk::Format::eR8G8B8Srgb, PixelFormat::sRGB8 },
        { vk::Format::eR8G8B8A8Srgb, PixelFormat::sRGBA8 },
        { vk::Format::eB8G8R8Srgb, PixelFormat::sBGR8 },
        { vk::Format::eB8G8R8A8Srgb, PixelFormat::sBGRA8 },

        { vk::Format::eD16Unorm, PixelFormat::Depth16 },
        { vk::Format::eX8D24UnormPack32, PixelFormat::Depth24 },
        { vk::Format::eD32Sfloat, PixelFormat::Depth32F },

        { vk::Format::eD16UnormS8Uint, PixelFormat::Depth16_Stencil8 },
        { vk::Format::eD24UnormS8Uint, PixelFormat::Depth24_Stencil8 },
        { vk::Format::eD32SfloatS8Uint, PixelFormat::Depth32F_Stencil8 },

        { vk::Format::eUndefined, PixelFormat::Undefined },
    };

    AS_AND_FROM(pixel_format_map, PixelFormat, vk::Format)

    static const auto attachment_load_operation_map =
        storm::core::HashMap<vk::AttachmentLoadOp, AttachmentLoadOperation> {
            { vk::AttachmentLoadOp::eLoad, AttachmentLoadOperation::Load },
            { vk::AttachmentLoadOp::eClear, AttachmentLoadOperation::Clear },
            { vk::AttachmentLoadOp::eDontCare, AttachmentLoadOperation::Dont_Care },
        };

    AS_AND_FROM(attachment_load_operation_map, AttachmentLoadOperation, vk::AttachmentLoadOp)

    static const auto attachment_store_operation_map =
        storm::core::HashMap<vk::AttachmentStoreOp, AttachmentStoreOperation> {
            { vk::AttachmentStoreOp::eStore, AttachmentStoreOperation::Store },
            { vk::AttachmentStoreOp::eDontCare, AttachmentStoreOperation::Dont_Care },
        };

    AS_AND_FROM(attachment_store_operation_map, AttachmentStoreOperation, vk::AttachmentStoreOp)

    static const auto texture_layout_operation_map =
        storm::core::HashMap<vk::ImageLayout, TextureLayout> {
            { vk::ImageLayout::eUndefined, TextureLayout::Undefined },
            { vk::ImageLayout::eGeneral, TextureLayout::General },
            { vk::ImageLayout::eColorAttachmentOptimal, TextureLayout::Color_Attachment_Optimal },
            { vk::ImageLayout::eDepthStencilAttachmentOptimal,
              TextureLayout::Depth_Stencil_Attachment_Optimal },
            { vk::ImageLayout::eDepthStencilReadOnlyOptimal,
              TextureLayout::Depth_Stencil_Read_Only_Optimal },
            { vk::ImageLayout::eShaderReadOnlyOptimal, TextureLayout::Shader_Read_Only_Optimal },
            { vk::ImageLayout::eTransferSrcOptimal, TextureLayout::Transfer_Src_Optimal },
            { vk::ImageLayout::eTransferDstOptimal, TextureLayout::Transfer_Dst_Optimal },
            { vk::ImageLayout::ePreinitialized, TextureLayout::Preinitialized },
            { vk::ImageLayout::eDepthReadOnlyStencilAttachmentOptimal,
              TextureLayout::Depth_Read_Only_Stencil_Attachment_Optimal },
            { vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal,
              TextureLayout::Depth_Attachment_Stencil_Read_Only_Optimal },
            { vk::ImageLayout::ePresentSrcKHR, TextureLayout::Present_Src },
            { vk::ImageLayout::eSharedPresentKHR, TextureLayout::Shared_Present },
        };

    AS_AND_FROM(texture_layout_operation_map, TextureLayout, vk::ImageLayout)

    static const auto pipeline_bind_point_map =
        storm::core::HashMap<vk::PipelineBindPoint, PipelineBindPoint> {
            { vk::PipelineBindPoint::eGraphics, PipelineBindPoint::Graphics },
            { vk::PipelineBindPoint::eCompute, PipelineBindPoint::Compute },
        };

    AS_AND_FROM(pipeline_bind_point_map, PipelineBindPoint, vk::PipelineBindPoint)

    static const auto vertex_input_rate_map =
        storm::core::HashMap<vk::VertexInputRate, VertexInputRate> {
            { vk::VertexInputRate::eVertex, VertexInputRate::Vertex },
            { vk::VertexInputRate::eInstance, VertexInputRate::Instance },
        };

    AS_AND_FROM(vertex_input_rate_map, VertexInputRate, vk::VertexInputRate)

    static const auto format_map = storm::core::HashMap<vk::Format, Format> {
        { vk::Format::eR8Sint, Format::Byte },
        { vk::Format::eR8G8Sint, Format::Byte2 },
        { vk::Format::eR8G8B8Sint, Format::Byte3 },
        { vk::Format::eR8G8B8A8Sint, Format::Byte4 },

        { vk::Format::eR8Uint, Format::UByte },
        { vk::Format::eR8G8Uint, Format::UByte2 },
        { vk::Format::eR8G8B8Uint, Format::UByte3 },
        { vk::Format::eR8G8B8A8Uint, Format::UByte4 },

        { vk::Format::eR8Snorm, Format::Byte_Norm },
        { vk::Format::eR8G8Snorm, Format::Byte2_Norm },
        { vk::Format::eR8G8B8Snorm, Format::Byte3_Norm },
        { vk::Format::eR8G8B8A8Snorm, Format::Byte3_Norm },

        { vk::Format::eR8Unorm, Format::UByte_Norm },
        { vk::Format::eR8G8Unorm, Format::UByte2_Norm },
        { vk::Format::eR8G8B8Unorm, Format::UByte3_Norm },
        { vk::Format::eR8G8B8A8Unorm, Format::UByte4_Norm },

        { vk::Format::eR8Sscaled, Format::Byte_Scaled },
        { vk::Format::eR8G8Sscaled, Format::Byte2_Scaled },
        { vk::Format::eR8G8B8Sscaled, Format::Byte3_Scaled },
        { vk::Format::eR8G8B8A8Sscaled, Format::Byte3_Scaled },

        { vk::Format::eR8Uscaled, Format::UByte_Scaled },
        { vk::Format::eR8G8Uscaled, Format::UByte2_Scaled },
        { vk::Format::eR8G8B8Uscaled, Format::UByte3_Scaled },
        { vk::Format::eR8G8B8A8Uscaled, Format::UByte4_Scaled },

        { vk::Format::eR16Sint, Format::Short },
        { vk::Format::eR16G16Sint, Format::Short2 },
        { vk::Format::eR16G16B16Sint, Format::Short3 },
        { vk::Format::eR16G16B16A16Sint, Format::Short4 },

        { vk::Format::eR16Uint, Format::UShort },
        { vk::Format::eR16G16Uint, Format::UShort2 },
        { vk::Format::eR16G16B16Uint, Format::UShort3 },
        { vk::Format::eR16G16B16A16Uint, Format::UShort4 },

        { vk::Format::eR16Snorm, Format::Short_Norm },
        { vk::Format::eR16G16Snorm, Format::Short2_Norm },
        { vk::Format::eR16G16B16Snorm, Format::Short3_Norm },
        { vk::Format::eR16G16B16A16Snorm, Format::Short3_Norm },

        { vk::Format::eR16Unorm, Format::UShort_Norm },
        { vk::Format::eR16G16Unorm, Format::UShort2_Norm },
        { vk::Format::eR16G16B16Unorm, Format::UShort3_Norm },
        { vk::Format::eR16G16B16A16Unorm, Format::UShort4_Norm },

        { vk::Format::eR16Sscaled, Format::Short_Scaled },
        { vk::Format::eR16G16Sscaled, Format::Short2_Scaled },
        { vk::Format::eR16G16B16Sscaled, Format::Short3_Scaled },
        { vk::Format::eR16G16B16A16Sscaled, Format::Short3_Scaled },

        { vk::Format::eR16Uscaled, Format::UShort_Scaled },
        { vk::Format::eR16G16Uscaled, Format::UShort2_Scaled },
        { vk::Format::eR16G16B16Uscaled, Format::UShort3_Scaled },
        { vk::Format::eR16G16B16A16Uscaled, Format::UShort4_Scaled },

        { vk::Format::eR32Sint, Format::Int },
        { vk::Format::eR32G32Sint, Format::Int2 },
        { vk::Format::eR32G32B32Sint, Format::Int3 },
        { vk::Format::eR32G32B32A32Sint, Format::Int4 },

        { vk::Format::eR32Uint, Format::UInt },
        { vk::Format::eR32G32Uint, Format::UInt2 },
        { vk::Format::eR32G32B32Uint, Format::UInt3 },
        { vk::Format::eR32G32B32A32Uint, Format::UInt4 },

        { vk::Format::eR64Sint, Format::Long },
        { vk::Format::eR64G64Sint, Format::Long2 },
        { vk::Format::eR64G64B64Sint, Format::Long3 },
        { vk::Format::eR64G64B64A64Sint, Format::Long4 },

        { vk::Format::eR64Uint, Format::ULong },
        { vk::Format::eR64G64Uint, Format::ULong2 },
        { vk::Format::eR64G64B64Uint, Format::ULong3 },
        { vk::Format::eR64G64B64A64Uint, Format::ULong4 },

        { vk::Format::eR32Sfloat, Format::Float },
        { vk::Format::eR32G32Sfloat, Format::Float2 },
        { vk::Format::eR32G32B32Sfloat, Format::Float3 },
        { vk::Format::eR32G32B32A32Sfloat, Format::Float4 },

        { vk::Format::eR64Sfloat, Format::Double },
        { vk::Format::eR64G64Sfloat, Format::Double2 },
        { vk::Format::eR64G64B64Sfloat, Format::Double3 },
        { vk::Format::eR64G64B64A64Sfloat, Format::Double4 },

        { vk::Format::eUndefined, Format::Undefined },
    };

    using FormatProxy = core::NamedType<vk::Format, struct VkFormatTag>;
    AS_AND_FROM_PROXY(format_map, Format, FormatProxy)

    static const auto buffer_usage_map =
        storm::core::HashMap<vk::BufferUsageFlagBits, HardwareBufferUsage> {
            { vk::BufferUsageFlagBits::eVertexBuffer, HardwareBufferUsage::Vertex },
            { vk::BufferUsageFlagBits::eIndexBuffer, HardwareBufferUsage::Index },
            { vk::BufferUsageFlagBits::eTransferSrc, HardwareBufferUsage::Transfert_Src },
            { vk::BufferUsageFlagBits::eTransferDst, HardwareBufferUsage::Transfert_Dst },
            { vk::BufferUsageFlagBits::eUniformBuffer, HardwareBufferUsage::Uniform },
            { vk::BufferUsageFlagBits::eStorageBuffer, HardwareBufferUsage::Storage },
            { vk::BufferUsageFlagBits::eUniformTexelBuffer, HardwareBufferUsage::Uniform_Texel },
            { vk::BufferUsageFlagBits::eStorageTexelBuffer, HardwareBufferUsage::Storage_Texel },
            { vk::BufferUsageFlagBits::eIndirectBuffer, HardwareBufferUsage::Indirect },
        };

    AS_AND_FROM_FLAGS(buffer_usage_map, HardwareBufferUsage, vk::BufferUsageFlags)

    static const auto texture_usage_map =
        storm::core::HashMap<vk::ImageUsageFlagBits, TextureUsage> {
            { vk::ImageUsageFlagBits::eTransferSrc, TextureUsage::Transfert_Src },
            { vk::ImageUsageFlagBits::eTransferDst, TextureUsage::Transfert_Dst },
            { vk::ImageUsageFlagBits::eSampled, TextureUsage::Sampled },
            { vk::ImageUsageFlagBits::eStorage, TextureUsage::Storage },
            { vk::ImageUsageFlagBits::eColorAttachment, TextureUsage::Color_Attachment },
            { vk::ImageUsageFlagBits::eDepthStencilAttachment,
              TextureUsage::Depth_Stencil_Attachment },
            { vk::ImageUsageFlagBits::eTransientAttachment, TextureUsage::Transient_Attachment },
            { vk::ImageUsageFlagBits::eInputAttachment, TextureUsage::Input_Attachment },
        };

    AS_AND_FROM_FLAGS(texture_usage_map, TextureUsage, vk::ImageUsageFlags)

    static const auto memory_property_map =
        storm::core::HashMap<vk::MemoryPropertyFlagBits, MemoryProperty> {
            { vk::MemoryPropertyFlagBits::eDeviceLocal, MemoryProperty::Device_Local },
            { vk::MemoryPropertyFlagBits::eHostVisible, MemoryProperty::Host_Visible },
            { vk::MemoryPropertyFlagBits::eHostCoherent, MemoryProperty::Host_Coherent },
            { vk::MemoryPropertyFlagBits::eHostCached, MemoryProperty::Host_Cached },
        };

    AS_AND_FROM_FLAGS(memory_property_map, MemoryProperty, vk::MemoryPropertyFlags)

    static const auto texture_aspect_mask_map =
        storm::core::HashMap<vk::ImageAspectFlagBits, TextureAspectMask> {
            { vk::ImageAspectFlagBits::eColor, TextureAspectMask::Color },
            { vk::ImageAspectFlagBits::eDepth, TextureAspectMask::Depth },
            { vk::ImageAspectFlagBits::eStencil, TextureAspectMask::Stencil },
        };

    AS_AND_FROM_FLAGS(texture_aspect_mask_map, TextureAspectMask, vk::ImageAspectFlags)

    static const auto texture_flag_map =
        storm::core::HashMap<vk::ImageCreateFlagBits, TextureCreateFlag> {
            { vk::ImageCreateFlagBits {}, TextureCreateFlag::None },
            { vk::ImageCreateFlagBits::eSparseBinding, TextureCreateFlag::Sparse_Binding },
            { vk::ImageCreateFlagBits::eSparseResidency, TextureCreateFlag::Sparse_Residency },
            { vk::ImageCreateFlagBits::eSparseAliased, TextureCreateFlag::Sparse_Aliased },
            { vk::ImageCreateFlagBits::eMutableFormat, TextureCreateFlag::Mutable_Format },
            { vk::ImageCreateFlagBits::eCubeCompatible, TextureCreateFlag::Cube_Compatible },
            { vk::ImageCreateFlagBits::eAlias, TextureCreateFlag::Alias },
            { vk::ImageCreateFlagBits::eSplitInstanceBindRegions,
              TextureCreateFlag::Split_Instance_Bind_Regions },
            { vk::ImageCreateFlagBits::e2DArrayCompatible, TextureCreateFlag::Array_2D_Compatible },
            { vk::ImageCreateFlagBits::eBlockTexelViewCompatible,
              TextureCreateFlag::Block_Texel_View_Compatible },
            { vk::ImageCreateFlagBits::eExtendedUsage, TextureCreateFlag::Extended_Usage },
            { vk::ImageCreateFlagBits::eProtected, TextureCreateFlag::Protected },
            { vk::ImageCreateFlagBits::eDisjoint, TextureCreateFlag::Disjoint }
        };
    AS_AND_FROM_FLAGS(texture_flag_map, TextureCreateFlag, vk::ImageCreateFlags)

    static const auto descriptor_type_map =
        storm::core::HashMap<vk::DescriptorType, DescriptorType> {
            { vk::DescriptorType::eSampler, DescriptorType::Sampler },
            { vk::DescriptorType::eCombinedImageSampler, DescriptorType::Combined_Texture_Sampler },
            { vk::DescriptorType::eSampledImage, DescriptorType::Sampled_Image },
            { vk::DescriptorType::eStorageImage, DescriptorType::Storage_Image },
            { vk::DescriptorType::eUniformTexelBuffer, DescriptorType::Uniform_Texel_Buffer },
            { vk::DescriptorType::eStorageTexelBuffer, DescriptorType::Storage_Texel_Buffer },
            { vk::DescriptorType::eUniformBuffer, DescriptorType::Uniform_Buffer },
            { vk::DescriptorType::eStorageBuffer, DescriptorType::Storage_Buffer },
            { vk::DescriptorType::eUniformBufferDynamic, DescriptorType::Uniform_Buffer_Dynamic },
            { vk::DescriptorType::eStorageBufferDynamic, DescriptorType::Storage_Buffer_Dynamic },
            { vk::DescriptorType::eInputAttachment, DescriptorType::Input_Attachment },
        };

    AS_AND_FROM(descriptor_type_map, DescriptorType, vk::DescriptorType)

    static const auto command_buffer_level_map =
        storm::core::HashMap<vk::CommandBufferLevel, CommandBufferLevel> {
            { vk::CommandBufferLevel::ePrimary, CommandBufferLevel::Primary },
            { vk::CommandBufferLevel::eSecondary, CommandBufferLevel::Secondary },
        };

    AS_AND_FROM(command_buffer_level_map, CommandBufferLevel, vk::CommandBufferLevel)

    static const auto command_operation_map =
        storm::core::HashMap<vk::CompareOp, CompareOperation> {
            { vk::CompareOp::eNever, CompareOperation::Never },
            { vk::CompareOp::eLess, CompareOperation::Less },
            { vk::CompareOp::eEqual, CompareOperation::Equal },
            { vk::CompareOp::eLessOrEqual, CompareOperation::Less_Or_Equal },
            { vk::CompareOp::eGreater, CompareOperation::Greater },
            { vk::CompareOp::eNotEqual, CompareOperation::Not_Equal },
            { vk::CompareOp::eGreaterOrEqual, CompareOperation::Greater_Or_Equal },
            { vk::CompareOp::eAlways, CompareOperation::Always },
        };

    AS_AND_FROM(command_operation_map, CompareOperation, vk::CompareOp)

    static const auto filter_map = storm::core::HashMap<vk::Filter, Filter> {
        { vk::Filter::eLinear, Filter::Linear },
        { vk::Filter::eNearest, Filter::Nearest },
    };

    AS_AND_FROM(filter_map, Filter, vk::Filter)

    static const auto sampler_address_mode_map =
        storm::core::HashMap<vk::SamplerAddressMode, SamplerAddressMode> {
            { vk::SamplerAddressMode::eRepeat, SamplerAddressMode::Repeat },
            { vk::SamplerAddressMode::eMirroredRepeat, SamplerAddressMode::Mirrored_Repeat },
            { vk::SamplerAddressMode::eClampToEdge, SamplerAddressMode::Clamp_To_Edge },
            { vk::SamplerAddressMode::eClampToBorder, SamplerAddressMode::Clamp_To_Border },
            { vk::SamplerAddressMode::eMirrorClampToEdge,
              SamplerAddressMode::Mirror_Clamp_To_Edge },
        };

    AS_AND_FROM(sampler_address_mode_map, SamplerAddressMode, vk::SamplerAddressMode)

    static const auto border_color_map = storm::core::HashMap<vk::BorderColor, BorderColor> {
        { vk::BorderColor::eFloatTransparentBlack, BorderColor::Float_Transparent_Black },
        { vk::BorderColor::eIntTransparentBlack, BorderColor::Int_Transparent_Black },
        { vk::BorderColor::eFloatOpaqueBlack, BorderColor::Float_Opaque_Black },
        { vk::BorderColor::eIntOpaqueBlack, BorderColor::Int_Opaque_Black },
        { vk::BorderColor::eFloatOpaqueWhite, BorderColor::Float_Opaque_White },
        { vk::BorderColor::eIntOpaqueWhite, BorderColor::Int_opaque_White },
    };

    AS_AND_FROM(border_color_map, BorderColor, vk::BorderColor)

    static const auto sampler_mipmap_mode_map =
        storm::core::HashMap<vk::SamplerMipmapMode, SamplerMipmapMode> {
            { vk::SamplerMipmapMode::eLinear, SamplerMipmapMode::Linear },
            { vk::SamplerMipmapMode::eNearest, SamplerMipmapMode::Nearest },
        };

    AS_AND_FROM(sampler_mipmap_mode_map, SamplerMipmapMode, vk::SamplerMipmapMode)

    static const auto texture_type_map = storm::core::HashMap<vk::ImageType, TextureType> {
        { vk::ImageType::e1D, TextureType::T1D },
        { vk::ImageType::e2D, TextureType::T2D },
        { vk::ImageType::e3D, TextureType::T3D },
    };

    AS_AND_FROM(texture_type_map, TextureType, vk::ImageType)

    static const auto texture_view_type_map =
        storm::core::HashMap<vk::ImageViewType, TextureViewType> {
            { vk::ImageViewType::e1D, TextureViewType::T1D },
            { vk::ImageViewType::e2D, TextureViewType::T2D },
            { vk::ImageViewType::e3D, TextureViewType::T3D },
            { vk::ImageViewType::eCube, TextureViewType::Cube },
            { vk::ImageViewType::e1DArray, TextureViewType::T1D_Array },
            { vk::ImageViewType::e2DArray, TextureViewType::T2D_Array },
            { vk::ImageViewType::eCubeArray, TextureViewType::Cube_Array },
        };

    AS_AND_FROM(texture_view_type_map, TextureViewType, vk::ImageViewType)

    static const auto debug_report_object_type_map =
        storm::core::HashMap<vk::ObjectType, DebugObjectType> {
            { vk::ObjectType::eUnknown, DebugObjectType::Unknown },
            { vk::ObjectType::eInstance, DebugObjectType::Instance },
            { vk::ObjectType::ePhysicalDevice, DebugObjectType::Physical_Device },
            { vk::ObjectType::eDevice, DebugObjectType::Device },
            { vk::ObjectType::eQueue, DebugObjectType::Queue },
            { vk::ObjectType::eSemaphore, DebugObjectType::Semaphore },
            { vk::ObjectType::eCommandBuffer, DebugObjectType::Command_Buffer },
            { vk::ObjectType::eFence, DebugObjectType::Fence },
            { vk::ObjectType::eDeviceMemory, DebugObjectType::Device_Memory },
            { vk::ObjectType::eBuffer, DebugObjectType::Buffer },
            { vk::ObjectType::eImage, DebugObjectType::Image },
            { vk::ObjectType::eEvent, DebugObjectType::Event },
            { vk::ObjectType::eQueryPool, DebugObjectType::Query_Pool },
            { vk::ObjectType::eBufferView, DebugObjectType::Buffer_View },
            { vk::ObjectType::eImageView, DebugObjectType::Image_View },
            { vk::ObjectType::eShaderModule, DebugObjectType::Shader_Module },
            { vk::ObjectType::ePipelineCache, DebugObjectType::Pipeline_Cache },
            { vk::ObjectType::ePipelineLayout, DebugObjectType::Pipeline_Layout },
            { vk::ObjectType::eRenderPass, DebugObjectType::Render_Pass },
            { vk::ObjectType::ePipeline, DebugObjectType::Pipeline },
            { vk::ObjectType::eDescriptorSetLayout, DebugObjectType::Descriptor_Set_Layout },
            { vk::ObjectType::eSampler, DebugObjectType::Sampler },
            { vk::ObjectType::eDescriptorPool, DebugObjectType::Descriptor_Pool },
            { vk::ObjectType::eDescriptorSet, DebugObjectType::Descriptor_Set },
            { vk::ObjectType::eFramebuffer, DebugObjectType::Framebuffer },
            { vk::ObjectType::eCommandPool, DebugObjectType::Command_Pool },
            { vk::ObjectType::eSurfaceKHR, DebugObjectType::Surface },
            { vk::ObjectType::eSwapchainKHR, DebugObjectType::Swapchain },
            { vk::ObjectType::eDebugReportCallbackEXT, DebugObjectType::Debug_Report_Callback },
            { vk::ObjectType::eDisplayKHR, DebugObjectType::Display_KHR },
        };

    AS_AND_FROM(debug_report_object_type_map, DebugObjectType, vk::ObjectType)

    static const auto access_flag_map = storm::core::HashMap<vk::AccessFlagBits, AccessFlag> {
        { vk::AccessFlagBits {}, AccessFlag::None },
        { vk::AccessFlagBits::eIndirectCommandRead, AccessFlag::Indirect_Command_Read },
        { vk::AccessFlagBits::eVertexAttributeRead, AccessFlag::Vertex_Attribute_Read },
        { vk::AccessFlagBits::eUniformRead, AccessFlag::Uniform_Read },
        { vk::AccessFlagBits::eInputAttachmentRead, AccessFlag::Input_Attachment_Read },
        { vk::AccessFlagBits::eShaderRead, AccessFlag::Shader_Read },
        { vk::AccessFlagBits::eShaderWrite, AccessFlag::Shader_Write },
        { vk::AccessFlagBits::eColorAttachmentRead, AccessFlag::Color_Attachment_Read },
        { vk::AccessFlagBits::eColorAttachmentWrite, AccessFlag::Color_Attachment_Write },
        { vk::AccessFlagBits::eDepthStencilAttachmentRead,
          AccessFlag::Depth_Stencil_Attachment_Read },
        { vk::AccessFlagBits::eDepthStencilAttachmentWrite,
          AccessFlag::Depth_Stencil_Attachment_Write },
        { vk::AccessFlagBits::eTransferRead, AccessFlag::Transfer_Read },
        { vk::AccessFlagBits::eTransferWrite, AccessFlag::Transfer_Write },
        { vk::AccessFlagBits::eHostRead, AccessFlag::Host_Read },
        { vk::AccessFlagBits::eHostWrite, AccessFlag::Host_Write },
        { vk::AccessFlagBits::eMemoryRead, AccessFlag::Memory_Read },
        { vk::AccessFlagBits::eMemoryWrite, AccessFlag::Memory_Write },
    };

    AS_AND_FROM_FLAGS(access_flag_map, AccessFlag, vk::AccessFlags)

    static const auto pipeline_stage_flag_map =
        storm::core::HashMap<vk::PipelineStageFlagBits, PipelineStageFlag> {
            { vk::PipelineStageFlagBits {}, PipelineStageFlag::None },
            { vk::PipelineStageFlagBits::eTopOfPipe, PipelineStageFlag::Top_Of_Pipe },
            { vk::PipelineStageFlagBits::eDrawIndirect, PipelineStageFlag::Draw_Indirect },
            { vk::PipelineStageFlagBits::eVertexInput, PipelineStageFlag::Vertex_Input },
            { vk::PipelineStageFlagBits::eVertexShader, PipelineStageFlag::Vertex_Shader },
            { vk::PipelineStageFlagBits::eFragmentShader, PipelineStageFlag::Fragment_Shader },
            { vk::PipelineStageFlagBits::eTessellationControlShader,
              PipelineStageFlag::Tessellation_Control_Shader },
            { vk::PipelineStageFlagBits::eTessellationEvaluationShader,
              PipelineStageFlag::Tessellation_Evaluation_Shader },
            { vk::PipelineStageFlagBits::eGeometryShader, PipelineStageFlag::Geometry_Shader },
            { vk::PipelineStageFlagBits::eEarlyFragmentTests,
              PipelineStageFlag::Early_Fragment_Tests },
            { vk::PipelineStageFlagBits::eLateFragmentTests,
              PipelineStageFlag::Late_Fragment_Tests },
            { vk::PipelineStageFlagBits::eColorAttachmentOutput,
              PipelineStageFlag::Color_Attachment_Output },
            { vk::PipelineStageFlagBits::eComputeShader, PipelineStageFlag::Compute_Shader },
            { vk::PipelineStageFlagBits::eTransfer, PipelineStageFlag::Transfer },
            { vk::PipelineStageFlagBits::eBottomOfPipe, PipelineStageFlag::Bottom_Of_Pipe },
            { vk::PipelineStageFlagBits::eHost, PipelineStageFlag::Host },
            { vk::PipelineStageFlagBits::eAllGraphics, PipelineStageFlag::All_Graphics },
            { vk::PipelineStageFlagBits::eAllCommands, PipelineStageFlag::All_Commands },
        };

    AS_AND_FROM_FLAGS(pipeline_stage_flag_map, PipelineStageFlag, vk::PipelineStageFlags)

    static const auto dependency_flag_map =
        storm::core::HashMap<vk::DependencyFlagBits, DependencyFlag> {
            { vk::DependencyFlagBits {}, DependencyFlag::None },
            { vk::DependencyFlagBits::eByRegion, DependencyFlag::By_Region },
            { vk::DependencyFlagBits::eDeviceGroup, DependencyFlag::Device_Group },
            { vk::DependencyFlagBits::eViewLocal, DependencyFlag::View_Local },
        };

    AS_AND_FROM_FLAGS(dependency_flag_map, DependencyFlag, vk::DependencyFlags)

    static const auto dynamic_state_map = storm::core::HashMap<vk::DynamicState, DynamicState> {
        { vk::DynamicState::eViewport, DynamicState::Viewport },
        { vk::DynamicState::eScissor, DynamicState::Scissor },
        { vk::DynamicState::eLineWidth, DynamicState::Line_Width },
        { vk::DynamicState::eDepthBias, DynamicState::Depth_Bias },
        { vk::DynamicState::eBlendConstants, DynamicState::Blend_Constants },
        { vk::DynamicState::eDepthBounds, DynamicState::Depth_Bounds },
        { vk::DynamicState::eStencilCompareMask, DynamicState::Stencil_Compare_Mask },
        { vk::DynamicState::eStencilReference, DynamicState::Stencil_Reference },
    };

    AS_AND_FROM(dynamic_state_map, DynamicState, vk::DynamicState)

    static const auto texture_tiling_map = storm::core::HashMap<vk::ImageTiling, TextureTiling> {
        { vk::ImageTiling::eOptimal, TextureTiling::Optimal },
        { vk::ImageTiling::eLinear, TextureTiling::Linear },
        { vk::ImageTiling::eDrmFormatModifierEXT, TextureTiling::DRM_Ext },
    };

    AS_AND_FROM(texture_tiling_map, TextureTiling, vk::ImageTiling)
} // namespace storm::render

#undef AS_AND_FROM
#undef AS_AND_FROM_PROXY
#undef AS_AND_FROM_FLAGS
