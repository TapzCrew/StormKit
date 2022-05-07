vulkan_dep  = dependency('vulkan-headers', fallback: ['vulkan-headers', 'vulkan_headers_dep'])

enable_spirv_introspect = get_option('enable_render_introspect_spirv')

args = {}
if enable_spirv_introspect
cmake = import('cmake')

spirv_cross_core_dep = dependency('spirv_cross_core', method: 'cmake', modules: ['spirv-cross-core'], required: false)
spirv_cross_cpp_dep = dependency('spirv_cross_cpp', method: 'cmake', modules: ['spirv-cross-cpp'], required: false)
spirv_cross_glsl_dep = dependency('spirv_cross_glsl', method: 'cmake', modules: ['spirv-cross-glsl'], required: false)

if not spirv_cross_cpp_dep.found() or not spirv_cross_core_dep.found() or not spirv_cross_glsl_dep.found()
    spirv_cross_opt = cmake.subproject_options()
    spirv_cross_opt.add_cmake_defines({'SPIRV_CROSS_SHARED': false})
    spirv_cross_opt.add_cmake_defines({'SPIRV_CROSS_STATIC': true})
    spirv_cross_opt.add_cmake_defines({'SPIRV_CROSS_CLI': false})
    spirv_cross_opt.add_cmake_defines({'SPIRV_CROSS_ENABLE_TESTS': false})

    spirv_cross_opt.add_cmake_defines({'SPIRV_CROSS_ENABLE_GLSL': true})
    spirv_cross_opt.add_cmake_defines({'SPIRV_CROSS_ENABLE_HLSL': false})
    spirv_cross_opt.add_cmake_defines({'SPIRV_CROSS_ENABLE_MSL': false})
    spirv_cross_opt.add_cmake_defines({'SPIRV_CROSS_ENABLE_CPP': true})
    spirv_cross_opt.add_cmake_defines({'SPIRV_CROSS_ENABLE_REFLECT': true})
    spirv_cross_opt.add_cmake_defines({'SPIRV_CROSS_ENABLE_UTIL': false})

    spirv_cross_opt.add_cmake_defines({'SPIRV_CROSS_FORCE_PIC': true})
    if get_option('buildtype') == 'debug'
        spirv_cross_opt.add_cmake_defines({'CMAKE_BUILD_TYPE': 'Debug'})
    else
        spirv_cross_opt.add_cmake_defines({'CMAKE_BUILD_TYPE': 'Release'})
    endif

    spirv_cross = cmake.subproject('SPIRV-Cross', options: spirv_cross_opt)

    spirv_cross_core_dep = spirv_cross.dependency('spirv-cross-core')
    spirv_cross_cpp_dep = spirv_cross.dependency('spirv-cross-cpp')
    spirv_cross_glsl_dep = spirv_cross.dependency('spirv-cross-glsl')
endif
private_dependencies = [spirv_cross_cpp_dep, spirv_cross_glsl_dep, spirv_cross_core_dep]
args += {
'cpp_args': ['-DSTORMKIT_ENABLE_SPIRV_INTROSPECT']
}
endif

vulkan_headers_dep = declare_dependency(dependencies: [vulkan_dep])

public_dependencies = [core_dep, log_dep, image_dep, module_dep, vulkan_headers_dep]

if enable_window
    public_dependencies += [window_dep]
endif


#ide:editable-filelist
general_headers = files([
   'include/storm/render/Fwd.hpp'
])

#ide:editable-filelist
core_headers = files([
    'include/storm/render/core/vk_mem_alloc.h',
    'include/storm/render/core/Fwd.hpp',
    'include/storm/render/core/Vulkan.hpp',
    'include/storm/render/core/DynamicLoader.hpp',
    'include/storm/render/core/Enums.hpp',
    'include/storm/render/core/Instance.hpp',
    'include/storm/render/core/RenderCapabilities.hpp',
    'include/storm/render/core/MemoryBarrier.hpp',
    'include/storm/render/core/PhysicalDevice.hpp',
    'include/storm/render/core/PhysicalDeviceInfo.hpp',
    'include/storm/render/core/Device.hpp',
    'include/storm/render/core/QueueFamily.hpp',
    'include/storm/render/core/Surface.hpp',
    'include/storm/render/core/WindowSurface.hpp',
    'include/storm/render/core/OffscreenSurface.hpp',
    'include/storm/render/core/Queue.hpp',
    'include/storm/render/core/Command.hpp',
    'include/storm/render/core/CommandBuffer.hpp',
    'include/storm/render/core/Types.hpp',

    'include/storm/render/core/Enums.inl',
    'include/storm/render/core/Instance.inl',
    'include/storm/render/core/PhysicalDevice.inl',
    'include/storm/render/core/Device.inl',
    'include/storm/render/core/Surface.inl',
    'include/storm/render/core/WindowSurface.inl',
    'include/storm/render/core/Queue.inl',
    'include/storm/render/core/CommandBuffer.inl',
])

#ide:editable-filelist
resource_headers = files([
    'include/storm/render/resource/Fwd.hpp',
    'include/storm/render/resource/Shader.hpp',
    'include/storm/render/resource/TextureSubresource.hpp',
    'include/storm/render/resource/TextureSubresourceRange.hpp',
    'include/storm/render/resource/TextureSubresourceLayers.hpp',
    'include/storm/render/resource/Texture.hpp',
    'include/storm/render/resource/TextureView.hpp',
    'include/storm/render/resource/Sampler.hpp',
    'include/storm/render/resource/HardwareBuffer.hpp',

    'include/storm/render/resource/Shader.inl',
    'include/storm/render/resource/Texture.inl',
    'include/storm/render/resource/TextureView.inl',
    'include/storm/render/resource/Sampler.inl',
    'include/storm/render/resource/HardwareBuffer.inl',
])

#ide:editable-filelist
pipeline_headers = files([
    'include/storm/render/pipeline/Fwd.hpp',
    'include/storm/render/pipeline/AbstractPipeline.hpp',
    'include/storm/render/pipeline/GraphicsPipeline.hpp',
    'include/storm/render/pipeline/GraphicsPipelineState.hpp',
    'include/storm/render/pipeline/ComputePipeline.hpp',
    'include/storm/render/pipeline/ComputePipelineState.hpp',
    'include/storm/render/pipeline/RenderPassDescription.hpp',
    'include/storm/render/pipeline/RenderPass.hpp',
    'include/storm/render/pipeline/Framebuffer.hpp',
    'include/storm/render/pipeline/VertexBindingDescription.hpp',
    'include/storm/render/pipeline/VertexInputAttributeDescription.hpp',
    'include/storm/render/pipeline/DescriptorSetLayout.hpp',
    'include/storm/render/pipeline/DescriptorPool.hpp',
    'include/storm/render/pipeline/DescriptorSet.hpp',
    'include/storm/render/pipeline/PipelineCache.hpp',

    'include/storm/render/pipeline/PipelineCache.inl',
    'include/storm/render/pipeline/AbstractPipeline.inl',
    'include/storm/render/pipeline/GraphicsPipeline.inl',
    'include/storm/render/pipeline/ComputePipeline.inl',
    'include/storm/render/pipeline/RenderPass.inl',
    'include/storm/render/pipeline/Framebuffer.inl',
    'include/storm/render/pipeline/DescriptorSetLayout.inl',
    'include/storm/render/pipeline/DescriptorPool.inl',
    'include/storm/render/pipeline/DescriptorSet.inl',
])

#ide:editable-filelist
sync_headers = files([
    'include/storm/render/sync/Fwd.hpp',
    'include/storm/render/sync/Semaphore.hpp',
    'include/storm/render/sync/Fence.hpp',

    'include/storm/render/sync/Semaphore.inl',
    'include/storm/render/sync/Fence.inl',
])


private_headers = files([
    'src/Log.hpp'
])

#ide:editable-filelist
sources = files([
    'src/core/Instance.cpp',
    'src/core/DynamicLoader.cpp',
    'src/core/PhysicalDevice.cpp',
    'src/core/Device.cpp',
    'src/core/Surface.cpp',
    'src/core/WindowSurface.cpp',
    'src/core/OffscreenSurface.cpp',
    'src/core/Queue.cpp',
    'src/core/CommandBuffer.cpp',
    'src/core/Types.cpp',

    'src/resource/Shader.cpp',
    'src/resource/Texture.cpp',
    'src/resource/TextureView.cpp',
    'src/resource/ConvertFormatShaders.hpp',
    'src/resource/Sampler.cpp',
    'src/resource/HardwareBuffer.cpp',

    'src/pipeline/AbstractPipeline.cpp',
    'src/pipeline/GraphicsPipeline.cpp',
    'src/pipeline/GraphicsPipelineState.cpp',
    'src/pipeline/ComputePipeline.cpp',
    'src/pipeline/ComputePipelineState.cpp',
    'src/pipeline/RenderPassDescription.cpp',
    'src/pipeline/RenderPass.cpp',
    'src/pipeline/Framebuffer.cpp',
    'src/pipeline/DescriptorSetLayout.cpp',
    'src/pipeline/DescriptorPool.cpp',
    'src/pipeline/DescriptorSet.cpp',
    'src/pipeline/VertexBindingDescription.cpp',
    'src/pipeline/VertexInputAttributeDescription.cpp',
    'src/pipeline/PipelineCache.cpp',

    'src/sync/Semaphore.cpp',
    'src/sync/Fence.cpp',
])

headers = general_headers + core_headers + resource_headers + pipeline_headers + sync_headers

args += {'extra_files': headers + private_headers, 'gnu_symbol_visibility': 'hidden'}

public_render_inc = ['include/']
private_render_inc = []

args += {
    'dependencies': public_dependencies + private_dependencies,
    'include_directories': public_render_inc + private_render_inc
}

if lib_type == 'shared'
    render_lib_name = 'StormKit-render' + suffix
    render_lib = shared_library(
        render_lib_name,
        sources,
        kwargs: args,
        install: true
    )
    destination_path = join_paths(meson.project_build_root(), 'output/bin/')
else
    render_lib_name = 'StormKit-render-static' + suffix
    render_lib = static_library(
        render_lib_name,
        sources,
        kwargs: args,
        install: true
    )
    destination_path = join_paths(meson.project_build_root(), 'output/lib/')
endif

if copy_to_bindir
    render_copy_target = custom_target(
        'render_copy_target',
        input: render_lib,
        output: 'render-fake',
        command: [copy_file, '@INPUT@', destination_path],
        install: false,
        build_by_default: true
    )
endif

render_dep = declare_dependency(
    link_with: render_lib,
    dependencies: public_dependencies,
    include_directories: public_render_inc,
    compile_args: cpp_arguments,
)

if get_option('enable_examples') and enable_window
    subdir('samples')
endif

install_headers(general_headers, subdir : 'storm/render/')
install_headers(core_headers, subdir : 'storm/render/core')
install_headers(resource_headers, subdir : 'storm/render/resource')
install_headers(pipeline_headers, subdir : 'storm/render/pipeline')
install_headers(sync_headers, subdir : 'storm/render/sync')
