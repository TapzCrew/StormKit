# Installation

A C++23 compliant compiler is needed to build StormKit and XMake build system

## Dependencies
### StormKit-core module
- [glm](https://github.com/g-truc/glm)

### StormKit-image module
- [LibPNG >= 1.6](http://www.libpng.org/pub/png/libpng.html)
- [LibJPEG-turbo](https://www.libjpeg-turbo.org)

### stormkit-gpu module
- [Vulkan](https://vulkan.lunarg.com)
- [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
- [VulkanMemoryAllocator-hpp](https://github.com/YaaZ/VulkanMemoryAllocator-Hpp)

### StormKit-engine module
- [gli](https://github.com/g-truc/gli)

## Building
```
> xmake f -m <release|debug|releasedbg> -k <shared|static>
> xmake b
```

## Parameters
You can customize your build (with --option=value) with the following parameters 

|       Variable        |         Description             |                                   Default value                                       |
|-----------------------|---------------------------------|---------------------------------------------------------------------------------------|
| enable_tests          | Build tests                     | no                                                                                    |
| enable_examples       | Build examples                  | no                                                                                    |
| enable_log            | Build stormkit log library      | yes                                                                                   |
| enable_entities       | Build stormkit entities library | yes                                                                                   |
| enable_image          | Build stormkit image library    | yes                                                                                   |
| enable_wsi            | Build stormkit wsi library      | yes                                                                                   |
| enable_gpu            | Build stormkit gpu    library   | yes                                                                                   |
| enable_engine         | Build stormkit engine library   | yes                                                                                   |
