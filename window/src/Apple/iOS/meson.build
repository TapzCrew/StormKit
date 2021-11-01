add_languages('objc')
add_languages('objcpp')

private_dependencies = [window_dep]

sources = files([
    'src/View.m',
    'src/ViewController.m',
    'src/WindowImpl.mm',
    'src/InputHandlerImpl.mm',
    'src/AppDelegate.mm',
    'src/VideoSettings.mm',
    'src/iosMain.cpp',
    'src/View.h',
    'src/ViewController.h'    ,
    'src/WindowImpl.hpp',
    'src/AppDelegate.hpp',
    'src/InputHandlerImpl.hpp',
    'src/iosMain.hpp',
])

ios_plugin = shared_library(
    'StormKit-window-iOS' + suffix
    sources,
    dependencies: private_dependencies,
    objc_args: ['-std=c11']
    objcpp_args: ['-std=c++2a']
    install: true
)


