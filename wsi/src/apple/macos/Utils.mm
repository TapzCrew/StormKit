// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - StormKit::window - ///////////
#include "Utils.hpp"

/////////// - STL - ///////////
#include <iostream>

/////////// - AppKit - ///////////
#import <AppKit/NSEvent.h>

/////////// - Carbon - ///////////
#import <Carbon/Carbon.h>

/////////// - IOKit - ///////////
#import <IOKit/hid/IOHIDDevice.h>
#import <IOKit/hid/IOHIDManager.h>

namespace storm::window::details {
    /////////////////////////////////////
    /////////////////////////////////////
    auto toStormMouseButton(int button) noexcept -> MouseButton {
        switch (button) {
            case 0: return MouseButton::Left;
            case 1: return MouseButton::Right;
            case 2: return MouseButton::Middle;
            case 3: return MouseButton::Button1;
            case 4: return MouseButton::Button2;
        }

        return MouseButton::Unknow;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto localizedKeyToStormKey(char code) noexcept -> Key {
        switch (code) {
            case 'a':
            case 'A': return Key::A;
            case 'b':
            case 'B': return Key::B;
            case 'c':
            case 'C': return Key::C;
            case 'd':
            case 'D': return Key::D;
            case 'e':
            case 'E': return Key::E;
            case 'f':
            case 'F': return Key::F;
            case 'g':
            case 'G': return Key::G;
            case 'h':
            case 'H': return Key::H;
            case 'i':
            case 'I': return Key::I;
            case 'j':
            case 'J': return Key::J;
            case 'k':
            case 'K': return Key::K;
            case 'l':
            case 'L': return Key::L;
            case 'm':
            case 'M': return Key::M;
            case 'n':
            case 'N': return Key::N;
            case 'o':
            case 'O': return Key::O;
            case 'p':
            case 'P': return Key::P;
            case 'q':
            case 'Q': return Key::Q;
            case 'r':
            case 'R': return Key::R;
            case 's':
            case 'S': return Key::S;
            case 't':
            case 'T': return Key::T;
            case 'u':
            case 'U': return Key::U;
            case 'v':
            case 'V': return Key::V;
            case 'w':
            case 'W': return Key::W;
            case 'x':
            case 'X': return Key::X;
            case 'y':
            case 'Y': return Key::Y;
            case 'z':
            case 'Z': return Key::Z;
        }

        return Key::Unknow;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto nonLocalizedKeytoStormKey(unsigned short code) noexcept -> Key {
        switch (code) {
            case kVK_ANSI_A: return Key::A;
            case kVK_ANSI_B: return Key::B;
            case kVK_ANSI_C: return Key::C;
            case kVK_ANSI_D: return Key::D;
            case kVK_ANSI_E: return Key::E;
            case kVK_ANSI_F: return Key::F;
            case kVK_ANSI_G: return Key::G;
            case kVK_ANSI_H: return Key::H;
            case kVK_ANSI_I: return Key::I;
            case kVK_ANSI_J: return Key::J;
            case kVK_ANSI_K: return Key::K;
            case kVK_ANSI_L: return Key::L;
            case kVK_ANSI_M: return Key::M;
            case kVK_ANSI_N: return Key::N;
            case kVK_ANSI_O: return Key::O;
            case kVK_ANSI_P: return Key::P;
            case kVK_ANSI_Q: return Key::Q;
            case kVK_ANSI_R: return Key::R;
            case kVK_ANSI_S: return Key::S;
            case kVK_ANSI_T: return Key::T;
            case kVK_ANSI_U: return Key::U;
            case kVK_ANSI_V: return Key::V;
            case kVK_ANSI_W: return Key::W;
            case kVK_ANSI_X: return Key::X;
            case kVK_ANSI_Y: return Key::Y;
            case kVK_ANSI_Z: return Key::Z;

            case kVK_ANSI_0: return Key::Num0;
            case kVK_ANSI_1: return Key::Num1;
            case kVK_ANSI_2: return Key::Num2;
            case kVK_ANSI_3: return Key::Num3;
            case kVK_ANSI_4: return Key::Num4;
            case kVK_ANSI_5: return Key::Num5;
            case kVK_ANSI_6: return Key::Num6;
            case kVK_ANSI_7: return Key::Num7;
            case kVK_ANSI_8: return Key::Num8;
            case kVK_ANSI_9: return Key::Num9;

            case kVK_ANSI_Keypad0: return Key::Numpad0;
            case kVK_ANSI_Keypad1: return Key::Numpad1;
            case kVK_ANSI_Keypad2: return Key::Numpad2;
            case kVK_ANSI_Keypad3: return Key::Numpad3;
            case kVK_ANSI_Keypad4: return Key::Numpad4;
            case kVK_ANSI_Keypad5: return Key::Numpad5;
            case kVK_ANSI_Keypad6: return Key::Numpad6;
            case kVK_ANSI_Keypad7: return Key::Numpad7;
            case kVK_ANSI_Keypad8: return Key::Numpad8;
            case kVK_ANSI_Keypad9: return Key::Numpad9;

            case kVK_ANSI_KeypadPlus: return Key::Add;
            case kVK_ANSI_KeypadMinus: return Key::Substract;
            case kVK_ANSI_KeypadMultiply: return Key::Multiply;
            case kVK_ANSI_KeypadDivide: return Key::Divide;
            case kVK_ANSI_KeypadEquals:
            case kVK_ANSI_Equal: return Key::Equal;

            case kVK_ANSI_Slash: return Key::Slash;
            case kVK_ANSI_Backslash: return Key::Back_Slash;
            case kVK_ANSI_Minus: return Key::Hyphen;
            case kVK_ANSI_KeypadDecimal:
            case kVK_ANSI_Period: return Key::Period;
            case kVK_ANSI_Comma: return Key::Comma;
            case kVK_ANSI_Semicolon: return Key::Semi_Colon;
            case kVK_ANSI_Quote: return Key::Quote;
            case kVK_ANSI_LeftBracket: return Key::L_Bracket;
            case kVK_ANSI_RightBracket: return Key::R_Bracket;
            case kVK_Tab: return Key::Tab;
            case kVK_ANSI_KeypadEnter:
            case kVK_Return: return Key::Enter;
            case kVK_Space: return Key::Space;
            case kVK_ISO_Section: return Key::Tilde;

            case NSF1FunctionKey:
            case kVK_F1: return Key::F1;
            case NSF2FunctionKey:
            case kVK_F2: return Key::F2;
            case NSF3FunctionKey:
            case kVK_F3: return Key::F3;
            case NSF4FunctionKey:
            case kVK_F4: return Key::F4;
            case NSF5FunctionKey:
            case kVK_F5: return Key::F5;
            case NSF6FunctionKey:
            case kVK_F6: return Key::F6;
            case NSF7FunctionKey:
            case kVK_F7: return Key::F7;
            case NSF8FunctionKey:
            case kVK_F8: return Key::F8;
            case NSF9FunctionKey:
            case kVK_F9: return Key::F9;
            case NSF10FunctionKey:
            case kVK_F10: return Key::F10;
            case NSF11FunctionKey:
            case kVK_F11: return Key::F11;
            case NSF12FunctionKey:
            case kVK_F12: return Key::F12;
            case NSF13FunctionKey:
            case kVK_F13: return Key::F13;
            case NSF14FunctionKey:
            case kVK_F14: return Key::F14;
            case NSF15FunctionKey:
            case kVK_F15: return Key::F15;

            case NSUpArrowFunctionKey:
            case kVK_UpArrow: return Key::Up;
            case NSDownArrowFunctionKey:
            case kVK_DownArrow: return Key::Down;
            case NSLeftArrowFunctionKey:
            case kVK_LeftArrow: return Key::Left;
            case NSRightArrowFunctionKey:
            case kVK_RightArrow: return Key::Right;

            case kVK_Control: return Key::L_Control;
            case kVK_RightControl: return Key::R_Control;
            case kVK_Shift: return Key::L_Shift;
            case kVK_RightShift: return Key::R_Shift;
            case kVK_Option: return Key::L_Alt;
            case kVK_RightOption: return Key::R_Alt;
            case kVK_Command: return Key::L_System;
            case kVK_RightCommand: return Key::R_System;

            case kVK_Escape: return Key::Escape;

            case NSMenuFunctionKey:
            case 0x7f: return Key::Menu;
            case NSPageUpFunctionKey:
            case kVK_PageUp: return Key::Page_Up;
            case NSPageDownFunctionKey:
            case kVK_PageDown: return Key::Page_Down;
            case NSEndFunctionKey:
            case kVK_End: return Key::End;
            case NSHomeFunctionKey:
            case kVK_Home: return Key::Home;
            case NSInsertFunctionKey:
            case kVK_Help: return Key::Insert;
            case NSDeleteFunctionKey:
            case kVK_ForwardDelete: return Key::Delete;

            case NSBeginFunctionKey: return Key::Begin;
            case NSPauseFunctionKey: return Key::Pause;
            default: return Key::Unknow;
        }

        return Key::Unknow;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto usageToVirtualCode(core::UInt32 usage) noexcept -> core::UInt8 {
        switch (usage) {
            case kHIDUsage_KeyboardErrorRollOver: return 0xff;
            case kHIDUsage_KeyboardPOSTFail: return 0xff;
            case kHIDUsage_KeyboardErrorUndefined: return 0xff;

            case kHIDUsage_KeyboardA: return 0x00;
            case kHIDUsage_KeyboardB: return 0x0b;
            case kHIDUsage_KeyboardC: return 0x08;
            case kHIDUsage_KeyboardD: return 0x02;
            case kHIDUsage_KeyboardE: return 0x0e;
            case kHIDUsage_KeyboardF: return 0x03;
            case kHIDUsage_KeyboardG: return 0x05;
            case kHIDUsage_KeyboardH: return 0x04;
            case kHIDUsage_KeyboardI: return 0x22;
            case kHIDUsage_KeyboardJ: return 0x26;
            case kHIDUsage_KeyboardK: return 0x28;
            case kHIDUsage_KeyboardL: return 0x25;
            case kHIDUsage_KeyboardM: return 0x2e;
            case kHIDUsage_KeyboardN: return 0x2d;
            case kHIDUsage_KeyboardO: return 0x1f;
            case kHIDUsage_KeyboardP: return 0x23;
            case kHIDUsage_KeyboardQ: return 0x0c;
            case kHIDUsage_KeyboardR: return 0x0f;
            case kHIDUsage_KeyboardS: return 0x01;
            case kHIDUsage_KeyboardT: return 0x11;
            case kHIDUsage_KeyboardU: return 0x20;
            case kHIDUsage_KeyboardV: return 0x09;
            case kHIDUsage_KeyboardW: return 0x0d;
            case kHIDUsage_KeyboardX: return 0x07;
            case kHIDUsage_KeyboardY: return 0x10;
            case kHIDUsage_KeyboardZ: return 0x06;

            case kHIDUsage_Keyboard1: return 0x12;
            case kHIDUsage_Keyboard2: return 0x13;
            case kHIDUsage_Keyboard3: return 0x14;
            case kHIDUsage_Keyboard4: return 0x15;
            case kHIDUsage_Keyboard5: return 0x17;
            case kHIDUsage_Keyboard6: return 0x16;
            case kHIDUsage_Keyboard7: return 0x1a;
            case kHIDUsage_Keyboard8: return 0x1c;
            case kHIDUsage_Keyboard9: return 0x19;
            case kHIDUsage_Keyboard0: return 0x1d;

            case kHIDUsage_KeyboardReturnOrEnter: return 0x24;
            case kHIDUsage_KeyboardEscape: return 0x35;
            case kHIDUsage_KeyboardDeleteOrBackspace: return 0x33;
            case kHIDUsage_KeyboardTab: return 0x30;
            case kHIDUsage_KeyboardSpacebar: return 0x31;
            case kHIDUsage_KeyboardHyphen: return 0x1b;
            case kHIDUsage_KeyboardEqualSign: return 0x18;
            case kHIDUsage_KeyboardOpenBracket: return 0x21;
            case kHIDUsage_KeyboardCloseBracket: return 0x1e;
            case kHIDUsage_KeyboardBackslash: return 0x2a;
            case kHIDUsage_KeyboardNonUSPound: return 0xff;
            case kHIDUsage_KeyboardSemicolon: return 0x29;
            case kHIDUsage_KeyboardQuote: return 0x27;
            case kHIDUsage_KeyboardGraveAccentAndTilde: return 0x32;
            case kHIDUsage_KeyboardComma: return 0x2b;
            case kHIDUsage_KeyboardPeriod: return 0x2F;
            case kHIDUsage_KeyboardSlash: return 0x2c;
            case kHIDUsage_KeyboardCapsLock: return 0x39;

            case kHIDUsage_KeyboardF1: return 0x7a;
            case kHIDUsage_KeyboardF2: return 0x78;
            case kHIDUsage_KeyboardF3: return 0x63;
            case kHIDUsage_KeyboardF4: return 0x76;
            case kHIDUsage_KeyboardF5: return 0x60;
            case kHIDUsage_KeyboardF6: return 0x61;
            case kHIDUsage_KeyboardF7: return 0x62;
            case kHIDUsage_KeyboardF8: return 0x64;
            case kHIDUsage_KeyboardF9: return 0x65;
            case kHIDUsage_KeyboardF10: return 0x6d;
            case kHIDUsage_KeyboardF11: return 0x67;
            case kHIDUsage_KeyboardF12: return 0x6f;

            case kHIDUsage_KeyboardPrintScreen: return 0xff;
            case kHIDUsage_KeyboardScrollLock: return 0xff;
            case kHIDUsage_KeyboardPause: return 0xff;
            case kHIDUsage_KeyboardInsert: return 0x72;
            case kHIDUsage_KeyboardHome: return 0x73;
            case kHIDUsage_KeyboardPageUp: return 0x74;
            case kHIDUsage_KeyboardDeleteForward: return 0x75;
            case kHIDUsage_KeyboardEnd: return 0x77;
            case kHIDUsage_KeyboardPageDown: return 0x79;

            case kHIDUsage_KeyboardRightArrow: return 0x7c;
            case kHIDUsage_KeyboardLeftArrow: return 0x7b;
            case kHIDUsage_KeyboardDownArrow: return 0x7d;
            case kHIDUsage_KeyboardUpArrow: return 0x7e;

            case kHIDUsage_KeypadNumLock: return 0x47;
            case kHIDUsage_KeypadSlash: return 0x4b;
            case kHIDUsage_KeypadAsterisk: return 0x43;
            case kHIDUsage_KeypadHyphen: return 0x4e;
            case kHIDUsage_KeypadPlus: return 0x45;
            case kHIDUsage_KeypadEnter: return 0x4c;

            case kHIDUsage_Keypad1: return 0x53;
            case kHIDUsage_Keypad2: return 0x54;
            case kHIDUsage_Keypad3: return 0x55;
            case kHIDUsage_Keypad4: return 0x56;
            case kHIDUsage_Keypad5: return 0x57;
            case kHIDUsage_Keypad6: return 0x58;
            case kHIDUsage_Keypad7: return 0x59;
            case kHIDUsage_Keypad8: return 0x5b;
            case kHIDUsage_Keypad9: return 0x5c;
            case kHIDUsage_Keypad0: return 0x52;

            case kHIDUsage_KeypadPeriod: return 0x41;
            case kHIDUsage_KeyboardNonUSBackslash: return 0xff;
            case kHIDUsage_KeyboardApplication: return 0x6e;
            case kHIDUsage_KeyboardPower: return 0xff;
            case kHIDUsage_KeypadEqualSign: return 0x51;

            case kHIDUsage_KeyboardF13: return 0x69;
            case kHIDUsage_KeyboardF14: return 0x6b;
            case kHIDUsage_KeyboardF15: return 0x71;
            case kHIDUsage_KeyboardF16: return 0xff;
            case kHIDUsage_KeyboardF17: return 0xff;
            case kHIDUsage_KeyboardF18: return 0xff;
            case kHIDUsage_KeyboardF19: return 0xff;
            case kHIDUsage_KeyboardF20: return 0xff;
            case kHIDUsage_KeyboardF21: return 0xff;
            case kHIDUsage_KeyboardF22: return 0xff;
            case kHIDUsage_KeyboardF23: return 0xff;
            case kHIDUsage_KeyboardF24: return 0xff;

            case kHIDUsage_KeyboardExecute: return 0xff;
            case kHIDUsage_KeyboardHelp: return 0xff;
            case kHIDUsage_KeyboardMenu: return 0x7F;
            case kHIDUsage_KeyboardSelect: return 0x4c;
            case kHIDUsage_KeyboardStop: return 0xff;
            case kHIDUsage_KeyboardAgain: return 0xff;
            case kHIDUsage_KeyboardUndo: return 0xff;
            case kHIDUsage_KeyboardCut: return 0xff;
            case kHIDUsage_KeyboardCopy: return 0xff;
            case kHIDUsage_KeyboardPaste: return 0xff;
            case kHIDUsage_KeyboardFind: return 0xff;

            case kHIDUsage_KeyboardMute: return 0xff;
            case kHIDUsage_KeyboardVolumeUp: return 0xff;
            case kHIDUsage_KeyboardVolumeDown: return 0xff;

            case kHIDUsage_KeyboardLockingCapsLock: return 0xff;
            case kHIDUsage_KeyboardLockingNumLock: return 0xff;
            case kHIDUsage_KeyboardLockingScrollLock: return 0xff;

            case kHIDUsage_KeypadComma: return 0xff;
            case kHIDUsage_KeypadEqualSignAS400: return 0xff;
            case kHIDUsage_KeyboardInternational1: return 0xff;
            case kHIDUsage_KeyboardInternational2: return 0xff;
            case kHIDUsage_KeyboardInternational3: return 0xff;
            case kHIDUsage_KeyboardInternational4: return 0xff;
            case kHIDUsage_KeyboardInternational5: return 0xff;
            case kHIDUsage_KeyboardInternational6: return 0xff;
            case kHIDUsage_KeyboardInternational7: return 0xff;
            case kHIDUsage_KeyboardInternational8: return 0xff;
            case kHIDUsage_KeyboardInternational9: return 0xff;

            case kHIDUsage_KeyboardLANG1: return 0xff;
            case kHIDUsage_KeyboardLANG2: return 0xff;
            case kHIDUsage_KeyboardLANG3: return 0xff;
            case kHIDUsage_KeyboardLANG4: return 0xff;
            case kHIDUsage_KeyboardLANG5: return 0xff;
            case kHIDUsage_KeyboardLANG6: return 0xff;
            case kHIDUsage_KeyboardLANG7: return 0xff;
            case kHIDUsage_KeyboardLANG8: return 0xff;
            case kHIDUsage_KeyboardLANG9: return 0xff;

            case kHIDUsage_KeyboardAlternateErase: return 0xff;
            case kHIDUsage_KeyboardSysReqOrAttention: return 0xff;
            case kHIDUsage_KeyboardCancel: return 0xff;
            case kHIDUsage_KeyboardClear: return 0xff;
            case kHIDUsage_KeyboardPrior: return 0xff;
            case kHIDUsage_KeyboardReturn: return 0xff;
            case kHIDUsage_KeyboardSeparator: return 0xff;
            case kHIDUsage_KeyboardOut: return 0xff;
            case kHIDUsage_KeyboardOper: return 0xff;
            case kHIDUsage_KeyboardClearOrAgain: return 0xff;
            case kHIDUsage_KeyboardCrSelOrProps: return 0xff;
            case kHIDUsage_KeyboardExSel: return 0xff;

            case kHIDUsage_KeyboardLeftControl: return 0x3b;
            case kHIDUsage_KeyboardLeftShift: return 0x38;
            case kHIDUsage_KeyboardLeftAlt: return 0x3a;
            case kHIDUsage_KeyboardLeftGUI: return 0x37;
            case kHIDUsage_KeyboardRightControl: return 0x3e;
            case kHIDUsage_KeyboardRightShift: return 0x3c;
            case kHIDUsage_KeyboardRightAlt: return 0x3d;
            case kHIDUsage_KeyboardRightGUI: return 0x36;
            case kHIDUsage_Keyboard_Reserved: return 0xff;
            default: return 0xff;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto toStormVec(NSPoint point) noexcept -> core::Vector2u {
        return { static_cast<core::UInt16>(point.x), static_cast<core::UInt16>(point.y) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto fromStormVec(const storm::core::Vector2u &point) noexcept -> NSPoint {
        return CGPointMake(point.x, point.y);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto toStormVec(CGSize size) noexcept -> core::Extentu {
        return { static_cast<core::UInt32>(size.width), static_cast<core::UInt32>(size.height) };
    }
}
