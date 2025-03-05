/*
 * AST/Qualifiers.h
 * © suhas pai
 */

#pragma once
#include <unordered_map>

#include "Sema/InlinePolicy.h"
#include "Source/SourceLocation.h"

namespace AST {
    enum class Qualifier {
        Mutable,
        Volatile,
        Comptime,
        Extern,
    };

    struct Qualifiers {
    protected:
        std::unordered_map<Qualifier, SourceLocation> LocationList;

        Sema::InlinePolicy InlinePolicy = Sema::InlinePolicy::None;
        SourceLocation InlineLoc;
    public:
        explicit Qualifiers() noexcept = default;

        [[nodiscard]] constexpr auto isMutable() const noexcept {
            return this->LocationList.contains(Qualifier::Mutable);
        }

        [[nodiscard]] constexpr auto getMutableLoc() const noexcept {
            return this->LocationList.at(Qualifier::Mutable);
        }

        [[nodiscard]] constexpr auto isVolatile() const noexcept {
            return this->LocationList.contains(Qualifier::Volatile);
        }

        [[nodiscard]] constexpr auto getVolatileLoc() const noexcept {
            return this->LocationList.at(Qualifier::Volatile);
        }

        [[nodiscard]] constexpr auto isComptime() const noexcept {
            return this->LocationList.contains(Qualifier::Comptime);
        }

        [[nodiscard]] constexpr auto getComptimeLoc() const noexcept {
            return this->LocationList.at(Qualifier::Comptime);
        }

        [[nodiscard]] constexpr auto isExtern() const noexcept {
            return this->LocationList.contains(Qualifier::Extern);
        }

        [[nodiscard]] constexpr auto getExternLoc() const noexcept {
            return this->LocationList.at(Qualifier::Extern);
        }

        [[nodiscard]] constexpr auto getInlinePolicy() const noexcept {
            return this->InlinePolicy;
        }

        [[nodiscard]] constexpr auto getInlineLoc() const noexcept {
            return this->InlineLoc;
        }

        [[nodiscard]] constexpr auto empty() const noexcept {
            return this->LocationList.empty() &&
                   this->InlinePolicy == Sema::InlinePolicy::None;
        }

        constexpr auto clear() noexcept -> decltype(*this) {
            this->LocationList.clear();
            this->InlinePolicy = Sema::InlinePolicy::None;

            return *this;
        }

        constexpr auto
        setIsMutable(const bool IsMutable, const SourceLocation Loc) noexcept
            -> decltype(*this)
        {
            if (IsMutable) {
                this->LocationList.emplace(Qualifier::Mutable, Loc);
            } else {
                this->LocationList.erase(Qualifier::Mutable);
            }

            return *this;
        }

        constexpr auto
        setIsVolatile(const bool IsVolatile, const SourceLocation Loc) noexcept
            -> decltype(*this)
        {
            if (IsVolatile) {
                this->LocationList.emplace(Qualifier::Volatile, Loc);
            } else {
                this->LocationList.erase(Qualifier::Volatile);
            }

            return *this;
        }

        constexpr auto
        setIsComptime(const bool IsComptime, const SourceLocation Loc) noexcept
            -> decltype(*this)
        {
            if (IsComptime) {
                this->LocationList.emplace(Qualifier::Comptime, Loc);
            } else {
                this->LocationList.erase(Qualifier::Comptime);
            }

            return *this;
        }

        constexpr auto
        setInlinePolicy(const Sema::InlinePolicy InlinePolicy,
                        const SourceLocation Loc) noexcept
            -> decltype(*this)
        {
            if (InlinePolicy != Sema::InlinePolicy::None) {
                this->InlinePolicy = InlinePolicy;
                this->InlineLoc = Loc;
            } else {
                this->InlinePolicy = Sema::InlinePolicy::None;
                this->InlineLoc = SourceLocation();
            }

            return *this;
        }

        constexpr auto
        setIsExtern(const bool IsExtern, const SourceLocation Loc) noexcept
            -> decltype(*this)
        {
            if (IsExtern) {
                this->LocationList.emplace(Qualifier::Extern, Loc);
            } else {
                this->LocationList.erase(Qualifier::Extern);
            }

            return *this;
        }
    };
}
