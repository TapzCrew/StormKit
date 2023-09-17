// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - StormKit::window - ///////////
#import "macOSKeyboard.hpp"
#import "StormView.hpp"
#import "Utils.hpp"
#import "macOSWindow.hpp"

#include <storm/window/Event.hpp>
#include <storm/window/VideoSettings.hpp>
#include <storm/window/Window.hpp>

/////////// - SL - ///////////
#include <cmath>

/////////// - AppKit - ///////////
#import <AppKit/AppKit.h>

using namespace storm;
using namespace storm::window;
using namespace storm::window::details;

/////////////////////////////////////
/////////////////////////////////////
macOSKeyboard::macOSKeyboard(const AbstractWindow &window)
    : Keyboard { window }, m_macos_window { &static_cast<const macOSWindow &>(window) } {
}

/////////////////////////////////////
/////////////////////////////////////
macOSKeyboard::~macOSKeyboard() = default;

/////////////////////////////////////
/////////////////////////////////////
macOSKeyboard::macOSKeyboard(macOSKeyboard &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto macOSKeyboard::operator=(macOSKeyboard &&) noexcept -> macOSKeyboard & = default;

/////////////////////////////////////
/////////////////////////////////////
auto macOSKeyboard::isKeyPressed(Key key) const noexcept -> bool {
    initIOHID();

    auto elements = m_keys[static_cast<core::ArraySize>(key)];

    auto state = false;
    for (auto element = std::begin(elements); element != std::end(elements);) {
        auto value  = IOHIDValueRef { nullptr };
        auto device = IOHIDElementGetDevice(*element);

        IOHIDDeviceGetValue(device, *element, &value);

        if (!value) {
            CFRelease(*element);
            element = elements.erase(element);
        } else if (IOHIDValueGetIntegerValue(value) == 1) {
            state = true;
            break;
        } else
            ++element;
    }

    return state;
}

/////////////////////////////////////
/////////////////////////////////////
auto macOSKeyboard::setVirtualKeyboardVisible([[maybe_unused]] bool visible) noexcept -> void {
    // not supported
}

/////////////////////////////////////
/////////////////////////////////////
auto macOSKeyboard::initIOHID() -> void {
    static auto is_init = false;

    if (is_init) return;

    auto tis = TISCopyCurrentKeyboardLayoutInputSource();
    m_layout_data =
        static_cast<CFDataRef>(TISGetInputSourceProperty(tis, kTISPropertyUnicodeKeyLayoutData));

    CFRetain(m_layout_data);
    m_layout = const_cast<UCKeyboardLayout *>(
        reinterpret_cast<const UCKeyboardLayout *>(CFDataGetBytePtr(m_layout_data)));

    CFRelease(tis);

    m_manager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);

    IOHIDManagerOpen(m_manager, kIOHIDOptionsTypeNone);

    initializeKeyboard();

    is_init = true;
}

/////////////////////////////////////
/////////////////////////////////////
auto macOSKeyboard::initializeKeyboard() -> void {
    auto keyboards = copyDevices(kHIDPage_GenericDesktop, kHIDUsage_GD_Keyboard);

    auto keyboardCount = CFSetGetCount(keyboards);

    auto devices_array = std::vector<CFTypeRef> {};
    devices_array.resize(keyboardCount, nullptr);

    CFSetGetValues(keyboards, std::data(devices_array));

    for (auto i : core::range<CFIndex>(keyboardCount)) {
        IOHIDDeviceRef keyboard =
            reinterpret_cast<IOHIDDeviceRef>(const_cast<void *>(devices_array[i]));
        loadKeyboard(keyboard);
    }

    CFRelease(keyboards);
}

auto macOSKeyboard::loadKeyboard(IOHIDDeviceRef keyboard) -> void {
    auto keys = IOHIDDeviceCopyMatchingElements(keyboard, nullptr, kIOHIDOptionsTypeNone);

    auto keys_count = CFArrayGetCount(keys);

    for (auto i : core::range<CFIndex>(keys_count)) {
        IOHIDElementRef key =
            reinterpret_cast<IOHIDElementRef>(const_cast<void *>(CFArrayGetValueAtIndex(keys, i)));

        if (IOHIDElementGetUsagePage(key) != kHIDPage_KeyboardOrKeypad) continue;

        loadKey(key);
    }

    // Release unused stuff
    CFRelease(keys);
}

/////////////////////////////////////
/////////////////////////////////////
auto macOSKeyboard::loadKey(IOHIDElementRef key) -> void {
    auto usage_code   = IOHIDElementGetUsage(key);
    auto virtual_code = usageToVirtualCode(usage_code);

    if (virtual_code == 0xff) return;

    auto dead_key_state          = core::UInt32 { 0 };
    const auto max_string_length = UniCharCount { 4 };
    auto actual_string_length    = UniCharCount { 0 };

    auto unicode_string = std::vector<UniChar> {};
    unicode_string.resize(max_string_length);

    UCKeyTranslate(m_layout,
                   virtual_code,
                   kUCKeyActionDown,
                   0x100,
                   LMGetKbdType(),
                   kUCKeyTranslateNoDeadKeysBit,
                   &dead_key_state,
                   max_string_length,
                   &actual_string_length,
                   std::data(unicode_string));

    auto code = Key::Unknow;

    if (actual_string_length > 0) code = localizedKeyToStormKey(unicode_string[0]);

    if (code == Key::Unknow) code = nonLocalizedKeytoStormKey(virtual_code);

    if (code != Key::Unknow) {
        m_keys[static_cast<core::ArraySize>(code)].emplace_back(key);

        CFRetain(m_keys[static_cast<core::ArraySize>(code)].back());
    }
}

/////////////////////////////////////
/////////////////////////////////////
auto macOSKeyboard::copyDevicesMask(core::UInt32 page, core::UInt32 usage) -> CFDictionaryRef {
    auto dict = CFDictionaryCreateMutable(kCFAllocatorDefault,
                                          2,
                                          &kCFTypeDictionaryKeyCallBacks,
                                          &kCFTypeDictionaryValueCallBacks);

    auto value = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &page);
    CFDictionarySetValue(dict, CFSTR(kIOHIDDeviceUsagePageKey), value);
    CFRelease(value);

    value = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usage);
    CFDictionarySetValue(dict, CFSTR(kIOHIDDeviceUsageKey), value);
    CFRelease(value);

    return dict;
}

/////////////////////////////////////
/////////////////////////////////////
auto macOSKeyboard::copyDevices(core::UInt32 page, core::UInt32 usage) -> CFSetRef {
    auto mask = copyDevicesMask(page, usage);

    IOHIDManagerSetDeviceMatching(m_manager, mask);

    CFRelease(mask);
    mask = nullptr;

    auto devices = IOHIDManagerCopyDevices(m_manager);
    if (devices == nullptr) return nullptr;

    CFIndex deviceCount = CFSetGetCount(devices);
    if (deviceCount < 1) {
        CFRelease(devices);
        return nullptr;
    }

    return devices;
}
