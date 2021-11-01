prog_glslang = find_program('glslangValidator')
command_glslang = ['--target-env', 'vulkan1.0', '-V', '-x', '-o', '@OUTPUT@', '@INPUT@']

shader_compiler = generator(
    prog_glslang,
    output: '@BASENAME@.spv.hpp',
    arguments: command_glslang
)


subdir('Triangle/')
subdir('UniformBuffer/')
subdir('Texture/')

