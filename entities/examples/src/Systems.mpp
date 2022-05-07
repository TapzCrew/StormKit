#pragma once

#include <stormkit/image/Fwd.hpp>

#include <stormkit/entities/System.mpp>

#include "Constants.mpp"

class Renderer;
class UpdateBoardSystem final: public stormkit::entities::System {
  public:
    UpdateBoardSystem(stormkit::image::Image &board,
                      Renderer &renderer,
                      stormkit::entities::EntityManager &manager);
    ~UpdateBoardSystem() override;

    UpdateBoardSystem(UpdateBoardSystem &&) noexcept;
    auto operator=(UpdateBoardSystem &&) noexcept -> UpdateBoardSystem &;

    auto update(stormkit::core::Secondf delta) -> void override;
    auto postUpdate() -> void override;

    auto setEditModeEnabled(bool enabled) noexcept { m_is_on_edit_mode = enabled; }
    auto incrementDelta(stormkit::core::Secondf delta) { m_refresh_board_delta += delta; }

  private:
    using Clock = std::chrono::high_resolution_clock;

    auto onMessageReceived(const stormkit::entities::Message &message) -> void override {};

    bool m_is_on_edit_mode          = true;
    stormkit::image::Image *m_board = nullptr;
    Renderer *m_renderer            = nullptr;

    Clock::time_point m_last_update;

    bool m_updated = true;

    stormkit::core::Secondf m_refresh_board_delta = REFRESH_BOARD_DELTA;
};
