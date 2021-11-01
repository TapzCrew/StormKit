/////////// - StormKit::render - ///////////
#include <storm/render/core/OffscreenSurface.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/core/WindowSurface.hpp>

#include <storm/render/pipeline/PipelineCache.hpp>

/////////// - StormKit::engine - ///////////
#include "Log.hpp"

#include <storm/engine/Engine.hpp>
#include <storm/engine/Profiler.hpp>

using namespace storm;
using namespace storm::engine;

#ifdef STORMKIT_OS_WINDOWS
    #include <shlobj_core.h>
auto getPipelineCacheDir() {
    auto path = std::array<char, MAX_PATH> {};

    SHGetFolderPathA(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, std::data(path));

    auto str = std::string { std::data(path) };

    return str;
}
#else
    #include <pwd.h>
    #include <sys/types.h>
    #include <unistd.h>

auto getPipelineCacheDir() {
    auto pw = getpwuid(getuid());

    auto str = std::string { pw->pw_dir };
    str += "/.cache/";

    return str;
}
#endif

Engine::Engine(const window::Window &window, std::string app_name) {
    m_instance = std::make_unique<render::Instance>(std::move(app_name));

    auto surface =
        m_instance->createWindowSurfacePtr(window, render::Surface::Buffering::Swapchain);

    const auto &physical_device      = m_instance->pickPhysicalDevice(*surface);
    const auto &physical_device_info = physical_device.info();
    dlog("Using physical device {}", physical_device_info.device_name);
    dlog("{}", physical_device_info);

    m_device = physical_device.createLogicalDevicePtr();
    dlog("Device successfully created");

    ilog("Render backend successfully initialized");

    m_surface = std::move(surface);

    initialize();
}

////////////////////////////////////////
////////////////////////////////////////
Engine::Engine(core::Extentu extent, render::Surface::Buffering buffering, std::string app_name) {
    m_instance = std::make_unique<render::Instance>(std::move(app_name));

    m_surface = m_instance->createOffscreenSurfacePtr(extent, buffering);

    const auto &physical_device      = m_instance->pickPhysicalDevice();
    const auto &physical_device_info = physical_device.info();
    dlog("Using physical device {}", physical_device_info.device_name);
    dlog("{}", physical_device_info);

    m_device = physical_device.createLogicalDevicePtr();
    dlog("Device successfully created");

    ilog("Render backend successfully initialized");

    initialize();
}

////////////////////////////////////////
////////////////////////////////////////
Engine::Engine(VkInstance instance,
               VkPhysicalDevice physical_device,
               VkDevice device,
               const window::Window &window) {
    m_instance        = std::make_unique<render::Instance>(instance);
    m_physical_device = std::make_unique<render::PhysicalDevice>(physical_device, *m_instance);
    m_device          = std::make_unique<render::Device>(device, *m_physical_device, *m_instance);

    ilog("Render backend successfully initialized");

    m_surface = m_instance->createWindowSurfacePtr(window, render::Surface::Buffering::Swapchain);

    initialize();
}

////////////////////////////////////////
////////////////////////////////////////
Engine::Engine(VkInstance instance,
               VkPhysicalDevice physical_device,
               VkDevice device,
               core::Extentu extent,
               render::Surface::Buffering buffering) {
    m_instance        = std::make_unique<render::Instance>(instance);
    m_physical_device = std::make_unique<render::PhysicalDevice>(physical_device, *m_instance);
    m_device          = std::make_unique<render::Device>(device, *m_physical_device, *m_instance);

    ilog("Render backend successfully initialized");

    m_surface = m_instance->createOffscreenSurfacePtr(extent, buffering);

    initialize();
}

////////////////////////////////////////
////////////////////////////////////////
Engine::~Engine() {
    m_surface->onSwapchainFenceSignaled = [](const auto &fence) {};

    m_device->waitIdle();

    m_deletion_queue->flush();
}

////////////////////////////////////////
////////////////////////////////////////
Engine::Engine(Engine &&other) noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
auto Engine::operator=(Engine &&other) noexcept -> Engine & = default;

////////////////////////////////////////
////////////////////////////////////////
auto Engine::update() -> void {
    m_profiler->newFrame();

    if (m_state_manager.hasPendingRequests()) {
        m_profiler->beginStage("State push/pop/set");

        m_state_manager.executeRequests();

        m_profiler->endStage("State push/pop/set");
    }

    m_profiler->beginStage("State update");

    m_state_manager.update(m_delta);

    m_profiler->endStage("State update");
}

////////////////////////////////////////
////////////////////////////////////////
void Engine::recreateSwapchain() {
    m_surface->recreate();
}

////////////////////////////////////////
////////////////////////////////////////
auto Engine::beginFrame() -> render::Surface::Frame & {
    namespace Chrono = std::chrono;

    STORMKIT_EXPECTS_MESSAGE(
        !m_is_frame_began,
        "Double call to beginFrame(), please call endFrame() before recalling beginFrame()");

    if (m_surface->needRecreate()) m_surface->recreate();

    m_profiler->beginStage("Frame acquire");
    m_frame.emplace(m_surface->acquireNextFrame());
    m_profiler->endStage("Frame acquire");

    const auto now = Clock::now();
    m_delta        = Chrono::duration_cast<core::Secondf>(now - m_last_tp);

    m_cpu_time = m_delta.count();
    m_last_tp  = now;

    m_is_frame_began = true;

    return *m_frame;
}

////////////////////////////////////////
////////////////////////////////////////
auto Engine::endFrame() -> void {
    STORMKIT_EXPECTS_MESSAGE(m_is_frame_began,
                             "Please call beginFrame() before calling endFrame()");

    m_profiler->beginStage("Frame present");
    m_surface->present(*m_frame);
    m_profiler->endStage("Frame present");

    m_is_frame_began = false;
}

////////////////////////////////////////
////////////////////////////////////////
auto Engine::immediateSubmit(const Engine::ImmediateSubmitCallback &callback,
                             bool on_transfert_queue) const -> ImmediateSubmitData {
    auto &queue = on_transfert_queue && m_device->hasAsyncTransfertQueue()
                      ? m_device->asyncTransfertQueue()
                      : m_device->graphicsQueue();

    auto data = ImmediateSubmitData { .cmb   = queue.createCommandBuffer(),
                                      .fence = m_device->createFence() };

    data.cmb.begin(true);
    callback(data.cmb);
    data.cmb.end();

    data.cmb.submit({}, {}, core::makeObserver(data.fence));

    return data;
}

////////////////////////////////////////
////////////////////////////////////////
auto Engine::initialize() -> void {
    m_surface->initialize(*m_device);
    m_last_tp = Clock::now();

    const auto &physical_device = m_device->physicalDevice();

    const auto &limits   = physical_device.capabilities().limits;
    const auto &features = physical_device.capabilities().features;

    auto count = limits.framebuffer_color_sample_counts;
    count &= limits.framebuffer_depth_sample_counts;

    if (features.sampler_anisotropy)
        m_max_anisotropy = m_device->physicalDevice().capabilities().limits.max_sampler_anisotropy;

    if ((count & render::SampleCountFlag::C2_BIT) == render::SampleCountFlag::C2_BIT)
        m_max_sample_count = render::SampleCountFlag::C2_BIT;
    if ((count & render::SampleCountFlag::C4_BIT) == render::SampleCountFlag::C4_BIT)
        m_max_sample_count = render::SampleCountFlag::C4_BIT;
    if ((count & render::SampleCountFlag::C8_BIT) == render::SampleCountFlag::C8_BIT)
        m_max_sample_count = render::SampleCountFlag::C8_BIT;
    if ((count & render::SampleCountFlag::C16_BIT) == render::SampleCountFlag::C16_BIT)
        m_max_sample_count = render::SampleCountFlag::C16_BIT;
    if ((count & render::SampleCountFlag::C32_BIT) == render::SampleCountFlag::C32_BIT)
        m_max_sample_count = render::SampleCountFlag::C32_BIT;
    if ((count & render::SampleCountFlag::C64_BIT) == render::SampleCountFlag::C64_BIT)
        m_max_sample_count = render::SampleCountFlag::C64_BIT;

    m_frame_graph    = FrameGraph { *this };
    m_resource_cache = ResourceCache { *this };
    m_profiler       = Profiler {};
    m_deletion_queue = DeletionQueue { *this };

    m_surface->onSwapchainFenceSignaled = [this](const render::Fence &fence) {
        m_deletion_queue->flush();
    };
}
