#include "MainState.hpp"
#include "Log.hpp"

/////////// - StormKit::entities - ///////////
#include <storm/entities/MessageBus.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Device.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Surface.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Keyboard.hpp>
#include <storm/window/Mouse.hpp>
#include <storm/window/Window.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>
#include <storm/engine/NameComponent.hpp>

#include <storm/engine/render/DrawableComponent.hpp>
#include <storm/engine/render/MaterialComponent.hpp>
#include <storm/engine/render/TransformComponent.hpp>

#include <storm/engine/render/3D/FPSCamera.hpp>
#include <storm/engine/render/3D/Model.hpp>
#include <storm/engine/render/3D/PbrMesh.hpp>
#include <storm/engine/render/3D/PbrRenderSystem.hpp>

using namespace storm;

static constexpr auto ROTATE_ANGLE = 5.f;

class RotationSystem: public entities::System {
  public:
    explicit RotationSystem(entities::EntityManager &manager)
        : System { manager, 1, { engine::TransformComponent::TYPE } } {}
    ~RotationSystem() override = default;

    RotationSystem(RotationSystem &&) noexcept = default;
    RotationSystem &operator=(RotationSystem &&) noexcept = default;

    void update(core::Secondf delta) override {
        for (auto e : m_entities) {
            auto &transform_component = m_manager->getComponent<engine::TransformComponent>(e);

            transform_component.transform.rotateRoll(ROTATE_ANGLE * delta.count());
        }
    }

  protected:
    void onMessageReceived(const entities::Message &message) override {}
};

////////////////////////////////////////
////////////////////////////////////////
MainState::MainState(core::StateManager &owner, engine::Engine &engine, window::Window &window)
    : State { owner, engine }, m_window { window },
      m_keyboard { window.createKeyboardPtr() }, m_mouse { window.createMousePtr() } {
    m_mouse->setPositionOnWindow(
        core::Position2i { window.size().width / 2, window.size().height / 2 });

    const auto extent = State::engine().surface().extent();

    m_render_system = &m_world.addSystem<engine::PbrRenderSystem>(State::engine());
    m_world.addSystem<RotationSystem>();

    m_camera = engine::FPSCamera::allocateOwned(extent);
    m_camera->setPosition({ 0.f, 0.f, -3.f });

    m_render_system->setCamera(*m_camera);

    m_model   = engine::Model::allocateOwned(engine, EXAMPLES_DATA_DIR "models/Sword.glb");
    auto mesh = m_model->createMesh();

    auto e               = m_world.makeEntity();
    auto &name_component = m_world.addComponent<engine::NameComponent>(e);
    name_component.name  = "MyMesh";

    auto &drawable_component    = m_world.addComponent<engine::DrawableComponent>(e);
    drawable_component.drawable = std::move(mesh);

    m_world.addComponent<engine::TransformComponent>(e);

    enableCamera();
}

////////////////////////////////////////
////////////////////////////////////////
MainState::~MainState() = default;

////////////////////////////////////////
////////////////////////////////////////
MainState::MainState(MainState &&) noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
MainState &MainState::operator=(MainState &&) noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
void MainState::update(core::Secondf delta) {
    if (m_camera_enabled) {
        auto inputs = engine::FPSCamera::Inputs {
            .up    = m_keyboard->isKeyPressed(window::Key::Z),
            .down  = m_keyboard->isKeyPressed(window::Key::S),
            .right = m_keyboard->isKeyPressed(window::Key::D),
            .left  = m_keyboard->isKeyPressed(window::Key::Q),
        };

        const auto extent   = core::Extenti { State::engine().surface().extent() };
        const auto position = [&inputs, &extent, this]() {
            auto position = m_mouse->getPositionOnWindow();

            if (position->x <= 5 || position->x > (extent.width - 5)) {
                position->x         = extent.width / 2;
                inputs.mouse_ignore = true;
            }
            if (position->y <= 5 || position->y > (extent.height - 5)) {
                position->y         = extent.height / 2;
                inputs.mouse_ignore = true;
            }

            m_mouse->setPositionOnWindow(position);

            return position;
        }();

        inputs.mouse_updated = true;
        inputs.x_mouse       = static_cast<float>(position->x);
        inputs.y_mouse       = static_cast<float>(position->y);

        m_camera->feedInputs(inputs);
        m_camera->update(delta);
    }

    auto &frame = engine().beginFrame();

    m_world.step(delta);
    m_render_system->render(frame);

    engine().endFrame();
}

void MainState::enableCamera() noexcept {
    m_window.get().lockMouse();
    m_camera_enabled = true;
}

void MainState::disableCamera() noexcept {
    m_window.get().unlockMouse();
    m_camera_enabled = false;
}
