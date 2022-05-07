# Installation

A C++20 complient compiler is needed to build StormKit

## Dependencies
### StormKit-core module
- [{fmt}](https://fmt.dev/latest/index.html)

### StormKit-image module
- [LibPNG >= 1.6](http://www.libpng.org/pub/png/libpng.html)
- [LibJPEG-turbo](https://www.libjpeg-turbo.org)

### stormkit-gpu module
- [Vulkan](https://vulkan.lunarg.com)

### StormKit-engine module
- [glm](https://github.com/g-truc/glm)
- [gli](https://github.com/g-truc/gli)

## Building
### Windows
First you need to [download](https://vulkan.lunarg.com/sdk/home#sdk/downloadConfirm/latest/windows/vulkan-sdk.exe) and set an env var (VULKAN_SDK) to its root directory
```
set VULKAN_SDK=C:\Dev\VulkanSDK\<VERSION>
vcpkg install --triplet x64-windows vulkan-sdk libpng libjpeg-turbo fmt gli glm range-v3 ms-gsl

#Configure the project
## Dynamic library
meson builddir -Ddefault_library=shared
## Static library
meson builddir -Ddefault_library=static
## if you use vcpkg add --cmake_prefix-path
meson builddir --cmake-prefix-path=<PATHTOVCPKG>/installed/x64-windows

#Build the project
ninja -C builddir
```

### Linux / macOS
```
#Configure the project
## Dynamic library
meson builddir -Ddefault_library=shared
## Static library
meson builddir -Ddefault_library=static

#Build the project
ninja -C builddir
``` 

## Meson parameters
You can customize your build (with -Doption=value) with the following parameters 

|       Variable        |         Description          |                                   Default value                                       |
|-----------------------|------------------------------|---------------------------------------------------------------------------------------|
| enable_doc            | Generate documentation       | false                                                                                 |
| enable_examples       | Build examples               | false                                                                                 |
| enable_entities       | Build storm entities library | true                                                                                  |
| enable_log            | Build storm log library      | true                                                                                  |
| enable_window         | Build storm window library   | true                                                                                  |
| enable_image          | Build storm image library    | true                                                                                  |
| enable_module         | Build storm module library   | true                                                                                  |
| enable_render         | Build storm render library   | true                                                                                  |
| enable_engine       | Build storm engine library | true                                                                                  |