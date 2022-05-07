/////////// - StormKit::engine - ///////////
#include <storm/engine/render/Camera.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
Camera::Camera(Type type, core::Extentf viewport, float znear, float zfar) noexcept
    : m_type { type }, m_viewport { std::move(viewport) } {
    if (m_type == Type::Perspective)
        m_data.projection = core::perspective(m_fov, viewport.width / viewport.height, znear, zfar);
    else if (m_type == Type::Orthographic)
        m_data.projection = core::ortho(0.f, viewport.width, 0.f, viewport.height, znear, zfar);
}

////////////////////////////////////////
////////////////////////////////////////
Camera::~Camera() noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
Camera::Camera(const Camera &) noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
auto Camera::operator=(const Camera &) noexcept -> Camera & = default;

////////////////////////////////////////
////////////////////////////////////////
Camera::Camera(Camera &&) noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
auto Camera::operator=(Camera &&) noexcept -> Camera & = default;

////////////////////////////////////////
////////////////////////////////////////
auto Camera::update(core::Secondf delta) noexcept -> void {
}
