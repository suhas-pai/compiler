/*
 * ADT/PointerIntPair.h
 * © suhas pai
 */

#pragma once

#include <cassert>
#include <concepts>

namespace ADT {
    template <typename T, std::integral IntType, unsigned NumBits>
    struct PointerIntPair {
        static_assert(NumBits < 64, "NumBits must be less than 64");
        static_assert(alignof(T) >= (1 << NumBits),
                      "Alignment of T must be >= 2^NumBits");
    protected:
        union {
            T *Ptr;
            uintptr_t Value;
        };

        [[nodiscard]] constexpr auto getMask() const noexcept {
            return (1ull << NumBits) - 1;
        }
    public:
        constexpr explicit PointerIntPair(T *const Ptr) noexcept : Ptr(Ptr) {
            assert((Value & getMask()) == 0);
        }

        constexpr explicit
        PointerIntPair(T *const Ptr, const uint64_t Bits) noexcept : Ptr(Ptr) {
            assert((Value & getMask()) == 0);
            Value |= Bits;
        }

        [[nodiscard]] constexpr auto getPointer() const noexcept {
            const auto PtrValue = Value & ~getMask();
            return reinterpret_cast<T *>(PtrValue);
        }

        [[nodiscard]] constexpr auto getBits() const noexcept {
            return Value & getMask();
        }

        [[nodiscard]]
        constexpr auto setBits(const IntType Bits) const noexcept {
            Ptr = reinterpret_cast<T *>(getPointer() | Bits);
        }

        [[nodiscard]] constexpr auto setPointer(T *const Ptr) const noexcept {
            const auto PtrValue = reinterpret_cast<uintptr_t>(Ptr);
            Ptr = reinterpret_cast<T *>(PtrValue | getBits());
        }
    };
}
