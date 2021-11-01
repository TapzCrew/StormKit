/////////// - StormKit::engine - ///////////
#include <storm/engine/render/3D/FPSCamera.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
FPSCamera::FPSCamera(core::Extentf viewport, float znear, float zfar) noexcept
    : Camera { Camera::Type::Perspective, std::move(viewport), znear, zfar } {
}

////////////////////////////////////////
////////////////////////////////////////
FPSCamera::~FPSCamera() noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
FPSCamera::FPSCamera(FPSCamera &&) noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
auto FPSCamera::operator=(FPSCamera &&) noexcept -> FPSCamera & = default;

////////////////////////////////////////
////////////////////////////////////////
auto FPSCamera::update(core::Secondf delta) noexcept -> void {
    static auto first_iteration = true;

    using storm::core::cos;
    using storm::core::radians;
    using storm::core::sin;

    const auto need_update =
        m_inputs.up || m_inputs.down || m_inputs.right || m_inputs.left || m_inputs.mouse_updated;
    if (!need_update && !first_iteration) return;

    if (first_iteration || m_inputs.mouse_ignore) {
        m_last_x_mouse = m_inputs.x_mouse;
        m_last_y_mouse = m_inputs.y_mouse;
    }

    const auto rotation_speed = delta.count() * m_rotation_speed;
    if ((m_inputs.mouse_updated || first_iteration) && !m_inputs.mouse_ignore) {
        const auto dx = m_last_x_mouse - m_inputs.x_mouse;
        const auto dy = m_last_y_mouse - m_inputs.y_mouse;

        m_orientation.x += -dy * rotation_speed.y;
        m_orientation.y += -dx * rotation_speed.x;
    }

    auto front = core::Vector3f {};
    front.x    = -cos(radians(m_orientation.x)) * sin(radians(m_orientation.y));
    front.y    = sin(radians(m_orientation.x));
    front.z    = cos(radians(m_orientation.x)) * cos(radians(m_orientation.y));
    m_front    = core::normalize(front);

    const auto move_speed = delta.count() * m_move_speed;
    if (m_inputs.up) m_position += front * move_speed;
    if (m_inputs.down) m_position -= front * move_speed;
    if (m_inputs.left) m_position -= core::normalize(core::cross(m_front, m_up)) * move_speed;
    if (m_inputs.right) m_position += core::normalize(core::cross(m_front, m_up)) * move_speed;

    const auto rotation_matrix = [this]() {
        auto mat = core::Matrix { 1.f };

        mat = core::rotate(mat, core::radians(m_orientation.x), core::vec3(-1.f, 0.f, 0.f));
        mat = core::rotate(mat, core::radians(m_orientation.y), core::vec3(0.f, 1.f, 0.f));
        mat = core::rotate(mat, core::radians(m_orientation.z), core::vec3(0.f, 0.f, 1.f));

        return mat;
    }();
    auto position = m_position;
    position.y    = -position.y;

    const auto translation_matrix = core::translate(core::Matrix { 1.f }, position);

    m_data.position = core::Vector4f { position, 1.f };
    m_data.view     = rotation_matrix * translation_matrix;

    m_last_x_mouse = m_inputs.x_mouse;
    m_last_y_mouse = m_inputs.y_mouse;

    first_iteration = false;

    m_inputs = {};

    m_dirty = true;
}
