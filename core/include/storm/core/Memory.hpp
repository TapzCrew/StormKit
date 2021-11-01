// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <array>
#include <memory>
#include <optional>
#include <vector>

/////////// - StormKit::core - ///////////
#include <storm/core/Platform.hpp>
#include <storm/core/Ranges.hpp>
#include <storm/core/Span.hpp>
#include <storm/core/Types.hpp>

namespace storm::core {
    /// \brief Create an observer ptr from a pointer value.
    /// \requires `T` shall to be a raw or a smart pointer.
    /// \param ptr The pointer to observe.
    /// \returns An observer ptr.
    template<POINTER_CONCEPT(T)>
    constexpr auto makeObserver(T &ptr) noexcept;

    /// \brief Create an observer ptr from a non-pointer value.
    /// \requires `T` shall not to be a raw or a smart pointer.
    /// \param value The value to observe.
    /// \returns An observer ptr.
    template<NON_POINTER_CONCEPT(T)>
    constexpr auto makeObserver(T &value) noexcept;

    /// \brief Create a const observer ptr from a pointer value.
    /// \requires `T` shall to be a raw or a smart pointer.
    /// \param ptr The pointer to observe.
    /// \returns A const observer ptr.
    template<POINTER_CONCEPT(T)>
    constexpr auto makeConstObserver(const T &ptr) noexcept;

    /// \brief Create a const observer ptr from a non-pointer value.
    /// \requires `T` shall not to be a raw or a smart pointer.
    /// \param value The value to observe.
    /// \returns A const observer ptr.
    template<NON_POINTER_CONCEPT(T)>
    constexpr auto makeConstObserver(const T &value) noexcept;

    /// TODO FIX DOC
    /// \brief Create an  from a pointer value.
    /// \requires `T` shall to be a raw or a smart pointer.
    /// \param ptr The pointer to observe.
    /// \returns An observer ptr.
    template<POINTER_CONCEPT(T)>
    constexpr auto makeRef(T &ptr) noexcept;

    /// TODO FIX DOC
    /// \brief Create an observer ptr from a non-pointer value.
    /// \requires `T` shall not to be a raw or a smart pointer.
    /// \param value The value to observe.
    /// \returns An observer ptr.
    template<NON_POINTER_CONCEPT(T)>
    constexpr auto makeRef(T &value) noexcept;

    /// TODO FIX DOC
    /// \brief Create a const observer ptr from a pointer value.
    /// \requires `T` shall to be a raw or a smart pointer.
    /// \param ptr The pointer to observe.
    /// \returns A const observer ptr.
    template<POINTER_CONCEPT(T)>
    constexpr auto makeConstRef(const T &ptr) noexcept;

    /// TODO FIX DOC
    /// \brief Create a const observer ptr from a non-pointer value.
    /// \requires `T` shall not to be a raw or a smart pointer.
    /// \param value The value to observe.
    /// \returns A const observer ptr.
    template<NON_POINTER_CONCEPT(T)>
    constexpr auto makeConstRef(const T &value) noexcept;

    /// \brief Create a static array of observer ptr.
    ///
    /// Observer pointers are created with `makeObserver`.
    ///
    /// \param args The list of values to observe.
    /// \returns A static array of observer ptr.
    template<typename... Args>
    constexpr auto makeObserverStaticArray(Args &&...args) noexcept;

    /// \brief Create a static array of const observer ptr.
    ///
    /// Observer pointers are created with `makeConstObserver`.
    ///
    /// \param args The list of values to observe.
    /// \returns A static array of const observer ptr.
    template<typename... Args>
    constexpr auto makeConstObserverStaticArray(Args &&...args) noexcept;

    /// \brief Create a dynamic array of observer ptr.
    ///
    /// Observer pointers are created with `makeObserver`.
    ///
    /// \param args The list of values to observe.
    /// \returns A dynamic array of observer ptr.
    template<typename... Args>
    auto makeObserverArray(Args &&...args) noexcept;

    /// \brief Create a dynamic array of const observer ptr.
    ///
    /// Observer pointers are created with `makeConstObserver`.
    ///
    /// \param args The list of values to observe.
    /// \returns A dynamic array of const observer ptr.
    template<typename... Args>
    auto makeConstObserverArray(Args &&...args) noexcept;

    /// \brief Create a static array of reference_wrapper.
    ///
    /// References are created with `std::ref`.
    ///
    /// \param args The list of values to observe.
    /// \returns A static array of reference_wrapper
    template<typename... Args>
    auto makeRefStaticArray(Args &&...args) noexcept;

    /// \brief Create a dynamic array of reference_wrapper.
    ///
    /// References are created with `std::ref`.
    ///
    /// \param args The list of values to observe.
    /// \returns A dynamic array of reference_wrapper
    template<typename... Args>
    auto makeRefArray(Args &&...args) noexcept;

    /// \brief Create a dynamic array of const reference_wrapper.
    ///
    /// References are created with `std::cref`.
    ///
    /// \param args The list of values to observe.
    /// \returns A dynamic array of const reference_wrapper.
    template<typename... Args>
    auto makeConstRefStaticArray(Args &&...args) noexcept;

    /// \brief Create a dynamic array of const reference_wrapper.
    ///
    /// References are created with `std::cref`.
    ///
    /// \param args The list of values to observe.
    /// \returns A dynamic array of const reference_wrapper.
    template<typename... Args>
    auto makeConstRefArray(Args &&...args) noexcept;

    /// \brief Convert an array of data to an array of observer ptr of data.
    ///
    /// Observers ptr are created with makeObserverPtr.
    ///
    /// \param container The array to convert.
    /// \returns A dynamic array of observer ptrs.
    template<typename Container>
    auto toObserverArray(Container &&container) noexcept;

    /// \brief Convert an array of const data to an array of const observer ptr of data.
    ///
    /// Observers ptr are created with makeConstObserverPtr.
    ///
    /// \param container The array to convert.
    /// \returns A dynamic array of const observer ptrs.
    template<typename Container>
    auto toConstObserverArray(Container &&container);

    /// \brief Convert an array of data to an array of reference_wrapper of data.
    ///
    /// Reference_wrappers are created with std::ref.
    ///
    /// \param container The array to convert.
    /// \returns A dynamic array of reference_wrapper.
    template<typename Container>
    auto toRefArray(Container &&container);

    /// \brief Convert an array of data to an array of const reference_wrapper of data.
    ///
    /// Reference_wrappers are created with std::ref.
    ///
    /// \param container The array to convert.
    /// \returns A dynamic array of const reference_wrapper.
    template<typename Container>
    auto toConstRefArray(Container &&container);

    /// \brief Create a static array of `T`
    /// \param first The first value, the array will be an array of this type
    /// \param args The list of values
    /// \requires all values need to be the same type as `T`
    /// \return A static array of T
    template<typename T, SAME_TYPE_CONCEPT(... Args, T)>
    constexpr auto makeStaticArray(T &&first, Args &&...args) noexcept;

    /// \brief Create a dynamic array of `T`
    /// \param first The first value, the array will be an array of this type
    /// \param args The list of values
    /// \requires all values need to be the same type as `T`
    /// \return A dynamic array of T
    template<typename T, SAME_TYPE_CONCEPT(... Args, T)>
    auto makeArray(T &&first, Args &&...args) noexcept;

    /// \brief Utility to defer allocation on stack of a class member
    template<typename T>
    using DeferredAlloc = std::optional<T>;
} // namespace storm::core

#if defined(_MSC_VER) && _MSC_VER >= 1910 && !defined(STORMKIT_GEN_DOC)
    #define STORMKIT_EBCO __declspec(empty_bases)
#else
  /// \brief Enable empty base optimisation on MSVC
    #define STORMKIT_EBCO
#endif

#define ALLOCATE_HELPERS(T)                                          \
    template<typename... Args>                                       \
    [[nodiscard]] static inline auto allocateOwned(Args &&...args) { \
        return std::make_unique<T>(std::forward<Args>(args)...);     \
    }

/// \exclude
#define DECLARE_ARRAYS_(x)                       \
    using x##Array = std::vector<x>;             \
    template<storm::core::ArraySize S>           \
    using x##StaticArray = std::array<x, S>;     \
    using x##Span        = storm::core::span<x>; \
    using x##ConstSpan   = storm::core::span<const x>;

/// \exclude
#define DECLARE_PTR_AND_REF_(x)                                \
    using x##Ptr            = x *;                             \
    using x##ConstPtr       = const x *;                       \
    using x##OwnedPtr       = std::unique_ptr<x>;              \
    using x##SharedPtr      = std::shared_ptr<x>;              \
    using x##ConstSharedPtr = std::shared_ptr<const x>;        \
    using x##WeakPtr        = std::weak_ptr<x>;                \
    using x##ConstWeakPtr   = std::weak_ptr<const x>;          \
    using x##Ref            = std::reference_wrapper<x>;       \
    using x##ConstRef       = std::reference_wrapper<const x>; \
    using x##Opt            = std::optional<x>;                \
    DECLARE_ARRAYS_(x)                                         \
    DECLARE_ARRAYS_(x##Ptr)                                    \
    DECLARE_ARRAYS_(x##ConstPtr)                               \
    DECLARE_ARRAYS_(x##OwnedPtr)                               \
    DECLARE_ARRAYS_(x##SharedPtr)                              \
    DECLARE_ARRAYS_(x##WeakPtr)                                \
    DECLARE_ARRAYS_(x##Ref)                                    \
    DECLARE_ARRAYS_(x##ConstRef)                               \
    DECLARE_ARRAYS_(x##Opt)

/// \brief Declare pointer and reference_wrapper aliases to a type
/// \param type The type to declare aliases
#define DECLARE_PTR_AND_REF(type) DECLARE_PTR_AND_REF_(type)

/// \brief Declare array aliases to a type
/// \param type The type to declare aliases
#define DECLARE_ARRAYS(type) DECLARE_ARRAYS_(type)

/// \brief Add padding to a struct or a class
/// \param size The size of the padding
#define PADDING(size) storm::core::Byte private____padding[size];

#if defined(STORMKIT_COMPILER_MSVC) && !defined(STORMKIT_GEN_DOC)
    #define BEGIN_PACKED_STRUCT_A ___pragma(pack(push, 1))
    #define BEGIN_PACKED_STRUCT(x) BEGIN_PACKED_STRUCT_A struct x
    #define END_PACKET_STRUCT ___pragma(pack(pop))
#elif (defined(STORMKIT_COMPILER_CLANG) || defined(STORMKIT_COMPILER_GCC)) && \
    !defined(STORMKIT_GEN_DOC)
    #define BEGIN_PACKED_STRUCT_A __attribute__((__packed__))
    #define BEGIN_PACKED_STRUCT(type) struct BEGIN_PACKED_STRUCT_A type
    #define END_PACKED_STRUCT
#else
    #warning Packed struct is not avalaible
/// \exclude
    #define BEGIN_PACKED_STRUCT_A

    /// \exclude
    #define BEGIN_PACKED_STRUCT(x) BEGIN_PACKED_STRUCT_A struct x

    /// \exclude
    #define END_PACKED_STRUCT
#endif

/// \exclude
#define STORMKIT_UNUSED(x) (void)(x);

#define STORMKIT_RAII_CAPSULE(name, x, deleter)                  \
    struct name##Deleter {                                       \
        void operator()(x *ptr) const noexcept { deleter(ptr); } \
    };                                                           \
    using name##Scoped = std::unique_ptr<x, name##Deleter>;
#define STORMKIT_RAII_CAPSULE_PP(name, x, deleter)                  \
    struct name##Deleter {                                       \
        void operator()(x *ptr) const noexcept { deleter(&ptr); } \
    };                                                           \
    using name##Scoped = std::unique_ptr<x, name##Deleter>;

#include "Memory.inl"
