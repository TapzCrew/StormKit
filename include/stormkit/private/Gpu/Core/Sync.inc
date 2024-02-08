export namespace stormkit::gpu {
    class STORMKIT_API Fence: public DeviceObject {
        struct Tag {};

      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Fence;

        enum class Status {
            Signaled,
            Unsignaled
        };

        Fence(const Device& device, bool signaled, Tag);
        ~Fence();

        Fence(const Fence&)                    = delete;
        auto operator=(const Fence&) -> Fence& = delete;

        Fence(Fence&&) noexcept;
        auto operator=(Fence&&) noexcept -> Fence&;

        [[nodiscard]] static auto create(const Device& device,
                                         bool signaled = false) noexcept -> Expected<Fence>;
        [[nodiscard]] static auto createSignaled(const Device& device) noexcept -> Expected<Fence>;
        [[nodiscard]] static auto allocate(const Device& device, bool signaled = false) noexcept
            -> Expected<std::unique_ptr<Fence>>;
        [[nodiscard]] static auto
            allocateSignaled(const Device& device) noexcept -> Expected<std::unique_ptr<Fence>>;

        [[nodiscard]] auto wait(const std::chrono::milliseconds& wait_for =
                                    std::chrono::milliseconds::max()) const -> Expected<Result>;
        auto               reset() -> void;

        [[nodiscard]] auto status() const noexcept -> Expected<Status>;

        [[nodiscard]] auto vkHandle() const noexcept -> const vk::raii::Fence&;

      private:
        core::DeferInit<vk::raii::Fence> m_vk_fence;
    };

    class STORMKIT_API Semaphore: public DeviceObject {
        struct Tag {};

      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Semaphore;

        Semaphore(const Device& device, Tag);
        ~Semaphore();

        Semaphore(const Semaphore&)                    = delete;
        auto operator=(const Semaphore&) -> Semaphore& = delete;

        Semaphore(Semaphore&&) noexcept;
        auto operator=(Semaphore&&) noexcept -> Semaphore&;

        [[nodiscard]] auto create(const Device& device) noexcept -> Expected<Semaphore>;
        [[nodiscard]] auto
            allocate(const Device& device) noexcept -> Expected<std::unique_ptr<Semaphore>>;

        [[nodiscard]] auto vkHandle() const noexcept -> const vk::raii::Semaphore&;

      private:
        core::DeferInit<vk::raii::Semaphore> m_vk_semaphore;
    };
} // namespace stormkit::gpu

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE Fence::Fence(const Device& device, bool signaled, Tag)
        : DeviceObject { device } {
        const auto flags =
            (signaled) ? vk::FenceCreateFlagBits::eSignaled : vk::FenceCreateFlags {};

        const auto create_info = vk::FenceCreateInfo {}.setFlags(flags);

        this->device()
            .vkHandle()
            .createFence(create_info)
            .transform(core::monadic::set(m_vk_fence))
            .transform_error(
                core::monadic::map(core::monadic::narrow<Result>(), core::throwError()));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE Fence::~Fence() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE Fence::Fence(Fence&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto Fence::operator=(Fence&& other) noexcept -> Fence& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto Fence::status() const noexcept -> Expected<Status> {
        return vkCall(*m_vk_fence,
                      &vk::raii::Fence::getStatus,
                      { { vk::Result::eSuccess, vk::Result::eNotReady } })
            .transform([](auto&& result) noexcept {
                if (result == vk::Result::eNotReady) return Status::Unsignaled;
                return Status::Signaled;
            })
            .transform_error(core::monadic::narrow<Result>());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto Fence::create(const Device& device,
                                             bool signaled) noexcept -> Expected<Fence> try {
        return Fence { device, signaled, Tag {} };
    } catch (const Result& result) { return std::unexpected(result); }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto
        Fence::createSignaled(const Device& device) noexcept -> Expected<Fence> {
        return create(device, true);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto Fence::allocate(const Device& device, bool signaled) noexcept
        -> Expected<std::unique_ptr<Fence>> try {
        return std::make_unique<Fence>(device, signaled, Tag {});
    } catch (const Result& result) { return std::unexpected(result); }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto
        Fence::allocateSignaled(const Device& device) noexcept -> Expected<std::unique_ptr<Fence>> {
        return allocate(device, true);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto
        Fence::wait(const std::chrono::milliseconds& wait_for) const -> Expected<Result> {
        return device().waitForFence(*this, wait_for);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto Fence::reset() -> void {
        device().resetFence(*this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto Fence::vkHandle() const noexcept -> const vk::raii::Fence& {
        return m_vk_fence.get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE Semaphore::Semaphore(const Device& device, Tag)
        : DeviceObject { device } {
        const auto create_info = vk::SemaphoreCreateInfo {};

        this->device()
            .vkHandle()
            .createSemaphore(create_info)
            .transform(core::monadic::set(m_vk_semaphore))
            .transform_error(
                core::monadic::map(core::monadic::narrow<Result>(), core::throwError()));
    };

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE Semaphore::~Semaphore() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE Semaphore::Semaphore(Semaphore&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto
        Semaphore::operator=(Semaphore&& other) noexcept -> Semaphore& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto
        Semaphore::create(const Device& device) noexcept -> Expected<Semaphore> try {
        return Semaphore { device, Tag {} };
    } catch (const Result& result) { return std::unexpected(result); }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto Semaphore::allocate(const Device& device) noexcept
        -> Expected<std::unique_ptr<Semaphore>> try {
        return std::make_unique<Semaphore>(device, Tag {});
    } catch (const Result& result) { return std::unexpected(result); }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto Semaphore::vkHandle() const noexcept -> const vk::raii::Semaphore& {
        return m_vk_semaphore.get();
    }
} // namespace stormkit::gpu
