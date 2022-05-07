if is_windows
   png_dep = dependency('PNG', fallback: ['libpng', 'png_dep'])
else 
   png_dep = dependency('libpng', fallback: ['libpng', 'png_dep'])
endif

if is_windows
    jpeg_dep = dependency('JPEG', fallback: ['libjpeg', 'jpeg_dep'])
else 
    jpeg_dep = dependency('libjpeg', fallback: ['libjpeg', 'jpeg_dep'])
endif

gli_dep = dependency('gli')

public_dependencies = [core_dep]
private_dependencies = [png_dep, jpeg_dep, gli_dep]

cpp_args = []

if get_option('image_enable_log')
    public_dependencies += [log_dep]
else
    if meson.get_compiler('cpp').get_argument_syntax() == 'msvc'
        cpp_args += ['/DSTORMKIT_IMAGE_NO_LOGGING']
    else
        cpp_args += ['-DSTORMKIT_IMAGE_NO_LOGGING']
    endif
endif

#ide:editable-filelist
headers = files([
    'include/storm/image/Fwd.hpp',
    'include/storm/image/Image.hpp',
])

#ide:editable-filelist
inls = files([
    'include/storm/image/Image.inl',
])

#ide:editable-filelist
sources = files([
    'src/Image.cpp',

    'src/JPEGImage.cpp',
    'src/PNGImage.cpp',
    'src/TARGAImage.cpp',
    'src/PPMImage.cpp',
    'src/HDRImage.cpp',
    'src/KTXImage.cpp',
])

image_inc = ['include/']

if lib_type == 'shared'
    image_lib_name = 'StormKit-image' + suffix
    image_lib = shared_library(
        image_lib_name,
        sources,
        headers,
        extra_files: headers + inls,
        gnu_symbol_visibility: 'hidden',
        dependencies: public_dependencies + private_dependencies,
        include_directories: image_inc,
        c_args: cpp_args,
        cpp_args: cpp_args,
        install: true
    )
    destination_path = join_paths(meson.project_build_root(), 'output/bin/')
else
    image_lib_name = 'StormKit-image-static' + suffix
    image_lib = static_library(
        image_lib_name,
        sources,
        headers,
        extra_files: headers + inls,
        gnu_symbol_visibility: 'hidden',
        dependencies: public_dependencies + private_dependencies,
        include_directories: image_inc,
        c_args: cpp_args,
        cpp_args: cpp_args,
        install: true
    )
    destination_path = join_paths(meson.project_build_root(), 'output/lib/')
endif

if copy_to_bindir
    image_copy_target = custom_target(
        'image_copy_target',
        input: image_lib,
        output: 'image-fake',
        command: [copy_file, '@INPUT@', destination_path],
        install: false,
        build_by_default: true
    )
endif

image_dep = declare_dependency(
    link_with: image_lib,
    dependencies: public_dependencies,
    include_directories: image_inc,
    compile_args: cpp_arguments,
)

install_headers(headers + inls, subdir : 'storm/image')
