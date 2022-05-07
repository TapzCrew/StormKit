// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <cstdint>

#include <storm/core/Flags.hpp>
#include <storm/core/HashMap.hpp>
#include <storm/core/Strings.hpp>

#include <storm/render/core/Vulkan.hpp>

namespace storm::render {
    enum class PhysicalDeviceType : core::UInt8 {
        Discrete_GPU   = 0,
        Virtual_GPU    = 1,
        Integrated_GPU = 2,
        CPU            = 3
    };

    inline std::string to_string(PhysicalDeviceType type);

    enum class QueueFlag : core::UInt8 {
        Graphics       = 1 << 0,
        Compute        = 1 << 1,
        Transfert      = 1 << 2,
        Sparse_Binding = 1 << 3
    };

    enum class ShaderStage : core::UInt8 {
        Vertex   = 1 << 0,
        Fragment = 1 << 1,
        Geometry = 1 << 2,
        Compute  = 1 << 3
    };

    enum class PrimitiveTopology : core::UInt8 {
        Point_List     = 0,
        Line_List      = 1,
        Line_Strip     = 2,
        Triangle_List  = 3,
        Triangle_Strip = 4,
        Triangle_Fan   = 5
    };

    enum class PolygonMode : core::UInt8 { Fill = 0, Line = 1, Point = 2 };

    enum class CullMode : core::UInt8 {
        None       = 0,
        Front      = 1 << 0,
        Back       = 1 << 1,
        Front_Back = Front | Back
    };

    enum class FrontFace : core::UInt8 { Clockwise = 0, Counter_Clockwise = 1 };

    enum class SampleCountFlag : core::UInt8 {
        C1_BIT  = 1 << 0,
        C2_BIT  = 1 << 1,
        C4_BIT  = 1 << 2,
        C8_BIT  = 1 << 3,
        C16_BIT = 1 << 4,
        C32_BIT = 1 << 5,
        C64_BIT = 1 << 6
    };

    enum class ColorComponent : core::UInt8 {
        R    = 1 << 0,
        G    = 1 << 1,
        B    = 1 << 2,
        A    = 1 << 3,
        RG   = R | G,
        RGB  = RG | B,
        RGBA = RGB | A
    };

    enum class BlendFactor : core::UInt8 {
        One                      = 0,
        Zero                     = 1,
        Src_Color                = 2,
        One_Minus_Src_Color      = 3,
        Dst_Color                = 4,
        One_Minus_Dst_Color      = 5,
        Src_Alpha                = 6,
        One_Minus_Src_Alpha      = 7,
        Dst_Alpha                = 8,
        One_Minus_Dst_Alpha      = 9,
        Constant_Color           = 10,
        One_Minus_Constant_Color = 11,
        Constant_Alpha           = 12,
        One_Minus_Constant_Alpha = 13,
        Src_Alpha_Saturate       = 14,
        Src1_Color               = 15,
        One_Minus_Src1_Color     = 16,
        Src1_Alpha               = 17,
        One_Minus_Src1_Alpha     = 18
    };

    enum class BlendOperation : core::UInt8 {
        Add               = 0,
        Substract         = 1,
        Reverse_Substract = 2,
        Min               = 3,
        Max               = 4
    };

    enum class LogicOperation : core::UInt8 {
        Clear         = 0,
        And           = 1,
        And_Reverse   = 2,
        Copy          = 3,
        And_Inverted  = 4,
        No_Operation  = 5,
        Xor           = 6,
        Or            = 7,
        Nor           = 8,
        Equivalent    = 9,
        Invert        = 10,
        Or_Reverse    = 11,
        Copy_Inverted = 12,
        Or_Inverted   = 13,
        Nand          = 14,
        Set           = 15
    };

    enum class PixelFormat : core::UInt8 {
        R8_SNorm          = 0,
        RG8_SNorm         = 1,
        RGB8_SNorm        = 2,
        RGBA8_SNorm       = 3,
        R8_UNorm          = 4,
        RG8_UNorm         = 5,
        RGB8_UNorm        = 6,
        RGBA8_UNorm       = 7,
        R16_SNorm         = 8,
        RG16_SNorm        = 9,
        RGB16_SNorm       = 10,
        RGBA16_SNorm      = 11,
        R16_UNorm         = 12,
        RG16_UNorm        = 13,
        RGB16_UNorm       = 14,
        RGBA16_UNorm      = 15,
        RGB10_A2_UNorm    = 16,
        RGBA4_UNorm       = 17,
        RGB5_A1_UNorm     = 18,
        RGB_5_6_5_UNorm   = 19,
        BGR8_UNorm        = 20,
        BGRA8_UNorm       = 21,
        R8I               = 22,
        RG8I              = 23,
        RGB8I             = 24,
        RGBA8I            = 25,
        R8U               = 26,
        RG8U              = 27,
        RGB8U             = 28,
        RGBA8U            = 29,
        R16I              = 30,
        RG16I             = 31,
        RGB16I            = 32,
        RGBA16I           = 33,
        R16U              = 34,
        RG16U             = 35,
        RGB16U            = 36,
        RGBA16U           = 37,
        R32I              = 38,
        RG32I             = 39,
        RGB32I            = 40,
        RGBA32I           = 41,
        R32U              = 42,
        RG32U             = 43,
        RGB32U            = 44,
        RGBA32U           = 45,
        RGB10_A2U         = 46,
        R16F              = 47,
        RG16F             = 48,
        RGB16F            = 49,
        RGBA16F           = 50,
        R32F              = 51,
        RG32F             = 52,
        RGB32F            = 53,
        RGBA32F           = 54,
        BGR_11_11_10F     = 55,
        sRGB8             = 56,
        sRGBA8            = 57,
        sBGR8             = 58,
        sBGRA8            = 59,
        Depth16           = 60,
        Depth24           = 61,
        Depth32F          = 62,
        Depth16_Stencil8  = 63,
        Depth24_Stencil8  = 64,
        Depth32F_Stencil8 = 65,
        Undefined         = 254
    };

    enum class AttachmentLoadOperation : core::UInt8 { Load = 0, Clear = 1, Dont_Care = 2 };

    enum class AttachmentStoreOperation : core::UInt8 { Store = 0, Dont_Care = 1 };

    enum class PipelineBindPoint : core::UInt8 { Graphics = 0, Compute = 1 };

    enum class TextureLayout : core::UInt8 {
        General                                    = 0,
        Color_Attachment_Optimal                   = 1,
        Depth_Stencil_Attachment_Optimal           = 2,
        Depth_Stencil_Read_Only_Optimal            = 3,
        Shader_Read_Only_Optimal                   = 4,
        Transfer_Src_Optimal                       = 5,
        Transfer_Dst_Optimal                       = 6,
        Preinitialized                             = 7,
        Depth_Read_Only_Stencil_Attachment_Optimal = 8,
        Depth_Attachment_Stencil_Read_Only_Optimal = 9,
        Present_Src                                = 10,
        Shared_Present                             = 11,
        Undefined                                  = 12
    };

    enum class TextureAspectMask : core::UInt8 { Color = 1 << 0, Depth = 1 << 1, Stencil = 1 << 2 };

    enum class VertexInputRate : core::UInt8 { Vertex = 0, Instance = 1 };

    enum class TextureCreateFlag : core::UInt16 {
        None                        = 0,
        Sparse_Binding              = 1 << 0,
        Sparse_Residency            = 1 << 1,
        Sparse_Aliased              = 1 << 2,
        Mutable_Format              = 1 << 3,
        Cube_Compatible             = 1 << 4,
        Alias                       = 1 << 5,
        Split_Instance_Bind_Regions = 1 << 6,
        Array_2D_Compatible         = 1 << 7,
        Block_Texel_View_Compatible = 1 << 8,
        Extended_Usage              = 1 << 9,
        Protected                   = 1 << 10,
        Disjoint                    = 1 << 11
    };

    enum class Format : core::UInt8 {
        Byte           = 0,
        Byte2          = 1,
        Byte3          = 2,
        Byte4          = 3,
        Byte_Norm      = 4,
        Byte2_Norm     = 5,
        Byte3_Norm     = 6,
        Byte4_Norm     = 7,
        Byte_Scaled    = 8,
        Byte2_Scaled   = 9,
        Byte3_Scaled   = 10,
        Byte4_Scaled   = 11,
        UByte          = 12,
        UByte2         = 13,
        UByte3         = 14,
        UByte4         = 15,
        UByte_Norm     = 16,
        UByte2_Norm    = 17,
        UByte3_Norm    = 18,
        UByte4_Norm    = 19,
        UByte_Scaled   = 20,
        UByte2_Scaled  = 21,
        UByte3_Scaled  = 22,
        UByte4_Scaled  = 23,
        Short          = 24,
        Short2         = 25,
        Short3         = 26,
        Short4         = 27,
        Short_Norm     = 28,
        Short2_Norm    = 29,
        Short3_Norm    = 30,
        Short4_Norm    = 31,
        Short_Scaled   = 32,
        Short2_Scaled  = 33,
        Short3_Scaled  = 34,
        Short4_Scaled  = 35,
        UShort         = 36,
        UShort2        = 37,
        UShort3        = 38,
        UShort4        = 39,
        UShort_Norm    = 40,
        UShort2_Norm   = 41,
        UShort3_Norm   = 42,
        UShort4_Norm   = 43,
        UShort_Scaled  = 44,
        UShort2_Scaled = 45,
        UShort3_Scaled = 46,
        UShort4_Scaled = 47,
        Int            = 48,
        Int2           = 49,
        Int3           = 50,
        Int4           = 51,
        UInt           = 52,
        UInt2          = 53,
        UInt3          = 54,
        UInt4          = 55,
        Long           = 56,
        Long2          = 57,
        Long3          = 58,
        Long4          = 59,
        ULong          = 60,
        ULong2         = 61,
        ULong3         = 62,
        ULong4         = 63,
        Float          = 64,
        Float2         = 65,
        Float3         = 66,
        Float4         = 67,
        Double         = 68,
        Double2        = 69,
        Double3        = 70,
        Double4        = 71,
        Undefined      = 254
    };

    enum class HardwareBufferUsage : core::UInt16 {
        Vertex        = 1 << 0,
        Index         = 1 << 1,
        Transfert_Src = 1 << 2,
        Transfert_Dst = 1 << 3,
        Uniform       = 1 << 4,
        Storage       = 1 << 5,
        Uniform_Texel = 1 << 6,
        Storage_Texel = 1 << 7,
        Indirect      = 1 << 8
    };

    enum class TextureUsage : core::UInt16 {
        Transfert_Src            = 1 << 0,
        Transfert_Dst            = 1 << 1,
        Sampled                  = 1 << 2,
        Storage                  = 1 << 3,
        Color_Attachment         = 1 << 4,
        Depth_Stencil_Attachment = 1 << 5,
        Transient_Attachment     = 1 << 6,
        Input_Attachment         = 1 << 7
    };

    enum class MemoryProperty : core::UInt8 {
        Device_Local  = 1 << 0,
        Host_Visible  = 1 << 1,
        Host_Coherent = 1 << 2,
        Host_Cached   = 1 << 3
    };

    enum class CommandBufferLevel : core::UInt8 { Primary, Secondary };

    enum class DescriptorType : core::UInt8 {
        Sampler                  = 0,
        Combined_Texture_Sampler = 1,
        Sampled_Image            = 2,
        Storage_Image            = 3,
        Uniform_Texel_Buffer     = 4,
        Storage_Texel_Buffer     = 5,
        Uniform_Buffer           = 6,
        Storage_Buffer           = 7,
        Uniform_Buffer_Dynamic   = 8,
        Storage_Buffer_Dynamic   = 9,
        Input_Attachment         = 10
    };

    enum class CompareOperation : core::UInt8 {
        Never            = 0,
        Less             = 1,
        Equal            = 2,
        Less_Or_Equal    = 3,
        Greater          = 4,
        Not_Equal        = 5,
        Greater_Or_Equal = 6,
        Always           = 7
    };

    enum class Filter : core::UInt8 {
        Nearest = 0,
        Linear  = 1,
    };

    enum class SamplerAddressMode : core::UInt8 {
        Repeat               = 0,
        Mirrored_Repeat      = 1,
        Clamp_To_Edge        = 2,
        Clamp_To_Border      = 3,
        Mirror_Clamp_To_Edge = 4
    };

    enum class BorderColor : core::UInt8 {
        Float_Transparent_Black = 0,
        Int_Transparent_Black   = 1,
        Float_Opaque_Black      = 2,
        Int_Opaque_Black        = 3,
        Float_Opaque_White      = 4,
        Int_opaque_White        = 5
    };

    enum class SamplerMipmapMode : core::UInt8 { Nearest = 0, Linear = 1 };

    enum class WaitResult : core::UInt8 { Success = 0, Timeout = 1, Error = 2 };

    enum class TextureType : core::UInt8 { T1D = 0, T2D = 1, T3D = 2 };

    enum class TextureViewType : core::UInt8 {
        T1D        = 0,
        T2D        = 1,
        T3D        = 2,
        Cube       = 4,
        T1D_Array  = 5,
        T2D_Array  = 6,
        Cube_Array = 7
    };

    enum class DebugObjectType : core::UInt8 {
        Unknown               = 0,
        Instance              = 1,
        Physical_Device       = 2,
        Device                = 3,
        Queue                 = 4,
        Semaphore             = 5,
        Command_Buffer        = 6,
        Fence                 = 7,
        Device_Memory         = 8,
        Buffer                = 9,
        Image                 = 10,
        Event                 = 11,
        Query_Pool            = 12,
        Buffer_View           = 13,
        Image_View            = 14,
        Shader_Module         = 15,
        Pipeline_Cache        = 16,
        Pipeline_Layout       = 17,
        Render_Pass           = 18,
        Pipeline              = 19,
        Descriptor_Set_Layout = 20,
        Sampler               = 21,
        Descriptor_Pool       = 22,
        Descriptor_Set        = 23,
        Framebuffer           = 24,
        Command_Pool          = 25,
        Surface               = 26,
        Swapchain             = 27,
        Debug_Report_Callback = 28,
        Display_KHR           = 29,
    };

    enum class AccessFlag : core::UInt16 {
        None                           = 0,
        Indirect_Command_Read          = 1 << 0,
        Vertex_Attribute_Read          = 1 << 1,
        Uniform_Read                   = 1 << 2,
        Input_Attachment_Read          = 1 << 3,
        Shader_Read                    = 1 << 4,
        Shader_Write                   = 1 << 5,
        Color_Attachment_Read          = 1 << 6,
        Color_Attachment_Write         = 1 << 7,
        Depth_Stencil_Attachment_Read  = 1 << 8,
        Depth_Stencil_Attachment_Write = 1 << 9,
        Transfer_Read                  = 1 << 10,
        Transfer_Write                 = 1 << 11,
        Host_Read                      = 1 << 12,
        Host_Write                     = 1 << 13,
        Memory_Read                    = 1 << 14,
        Memory_Write                   = 1 << 15
    };

    enum class PipelineStageFlag : core::UInt32 {
        None                           = 0,
        Top_Of_Pipe                    = 1 << 0,
        Draw_Indirect                  = 1 << 1,
        Vertex_Input                   = 1 << 2,
        Vertex_Shader                  = 1 << 3,
        Tessellation_Control_Shader    = 1 << 4,
        Tessellation_Evaluation_Shader = 1 << 5,
        Geometry_Shader                = 1 << 6,
        Fragment_Shader                = 1 << 7,
        Early_Fragment_Tests           = 1 << 8,
        Late_Fragment_Tests            = 1 << 9,
        Color_Attachment_Output        = 1 << 10,
        Compute_Shader                 = 1 << 11,
        Transfer                       = 1 << 12,
        Bottom_Of_Pipe                 = 1 << 13,
        Host                           = 1 << 14,
        All_Graphics                   = 1 << 15,
        All_Commands                   = 1 << 16
    };

    enum class DependencyFlag : core::UInt8 {
        None         = 0,
        By_Region    = 1 << 0,
        Device_Group = 1 << 1,
        View_Local   = 1 << 2
    };

    enum class DynamicState : core::UInt8 {
        Viewport             = 0,
        Scissor              = 1,
        Line_Width           = 2,
        Depth_Bias           = 3,
        Blend_Constants      = 4,
        Depth_Bounds         = 5,
        Stencil_Compare_Mask = 6,
        Stencil_Write_Mask   = 7,
        Stencil_Reference    = 8
    };

    constexpr inline bool isDepthOnlyFormat(PixelFormat format) noexcept {
        return format == PixelFormat::Depth16 || format == PixelFormat::Depth24 ||
               format == PixelFormat::Depth32F;
    }

    constexpr inline bool isDepthStencilFormat(PixelFormat format) noexcept {
        return format == PixelFormat::Depth16_Stencil8 || format == PixelFormat::Depth24_Stencil8 ||
               format == PixelFormat::Depth32F_Stencil8;
    }

    constexpr inline bool isDepthFormat(PixelFormat format) noexcept {
        return isDepthOnlyFormat(format) || isDepthStencilFormat(format);
    }

    constexpr inline core::UInt8 getChannelCountFor(PixelFormat format) noexcept {
        switch (format) {
            case PixelFormat::R8_SNorm:
            case PixelFormat::R8_UNorm:
            case PixelFormat::R16_SNorm:
            case PixelFormat::R16_UNorm:
            case PixelFormat::R8I:
            case PixelFormat::R8U:
            case PixelFormat::R16I:
            case PixelFormat::R16U:
            case PixelFormat::R32I:
            case PixelFormat::R32U:
            case PixelFormat::R16F:
            case PixelFormat::R32F:
            case PixelFormat::Depth16:
            case PixelFormat::Depth24:
            case PixelFormat::Depth32F: return 1;

            case PixelFormat::RG8_SNorm:
            case PixelFormat::RG8_UNorm:
            case PixelFormat::RG16_SNorm:
            case PixelFormat::RG16_UNorm:
            case PixelFormat::RG8I:
            case PixelFormat::RG8U:
            case PixelFormat::RG16I:
            case PixelFormat::RG16U:
            case PixelFormat::RG32I:
            case PixelFormat::RG32U:
            case PixelFormat::RG16F:
            case PixelFormat::RG32F:
            case PixelFormat::Depth16_Stencil8:
            case PixelFormat::Depth24_Stencil8:
            case PixelFormat::Depth32F_Stencil8: return 2;

            case PixelFormat::RGB8_SNorm:
            case PixelFormat::RGB8_UNorm:
            case PixelFormat::RGB16_SNorm:
            case PixelFormat::RGB16_UNorm:
            case PixelFormat::BGR8_UNorm:
            case PixelFormat::RGB8I:
            case PixelFormat::RGB8U:
            case PixelFormat::RGB16I:
            case PixelFormat::RGB16U:
            case PixelFormat::RGB32I:
            case PixelFormat::RGB32U:
            case PixelFormat::RGB16F:
            case PixelFormat::RGB32F:
            case PixelFormat::sRGB8:
            case PixelFormat::sBGR8:
            case PixelFormat::RGB_5_6_5_UNorm:
            case PixelFormat::BGR_11_11_10F: return 3;

            case PixelFormat::RGBA8_SNorm:
            case PixelFormat::RGBA8_UNorm:
            case PixelFormat::RGBA16_SNorm:
            case PixelFormat::RGBA16_UNorm:
            case PixelFormat::BGRA8_UNorm:
            case PixelFormat::RGBA8I:
            case PixelFormat::RGBA8U:
            case PixelFormat::RGBA16I:
            case PixelFormat::RGBA16U:
            case PixelFormat::RGBA32I:
            case PixelFormat::RGBA32U:
            case PixelFormat::RGBA16F:
            case PixelFormat::RGBA32F:
            case PixelFormat::sRGBA8:
            case PixelFormat::sBGRA8: return 4;

            default: return 0u;
        }

        return 0u;
    }

    constexpr inline core::UInt8 getArraySizeByChannelFor(PixelFormat format) noexcept {
        switch (format) {
            case PixelFormat::R8_SNorm:
            case PixelFormat::R8_UNorm:
            case PixelFormat::RG8_SNorm:
            case PixelFormat::RG8_UNorm:
            case PixelFormat::R8I:
            case PixelFormat::R8U:
            case PixelFormat::RG8I:
            case PixelFormat::RG8U:
            case PixelFormat::RGB8_SNorm:
            case PixelFormat::RGB8_UNorm:
            case PixelFormat::BGR8_UNorm:
            case PixelFormat::RGB8I:
            case PixelFormat::RGB8U:
            case PixelFormat::RGBA8_SNorm:
            case PixelFormat::RGBA8_UNorm:
            case PixelFormat::RGBA16_SNorm:
            case PixelFormat::BGRA8_UNorm:
            case PixelFormat::sRGB8:
            case PixelFormat::sBGR8:
            case PixelFormat::sRGBA8:
            case PixelFormat::sBGRA8: return 1u;

            case PixelFormat::R16_SNorm:
            case PixelFormat::R16_UNorm:
            case PixelFormat::R16I:
            case PixelFormat::R16U:
            case PixelFormat::RG16_SNorm:
            case PixelFormat::RG16_UNorm:
            case PixelFormat::RG16I:
            case PixelFormat::RG16U:
            case PixelFormat::RG16F:
            case PixelFormat::RGB16I:
            case PixelFormat::RGB16U:
            case PixelFormat::RGB16F:
            case PixelFormat::RGBA16I:
            case PixelFormat::RGBA16U:
            case PixelFormat::RGBA16F:
            case PixelFormat::R16F: return 2u;

            case PixelFormat::R32I:
            case PixelFormat::R32U:
            case PixelFormat::R32F:
            case PixelFormat::RG32I:
            case PixelFormat::RG32U:
            case PixelFormat::RG32F:
            case PixelFormat::RGB16_SNorm:
            case PixelFormat::RGB32I:
            case PixelFormat::RGB32U:
            case PixelFormat::RGB32F:
            case PixelFormat::RGBA8I:
            case PixelFormat::RGBA8U:
            case PixelFormat::RGBA32I:
            case PixelFormat::RGBA32U:
            case PixelFormat::RGBA32F: return 4u;

            default: return 0u;
        }

        return 0u;
    }

    enum class TextureTiling {
        Optimal = 0u,
        Linear  = 1u,
        DRM_Ext = 1000158000
    };
} // namespace storm::render

FLAG_ENUM(storm::render::QueueFlag)
FLAG_ENUM(storm::render::ShaderStage)
FLAG_ENUM(storm::render::SampleCountFlag)
FLAG_ENUM(storm::render::ColorComponent)
FLAG_ENUM(storm::render::TextureAspectMask)
FLAG_ENUM(storm::render::TextureCreateFlag)
FLAG_ENUM(storm::render::CullMode)
FLAG_ENUM(storm::render::HardwareBufferUsage)
FLAG_ENUM(storm::render::TextureUsage)
FLAG_ENUM(storm::render::MemoryProperty)
FLAG_ENUM(storm::render::AccessFlag)
FLAG_ENUM(storm::render::PipelineStageFlag)
FLAG_ENUM(storm::render::DependencyFlag)

#include "Enums.inl"
