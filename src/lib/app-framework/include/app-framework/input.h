#pragma once

namespace physika {

enum MouseButton : int {
    kMouseNone   = 0,
    kMouseLeft   = 1 << 1,
    kMouseRight  = 1 << 2,
    kMouseMiddle = 1 << 3,
    kMouseExtra1 = 1 << 4,
    kMouseExtra2 = 1 << 5,
};

enum Keycode {
    //! Alphabets
    kA,
    kB,
    kC,
    kD,
    kE,
    kF,
    kG,
    kH,
    kI,
    kJ,
    kK,
    kL,
    kM,
    kN,
    kO,
    kP,
    kQ,
    kR,
    kS,
    kT,
    kU,
    kV,
    kW,
    kX,
    kY,
    kZ,
    k0,

    //! numeric
    k1,
    k2,
    k3,
    k4,
    k5,
    k6,
    k7,
    k8,
    k9,

    //! Numpad
    kNumpad0,
    kNumpad1,
    kNumpad2,
    kNumpad3,
    kNumpad4,
    kNumpad5,
    kNumpad6,
    kNumpad7,
    kNumpad8,
    kNumpad9,

    //! Symbols
    kMultiply,
    kAdd,
    kSeparator,
    kSubtract,
    kDecimal,
    kDivide,

    //! Function keys
    kF1,
    kF2,
    kF3,
    kF4,
    kF5,
    kF6,
    kF7,
    kF8,
    kF9,
    kF10,
    kF11,
    kF12,

    //! Modifiers
    kLeftShift,
    kRightShift,
    kLeftControl,
    kRightControl,
    kLeftSuper,
    kRightSuper,
    kLeftAlt,
    kRightAlt,
    kSpace,
    kEsc,

    kKeyCodeCount,
};

//! Ref:
//! https://stackoverflow.com/questions/15889414/how-to-overload-operator-on-scoped-enum
inline MouseButton operator|(MouseButton const& leftSide, MouseButton const& rightSide)
{
    MouseButton result =
        static_cast<MouseButton>(static_cast<int>(leftSide) | static_cast<int>(rightSide));
    return result;
}

inline MouseButton operator&(MouseButton const& leftSide, MouseButton const& rightSide)
{
    MouseButton result =
        static_cast<MouseButton>(static_cast<int>(leftSide) & static_cast<int>(rightSide));
    return result;
}

inline MouseButton& operator|=(MouseButton& leftSide, MouseButton const& rightSide)
{
    leftSide = leftSide | rightSide;
    return leftSide;
}

}  // namespace physika