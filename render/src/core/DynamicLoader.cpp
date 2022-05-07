#include <storm/render/core/DynamicLoader.hpp>

using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
DynamicLoader::DynamicLoader(std::filesystem::path vulkan_lib_path) : m_module { vulkan_lib_path } {
    m_success = m_module.isLoaded();

    if (m_success) {
        m_vkGetInstanceProcAddr =
            m_module.getFunc<GetInstanceProcAddrType>("vkGetInstanceProcAddr");

        m_pfn = *m_vkGetInstanceProcAddr.target<PFN_vkGetInstanceProcAddr>();
    }
}

/////////////////////////////////////
/////////////////////////////////////
DynamicLoader::~DynamicLoader() = default;

/////////////////////////////////////
/////////////////////////////////////
DynamicLoader::DynamicLoader(DynamicLoader &&) = default;

/////////////////////////////////////
/////////////////////////////////////
DynamicLoader &DynamicLoader::operator=(DynamicLoader &&) = default;
