/*
 * Source/SourceBuffer.h
 * © suhas pai
 */

#pragma once

#include <expected>
#include <string>

namespace ADT {
    struct SourceBuffer {
    public:
        struct Error {
            enum class Kind : uint8_t {
                FailedToOpenFile,
                FailedToStatFile,
                FailedToMapFile
            };

            Kind Kind;
            std::string Reason;

            constexpr Error(const enum Kind Kind, std::string &&Reason) noexcept
            : Kind(Kind), Reason(std::move(Reason)) {}
        };
    protected:
        enum class DestroyMapKind : uint8_t {
            Normal,
            Mapped,
            Allocated,
        };

        void *Base;
        size_t Size;

        DestroyMapKind DestroyKind : 2 = DestroyMapKind::Normal;

        constexpr explicit
        SourceBuffer(void *const Base,
                     const size_t Size,
                     const DestroyMapKind Kind) noexcept
        : Base(Base), Size(Size), DestroyKind(Kind) {}
    public:
        [[nodiscard]] static auto FromFile(const std::string_view Path) noexcept
            -> std::expected<SourceBuffer *, Error>;

        [[nodiscard]] static auto FromAlloc(void *Base, size_t Size) noexcept
            -> SourceBuffer *;

        [[nodiscard]] static
        auto FromString(const std::string_view Text) noexcept -> SourceBuffer *
        {
            const auto Copy = new char[Text.length() + 1]{0};
            memcpy(Copy, Text.data(), Text.length());

            return FromAlloc(Copy, Text.length());
        }

        ~SourceBuffer() noexcept;

        [[nodiscard]] constexpr auto text() const noexcept {
            return std::string_view(static_cast<const char *>(this->Base),
                                    this->Size);
        }
    };
}