/////////// - imgui - ///////////
#include <iterator>

/////////// - imgui - ///////////
#include "../imgui/imgui.hpp"
#include "../imgui/imgui_impl_vulkan.hpp"
#include "../imgui/imgui_widget_flamegraph.hpp"

/////////// - StormKit::core - ///////////
#include <storm/core/Numerics.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Keyboard.hpp>
#include <storm/window/Mouse.hpp>
#include <storm/window/Window.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Instance.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Queue.hpp>

#include <storm/render/pipeline/DescriptorPool.hpp>
#include <storm/render/pipeline/PipelineCache.hpp>

#include <storm/render/sync/Fence.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>
#include <storm/engine/Profiler.hpp>

#include <storm/engine/render/utils/DebugGUI.hpp>

using namespace storm;
using namespace storm::engine;

auto profilerGetValue(float *start,
                      float *end,
                      ImU8 *level,
                      const char **caption,
                      const auto *data,
                      int idx) -> void {
    namespace Chrono = std::chrono;

    auto entry = reinterpret_cast<const Profiler::Entry *>(data);
    auto it    = std::cbegin(entry->scopes);
    if (it == std::cend(entry->scopes)) return;

    std::advance(it, idx);

    const auto &[name, scope] = *it;

    if (start) {
        const auto time =
            Chrono::duration<float, std::milli> { scope.start - entry->start }.count();
        *start = time;
    }

    if (end) {
        const auto time = Chrono::duration<float, std::milli> { scope.end - entry->start }.count();
        *end            = time;
    }

    if (level) *level = scope.level;

    if (caption) *caption = std::data(name);
}

/////////////////////////////////////
/////////////////////////////////////
DebugGUI::DebugGUI(Engine &engine, window::Keyboard &keyboard, window::Mouse &mouse)
    : m_engine { engine }, m_keyboard { keyboard }, m_mouse { mouse } {
    const auto extent = core::Extentf { engine.surface().extent() };

    ImGui::CreateContext();
    auto &io         = ImGui::GetIO();
    io.DisplaySize.x = extent.width;
    io.DisplaySize.y = extent.height;
    io.IniFilename   = nullptr;
    io.Fonts->AddFontDefault();
    io.Fonts->Build();

    ImGui::StyleColorsDark();

    std::fill(core::ranges::begin(m_frame_times), core::ranges::end(m_frame_times), 0.f);
}

/////////////////////////////////////
/////////////////////////////////////
DebugGUI::~DebugGUI() {
    ImGui_ImplVulkan_Shutdown();

    ImGui::DestroyContext();
}

/////////////////////////////////////
/////////////////////////////////////
DebugGUI::DebugGUI(DebugGUI &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto DebugGUI::operator=(DebugGUI &&) noexcept -> DebugGUI & = default;

/////////////////////////////////////
/////////////////////////////////////
auto check_vk_result([[maybe_unused]] VkResult err) {
}

/////////////////////////////////////
/////////////////////////////////////
auto DebugGUI::init(const render::RenderPass &render_pass) -> void {
    const auto &device = engine().device();
    // auto &pipeline_cache        = m_engine->pipelineCache();
    // const auto &descriptor_pool = m_engine->descriptorPool();
    const auto &queue = device.graphicsQueue();
    m_buffer_count    = engine().surface().bufferingCount();

    /*auto init_info =
        ImGui_ImplVulkan_InitInfo { .Instance        = instance.vkHandle(),
                                    .PhysicalDevice  = device.physicalDevice().vkHandle(),
                                    .Device          = device.vkHandle(),
                                    .Dispatcher      = device.vkDispatcher(),
                                    .QueueFamily     = 0,
                                    .Queue           = queue.vkHandle(),
                                    .PipelineCache   = pipeline_cache.vkHandle(),
                                    .DescriptorPool  = descriptor_pool.vkHandle(),
                                    .MinImageCount   = m_buffer_count,
                                    .ImageCount      = m_buffer_count,
                                    .Allocator       = VK_NULL_HANDLE,
                                    .CheckVkResultFn = check_vk_result };*/

    m_sub_command_buffers =
        queue.createCommandBufferPtrs(m_buffer_count, storm::render::CommandBufferLevel::Secondary);

    // ImGui_ImplVulkan_Init(&init_info, render_pass.vkHandle());

    auto fence  = device.createFence();
    auto cmb    = queue.createCommandBuffer();
    auto cmb_vk = cmb.vkHandle();

    const auto begin_info =
        vk::CommandBufferBeginInfo {}.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    [[maybe_unused]] auto _ = cmb_vk.begin(begin_info, device.vkDispatcher());

    ImGui_ImplVulkan_CreateFontsTexture(cmb_vk);

    _ = cmb_vk.end(device.vkDispatcher());

    queue.submit(storm::core::makeConstObserverStaticArray(cmb),
                 {},
                 {},
                 storm::core::makeObserver(fence));

    fence.wait();

    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

/////////////////////////////////////
/////////////////////////////////////
auto DebugGUI::update() -> void {
    ImGui_ImplVulkan_NewFrame();

    m_current_cpu_time = engine().getCPUTime();

    ImGui::NewFrame();

    auto &io     = ImGui::GetIO();
    io.DeltaTime = m_current_cpu_time;

    auto &mouse = m_mouse.get();

    io.MouseDown[0] = mouse.isButtonPressed(window::MouseButton::Left);
    io.MouseDown[1] = mouse.isButtonPressed(window::MouseButton::Right);
    io.MouseDown[2] = mouse.isButtonPressed(window::MouseButton::Middle);

    const auto position = mouse.getPositionOnWindow();

    io.MousePos = { static_cast<float>(position->x), static_cast<float>(position->y) };

    const auto cpu_time = (core::realIsEqual(m_current_cpu_time, 0.f)) ? 1.f : m_current_cpu_time;
    m_frame_times[m_frame_time_pointer] = 1.f / cpu_time;
}

/////////////////////////////////////
/////////////////////////////////////
auto DebugGUI::render(storm::render::CommandBuffer &cmb, const render::RenderPass &render_pass)
    -> void {
    namespace Chrono = std::chrono;

    const auto &profiler = engine().profiler();

    auto index = profiler.currentEntryIndex();

    if (m_skip_frame > 0u) {
        if (m_skip_frame_counter >= m_skip_frame) {
            m_last_entry_index   = index;
            m_skip_frame_counter = 0u;
        } else {
            m_skip_frame_counter++;
            index = m_last_entry_index;
        }
    }

    const auto &current_entry = profiler.entries()[index];
    const auto extent         = core::Extentf { engine().surface().extent() };

    const auto &device = engine().device();
    m_current_buffer++;
    if (m_current_buffer >= m_buffer_count) m_current_buffer = 0;

    bool foo = true;
    ImGui::Begin("Debug window",
                 &foo,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
    ImGui::SetWindowPos("Debug window", { 0, 0 });

    ImGui::BeginGroup();

    for (auto i : m_frame_times) m_max_fps = std::max(m_max_fps, static_cast<core::UInt32>(i));

    const auto title = core::format("{}\n\n\n0", m_max_fps);
    ImGui::PlotHistogram(title.c_str(),
                         std::data(m_frame_times),
                         std::size(m_frame_times),
                         0,
                         "Frame times",
                         0,
                         static_cast<float>(m_max_fps),
                         { extent.width, 120.f });
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);

    static constexpr auto max = std::numeric_limits<float>::max();
    ImGuiWidgetFlameGraph::PlotFlame("CPU",
                                     &profilerGetValue,
                                     &current_entry,
                                     std::size(current_entry.scopes),
                                     0,
                                     "Main Thread",
                                     max,
                                     max,
                                     { extent.width, 120.f });
    ImGui::EndGroup();

    ImGui::BeginGroup();
    if (ImGui::CollapsingHeader("Detail CPU")) {
        const auto full_frame =
            Chrono::duration<float, std::milli> { current_entry.end - current_entry.start }.count();
        ImGui::Text("Full Frame: %.3f ms", full_frame);

        for (const auto &[name, scope] : current_entry.scopes) {
            const auto time =
                Chrono::duration<float, std::milli> { scope.end - scope.start }.count();
            auto str = std::string { "%s: %.3f ms" };
            for (auto i = 0u; i < scope.level; ++i) str.insert(0, "   ");

            ImGui::Text(std::data(str), std::data(name), time);
        }
    }
    ImGui::EndGroup();

    ImGui::End();
    ImGui::Render();

    auto &sub_cmb = *m_sub_command_buffers[m_current_buffer];
    sub_cmb.reset();

    auto sub_cmb_vk = sub_cmb.vkHandle();

    const auto inheritance_info =
        vk::CommandBufferInheritanceInfo {}.setRenderPass(render_pass.vkHandle()).setSubpass(0);

    const auto begin_info = vk::CommandBufferBeginInfo {}
                                .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit |
                                          vk::CommandBufferUsageFlagBits::eRenderPassContinue)
                                .setPInheritanceInfo(&inheritance_info);
    [[maybe_unused]] auto _ = sub_cmb_vk.begin(begin_info, device.vkDispatcher());

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), sub_cmb_vk);

    _ = sub_cmb_vk.end(device.vkDispatcher());

    cmb.executeSubCommandBuffers({ sub_cmb });

    m_frame_time_pointer++;
    if (m_frame_time_pointer >= std::size(m_frame_times)) m_frame_time_pointer = 0u;
}
