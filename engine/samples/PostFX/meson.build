dependencies = [core_dep, engine_dep, render_dep, window_dep, main_dep]

shader_files = shader_compiler.process(
    'shaders/mesh.vert.glsl', 
    'shaders/mesh.frag.glsl',
    'shaders/sepia.vert.glsl', 
    'shaders/sepia.frag.glsl'
)

#ide:editable-filelist
sources = files([
    'src/main.cpp',
    'src/App.cpp',
    'src/App.hpp'
])

executable(
    'PostFX',
    sources, 
    shader_files,
    dependencies: dependencies
)