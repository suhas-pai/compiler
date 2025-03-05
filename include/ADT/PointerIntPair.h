/*
 * ADT/PointerIntPair.h
 * © suhas pai
 */

#pragma once

#include <cassert>
#include <concepts>

#include "Basic/Macros.h"

namespace ADT {
    template <typename T, std::integral IntType, unsigned NumBits>
    struct PointerIntPair {
        static_assert(NumBits < sizeof_bits(IntType),
                      "NumBits must be less than Bit-Size of IntType");
        static_assert(alignof(T) >= 1 << NumBits,
                      "Alignment of T must be >= 2^NumBits");
    protected:
        union {
            T *Ptr;
            uintptr_t Value;
        };

        [[nodiscard]] constexpr static auto getMask() noexcept {
            return (1ull << NumBits) - 1;
        }
    public:
        constexpr explicit PointerIntPair(T *const Ptr) noexcept : Ptr(Ptr) {
            assert(this->getBits() == 0);
        }

        constexpr explicit
        PointerIntPair(T *const Ptr, const uintptr_t Bits) noexcept : Ptr(Ptr) {
            assert((this->Value & this->getMask()) == 0);
            this->Value |= Bits;
        }

        [[nodiscard]] constexpr auto getPointer() const noexcept {
            const auto PtrValue = this->Value & ~this->getMask();
            return reinterpret_cast<T *>(PtrValue);
        }

        [[nodiscard]] constexpr auto getBits() const noexcept {
            return this->Value & this->getMask();
        }

        [[nodiscard]]
        constexpr auto setBits(const IntType Bits) const noexcept {
            this->Ptr = reinterpret_cast<T *>(this->getPointer() | Bits);
        }

        [[nodiscard]] constexpr auto setPointer(T *const Ptr) const noexcept {
            const auto PtrValue = reinterpret_cast<uintptr_t>(Ptr);
            this->Ptr = reinterpret_cast<T *>(PtrValue | this->getBits());
        }
    };
}
