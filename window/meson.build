public_dependencies = [core_dep, module_dep, log_dep]
private_dependencies = []

#ide:editable-filelist
public_headers = files([
    'include/storm/window/Event.hpp',
    'include/storm/window/EventHandler.hpp',
    'include/storm/window/Fwd.hpp',
    'include/storm/window/Mouse.hpp',
    'include/storm/window/Keyboard.hpp',
    'include/storm/window/Key.hpp',
    'include/storm/window/MouseButton.hpp',
    'include/storm/window/VideoSettings.hpp',
    'include/storm/window/Window.hpp',
    'include/storm/window/WindowStyle.hpp',

    'include/storm/window/EventHandler.inl',
    'include/storm/window/Mouse.inl',
    'include/storm/window/Keyboard.inl',
    'include/storm/window/Window.inl',
])

private_headers = files([
    'src/Log.hpp',
    'src/AbstractWindow.hpp',
    'src/AbstractWindow.inl',
])

linux_headers = files([
   'src/Linux/Utils.hpp'
])

x11_headers = files([
    'src/Linux/X11/Log.hpp',
    'src/Linux/X11/Utils.hpp',
    'src/Linux/X11/X11Mouse.hpp',
    'src/Linux/X11/X11Keyboard.hpp',
    'src/Linux/X11/X11Window.hpp',
])

win32_headers = files([
    'src/Win32/Log.hpp',
    'src/Win32/Utils.hpp',
    'src/Win32/Win32Mouse.hpp',
    'src/Win32/Win32Keyboard.hpp',
    'src/Win32/Win32Window.hpp',
])

wayland_headers = files([
    'src/Linux/Wayland/Callback.hpp',
    'src/Linux/Wayland/Log.hpp',
    'src/Linux/Wayland/WaylandMouse.hpp',
    'src/Linux/Wayland/WaylandKeyboard.hpp',
    'src/Linux/Wayland/WaylandWindow.hpp',
])

macos_headers = files([
    'src/Apple/macOS/AutoReleasePoolWrapper.hpp',
    'src/Apple/macOS/Log.hpp',
    'src/Apple/macOS/macOSKeyboard.hpp',
    'src/Apple/macOS/macOSMouse.hpp',
    'src/Apple/macOS/macOSWindow.hpp',
    'src/Apple/macOS/StormApplication.h',
    'src/Apple/macOS/StormApplicationDelegate.h',
    'src/Apple/macOS/StormWindowController.hpp',
    'src/Apple/macOS/StormWindow.h',
    'src/Apple/macOS/StormView.hpp',
    'src/Apple/macOS/Utils.hpp',
])

#ide:editable-filelist
sources = files([
    'src/AbstractWindow.cpp',
    'src/EventHandler.cpp',
    'src/Window.cpp',
])

linux_sources = files([
    'src/Linux/Utils.cpp'
])

x11_sources = files([
    'src/Linux/X11/Utils.cpp',
    'src/Linux/X11/X11Mouse.cpp',
    'src/Linux/X11/X11Keyboard.cpp',
    'src/Linux/X11/X11Window.cpp',
])

win32_sources = files([
    'src/Win32/Utils.cpp',
    'src/Win32/Win32Mouse.cpp',
    'src/Win32/Win32Keyboard.cpp',
    'src/Win32/Win32Window.cpp',
])

wayland_sources = files([
    'src/Linux/Wayland/Callback.cpp',
    'src/Linux/Wayland/WaylandMouse.cpp',
    'src/Linux/Wayland/WaylandKeyboard.cpp',
    'src/Linux/Wayland/WaylandWindow.cpp',
])

macos_sources = files([
    'src/Apple/macOS/AutoReleasePoolWrapper.mm',
    'src/Apple/macOS/macOSKeyboard.mm',
    'src/Apple/macOS/macOSMouse.mm',
    'src/Apple/macOS/macOSWindow.mm',
    'src/Apple/macOS/StormApplication.m',
    'src/Apple/macOS/StormApplicationDelegate.m',
    'src/Apple/macOS/StormWindowController.mm',
    'src/Apple/macOS/StormWindow.m',
    'src/Apple/macOS/StormView.mm',
    'src/Apple/macOS/Utils.mm',
])

headers = public_headers + private_headers + x11_headers + wayland_headers + win32_headers + linux_headers + macos_headers
extra_files = headers

if is_linux
    sources += linux_sources

    xkbcommon_dep     = dependency('xkbcommon')

    private_dependencies += [xkbcommon_dep]
else
    extra_files += linux_sources
endif

if is_linux and enable_window_xcb
    sources += x11_sources

    xcb_dep           = dependency('xcb')
    xcb_key_dep       = dependency('xcb-keysyms')
    xcb_icccm_dep     = dependency('xcb-icccm')
    xcb_randr_dep     = dependency('xcb-randr')
    xcb_xfixes_dep       = dependency('xcb-xfixes')
    xcb_xkb_dep       = dependency('xcb-xkb')
    xkbcommon_x11_dep = dependency('xkbcommon-x11')

    private_dependencies += [xcb_dep, xcb_key_dep, xcb_icccm_dep, xcb_randr_dep, xcb_xfixes_dep, xcb_xkb_dep, xkbcommon_x11_dep]
else
    extra_files += x11_sources
endif

if is_linux and enable_window_wayland
    sources += wayland_sources

    wayland_dep = dependency('wayland-client')
    wayland_cursor_dep = dependency('wayland-cursor')

    prog_wayland_scanner = find_program('wayland-scanner')

    wayland_scanner_header_generator = generator(
        prog_wayland_scanner,
        output: '@BASENAME@.h',
        arguments: ['client-header', '@INPUT@', '@OUTPUT@']
    )
    wayland_scanner_source_generator = generator(
        prog_wayland_scanner,
        output: '@BASENAME@.c',
        arguments: ['code', '@INPUT@', '@OUTPUT@']
    )

    xdg_shell_header = wayland_scanner_header_generator.process('src/Linux/Wayland/xdg-shell.xml')
    xdg_shell_source = wayland_scanner_source_generator.process('src/Linux/Wayland/xdg-shell.xml')

    xdg_decoration_header = wayland_scanner_header_generator.process('src/Linux/Wayland/xdg-decoration-unstable-v1.xml')
    xdg_decoration_source = wayland_scanner_source_generator.process('src/Linux/Wayland/xdg-decoration-unstable-v1.xml')

    wp_pointer_constraints_header = wayland_scanner_header_generator.process('src/Linux/Wayland/pointer-constraints-unstable-v1.xml')
    wp_pointer_constraints_source = wayland_scanner_source_generator.process('src/Linux/Wayland/pointer-constraints-unstable-v1.xml')

    wp_relative_pointer_header = wayland_scanner_header_generator.process('src/Linux/Wayland/relative-pointer-unstable-v1.xml')
    wp_relative_pointer_source = wayland_scanner_source_generator.process('src/Linux/Wayland/relative-pointer-unstable-v1.xml')

    private_dependencies += [wayland_dep, wayland_cursor_dep]

    headers += [xdg_shell_header,
                xdg_decoration_header,
                wp_pointer_constraints_header,
                wp_relative_pointer_header]
    sources += [xdg_shell_header,
                xdg_shell_source,
                xdg_decoration_header,
                xdg_decoration_source,
                wp_pointer_constraints_header,
                wp_pointer_constraints_source,
                wp_relative_pointer_header,
                wp_relative_pointer_source]
else
    extra_files += wayland_sources
endif

if is_windows
    sources += win32_sources
else
    extra_files += win32_sources
endif

if is_macos
    add_languages('objc')
    add_languages('objcpp')

    cocoa_dep = dependency('Cocoa')
    quartz_core_dep = dependency('QuartzCore')
    iokit_dep = dependency('IOKit')
    carbon_dep = dependency('Carbon')


    private_dependencies += [cocoa_dep, quartz_core_dep, iokit_dep, carbon_dep]

    sources += macos_sources
else
    extra_files += macos_sources
endif

dependencies = public_dependencies + private_dependencies

window_inc = ['include/']

if lib_type == 'shared'
    window_lib_name = 'StormKit-window' + suffix
    window_lib = shared_library(
        window_lib_name,
        sources,
        extra_files: extra_files,
        gnu_symbol_visibility: 'hidden',
        dependencies: dependencies,
        include_directories: window_inc,
        install: true
    )
    destination_path = join_paths(meson.project_build_root(), 'output/bin/')
else
    window_lib_name = 'StormKit-window-static' + suffix
    window_lib = static_library(
        window_lib_name,
        sources,
        extra_files: extra_files,
        gnu_symbol_visibility: 'hidden',
        dependencies: dependencies,
        include_directories: window_inc,
        install: true
    )
    destination_path = join_paths(meson.project_build_root(), 'output/lib/')
endif

if copy_to_bindir
    window_copy_target = custom_target(
        'window_copy_target',
        input: window_lib,
        output: 'window-fake',
        command: [copy_file, '@INPUT@', destination_path],
        install: false,
        build_by_default: true
    )
endif

window_dep = declare_dependency(
    link_with: window_lib,
    include_directories: window_inc,
    dependencies: public_dependencies,
    compile_args: cpp_arguments,
)

if get_option('enable_examples')
    subdir('samples')
endif

install_headers(public_headers, subdir : 'storm/window')
