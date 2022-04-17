/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/render/Transform.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
Transform::Transform() noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
Transform::~Transform() noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
Transform::Transform(const Transform &) noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
auto Transform::operator=(const Transform &) noexcept -> Transform & = default;

////////////////////////////////////////
////////////////////////////////////////
Transform::Transform(Transform &&) noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
auto Transform::operator=(Transform &&) noexcept -> Transform & = default;

////////////////////////////////////////
////////////////////////////////////////
auto Transform::recomputeOrientation() const noexcept -> void {
    const auto pitch =
        core::angleAxis(core::radians(m_orientation_euler.x), core::Vector3f { 1.f, 0.f, 0.f });
    const auto yaw =
        core::angleAxis(core::radians(m_orientation_euler.y), core::Vector3f { 0.f, 1.f, 0.f });
    const auto roll =
        core::angleAxis(core::radians(m_orientation_euler.z), core::Vector3f { 0.f, 0.f, 1.f });

    m_orientation = roll * pitch * yaw;
}

////////////////////////////////////////
////////////////////////////////////////
auto Transform::recomputeMatrices() const noexcept -> void {
    const auto &quat = orientation();

    const auto translation_matrix = core::translate(core::Matrix { 1.f }, m_position);
    const auto rotation_matrix    = core::Matrix { quat };
    const auto scale_matrix       = core::scale(core::Matrix { 1.f }, m_scale);

    m_data.transform     = scale_matrix * rotation_matrix * translation_matrix;
    m_data.inv_transform = core::inverse(m_data.inv_transform);
}

////////////////////////////////////////
////////////////////////////////////////
auto Transform::extract(const core::Matrix &matrix) noexcept -> void {
    m_position = core::Vector3f { core::row(matrix, 0)[3],
                                  core::row(matrix, 1)[3],
                                  core::row(matrix, 2)[3] };

    auto sx = core::length(core::row(matrix, 0));
    auto sy = core::length(core::row(matrix, 1));
    auto sz = core::length(core::row(matrix, 2));

    auto det = core::determinant(matrix);
    if (det < 0) sx = -sx;

    const auto inv_sx = 1.f / sx;
    const auto inv_sy = 1.f / sy;
    const auto inv_sz = 1.f / sz;

    auto _m = matrix;
    core::row(_m, 0)[0] *= inv_sx;
    core::row(_m, 0)[1] *= inv_sx;
    core::row(_m, 0)[2] *= inv_sx;

    core::row(_m, 1)[0] *= inv_sy;
    core::row(_m, 1)[1] *= inv_sy;
    core::row(_m, 1)[2] *= inv_sy;

    core::row(_m, 2)[0] *= inv_sz;
    core::row(_m, 2)[1] *= inv_sz;
    core::row(_m, 2)[2] *= inv_sz;

    const auto euler = core::eulerAngles(core::Quaternion { _m });

    m_scale.x = sx;
    m_scale.y = sx;
    m_scale.z = sx;

    m_orientation_euler.x = core::degrees(euler.x);
    m_orientation_euler.y = core::degrees(euler.y);
    m_orientation_euler.z = core::degrees(euler.z);
}
