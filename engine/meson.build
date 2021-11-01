cmake = import('cmake')

static = 'OFF'
if lib_type == 'static'
    static = 'ON'
endif

buildtype = 'Debug'
if build_type == 'release'
    buildtype = 'Release'
endif

rapid_json_dep = dependency('RapidJSON')

public_dependencies = [core_dep, render_dep, entities_dep, log_dep, rapid_json_dep]
private_dependencies = []

prog_glslang = find_program('glslangValidator')
command_glslang = ['--target-env', 'vulkan1.0', '-V', '-x', '-o', '@OUTPUT@', '@INPUT@']

shader_compiler = generator(
    prog_glslang,
    output: '@BASENAME@.spv.hpp',
    arguments: command_glslang
)

shaders = files([
    'src/render/shaders/CubeMap.vert.glsl',
    'src/render/shaders/CubeMap.frag.glsl',
    'src/render/shaders/FilterCubeMap.vert.glsl',
    'src/render/shaders/GenBRDF.frag.glsl',
    'src/render/shaders/GenBRDF.vert.glsl',
    'src/render/shaders/GenIrradienceCubeMap.frag.glsl',
    'src/render/shaders/GenPrefilteredEnvCubeMap.frag.glsl',
    'src/render/shaders/PBRColorPass.vert.glsl',
    'src/render/shaders/PBRColorPass.frag.glsl',
    'src/render/shaders/PbrForward.frag.glsl',
    'src/render/shaders/PbrForward.vert.glsl',
])

#ide:editable-filelist
shader_files = shader_compiler.process(shaders)

#ide:editable-filelist
base_headers = files([
    'include/storm/engine/Engine.hpp',
    'include/storm/engine/Fwd.hpp',
    'include/storm/engine/NameComponent.hpp',
    'include/storm/engine/Profiler.hpp',
    'include/storm/engine/State.hpp',

    'include/storm/engine/Engine.inl',
    'include/storm/engine/Profiler.inl',
    'include/storm/engine/State.inl',
])

data_headers = files([
    'include/storm/engine/data/Fwd.hpp',
])

render_headers = files([
    'include/storm/engine/render/Camera.hpp',
    'include/storm/engine/render/BoundingBox.hpp',
    'include/storm/engine/render/Drawable.hpp',
    'include/storm/engine/render/DrawableComponent.hpp',
    'include/storm/engine/render/Fwd.hpp',
    'include/storm/engine/render/Material.hpp',
    'include/storm/engine/render/MaterialComponent.hpp',
    'include/storm/engine/render/RenderSystem.hpp',
    'include/storm/engine/render/RenderQueue.hpp',
    'include/storm/engine/render/Transform.hpp',
    'include/storm/engine/render/TransformComponent.hpp',
    'include/storm/engine/render/Vertex.hpp',

    'include/storm/engine/render/3D/FPSCamera.hpp',
    'include/storm/engine/render/3D/Fwd.hpp',
    'include/storm/engine/render/3D/Model.hpp',
    'include/storm/engine/render/3D/PbrMaterial.hpp',
    'include/storm/engine/render/3D/PbrMesh.hpp',
    'include/storm/engine/render/3D/PbrRenderSystem.hpp',

    'include/storm/engine/render/utils/DebugGUI.hpp',
    'include/storm/engine/render/utils/DeletionQueue.hpp',
    'include/storm/engine/render/utils/Fwd.hpp',
    'include/storm/engine/render/utils/RingHardwareBuffer.hpp',
    'include/storm/engine/render/utils/ResourceCache.hpp',

    'include/storm/engine/render/framegraph/Fwd.hpp',
    'include/storm/engine/render/framegraph/FrameGraph.hpp',
    'include/storm/engine/render/framegraph/FrameGraphBuilder.hpp',
    'include/storm/engine/render/framegraph/FrameGraphNode.hpp',
    'include/storm/engine/render/framegraph/FrameGraphResource.hpp',

    'include/storm/engine/render/Camera.inl',
    'include/storm/engine/render/Drawable.inl',
    'include/storm/engine/render/Material.inl',
    'include/storm/engine/render/RenderSystem.inl',
    'include/storm/engine/render/RenderQueue.inl',
    'include/storm/engine/render/Transform.inl',
    'include/storm/engine/render/Vertex.inl',

    'include/storm/engine/render/3D/FPSCamera.inl',
    'include/storm/engine/render/3D/Model.inl',
    'include/storm/engine/render/3D/PbrMaterial.inl',
    'include/storm/engine/render/3D/PbrMesh.inl',
    'include/storm/engine/render/3D/PbrRenderSystem.inl',

    'include/storm/engine/render/utils/DebugGUI.inl',
    'include/storm/engine/render/utils/DeletionQueue.inl',
    'include/storm/engine/render/utils/RingHardwareBuffer.inl',

    'include/storm/engine/render/framegraph/FrameGraph.inl',
    'include/storm/engine/render/framegraph/FrameGraphBuilder.inl',
    'include/storm/engine/render/framegraph/FrameGraphNode.inl',
    'include/storm/engine/render/framegraph/FrameGraphResource.inl',
])

private_headers = files([
    'src/Log.hpp',

    'src/render/imgui/imconfig.hpp',
    'src/render/imgui/imgui.hpp',
    'src/render/imgui/imgui_impl_vulkan.hpp',
    'src/render/imgui/imgui_internal.hpp',
    'src/render/imgui/imstb_rectpack.hpp',
    'src/render/imgui/imstb_textedit.hpp',
    'src/render/imgui/imstb_truetype.hpp',
    'src/render/imgui/imgui_widget_flamegraph.hpp',
])

#ide:editable-filelist
sources = files([
    'src/Engine.cpp', 
    'src/Profiler.cpp',
    'src/State.cpp',

    'src/data/tiny_gltf.hpp',
    'src/data/tiny_gltf.cpp',

    'src/render/Camera.cpp',
    'src/render/Material.cpp',
    'src/render/Drawable.cpp',
    'src/render/RenderSystem.cpp',
    'src/render/RenderQueue.cpp',
    'src/render/Transform.cpp',
    'src/render/Vertex.cpp',

    'src/render/3D/FPSCamera.cpp',
    'src/render/3D/Model.cpp',
    'src/render/3D/PbrMaterial.cpp',
    'src/render/3D/PbrMesh.cpp',
    'src/render/3D/PbrRenderSystem.cpp',

    'src/render/utils/DebugGUI.cpp',
    'src/render/utils/DeletionQueue.cpp',
    'src/render/utils/RingHardwareBuffer.cpp',
    'src/render/utils/ResourceCache.cpp',

    'src/render/imgui/imconfig.hpp',
    'src/render/imgui/imgui.cpp',
    'src/render/imgui/imgui_draw.cpp',
    'src/render/imgui/imgui_tables.cpp',
    'src/render/imgui/imgui_impl_vulkan.cpp',
    'src/render/imgui/imgui_widgets.cpp',
    'src/render/imgui/imgui_widget_flamegraph.cpp',

    'src/render/framegraph/FrameGraph.cpp',
    'src/render/framegraph/FrameGraphBuilder.cpp',
    'src/render/framegraph/FrameGraphNode.cpp',
    'src/render/framegraph/FrameGraphResource.cpp',
])

engine_inc = ['include/']

headers = base_headers + data_headers + render_headers

if lib_type == 'shared'
    engine_lib_name = 'StormKit-engine' + suffix
    engine_lib = shared_library(
        engine_lib_name,
        sources,
        shader_files,
        extra_files: headers + shaders + private_headers,
        gnu_symbol_visibility: 'hidden',
        dependencies: public_dependencies + private_dependencies,
        include_directories: engine_inc,
        install: true
    )
    destination_path = join_paths(meson.project_build_root(), 'output/bin/')
else
    engine_lib_name = 'StormKit-engine-static' + suffix
    engine_lib = static_library(
        engine_lib_name,
        sources,
        shader_files,
        extra_files: headers + shaders + private_headers,
        gnu_symbol_visibility: 'hidden',
        dependencies: public_dependencies + private_dependencies,
        include_directories: engine_inc,
        install: true
    )
    destination_path = join_paths(meson.project_build_root(), 'output/lib/')
endif

if copy_to_bindir
    engine_copy_target = custom_target(
        'engine_copy_target',
        input: engine_lib,
        output: 'engine-fake',
        command: [copy_file, '@INPUT@', destination_path],
        install: false,
        build_by_default: true
    )
endif
engine_dep = declare_dependency(
    link_with: engine_lib,
    dependencies: public_dependencies,
    include_directories: engine_inc,
    compile_args: cpp_arguments,
)

if get_option('enable_examples')
    subdir('samples')
endif

install_headers(base_headers, subdir : 'storm/engine')
install_headers(data_headers, subdir : 'storm/engine/data')
install_headers(render_headers, subdir : 'storm/engine/render')
