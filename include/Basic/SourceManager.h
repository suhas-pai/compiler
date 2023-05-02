/*
 * Basic/SourceManager.h
 */

#pragma once

#include <string>
#include "Basic/ErrorOr.h"
#include "Interface/DiagnosticsEngine.h"

struct SourceManager {
public:
    struct FailedToOpenFileError : public ErrorBase {
        std::string Reason;
    };

    struct FailedToStatError  : public ErrorBase {
        std::string Reason;
    };

    struct FailedToMapError  : public ErrorBase {
        std::string Reason;
    };

    struct Error : public ErrorBase {
        enum class ErrorKind : uint8_t {
            FailedToOpenFile,
            FailedToStatFile,
            FailedToMapFile
        };


        ErrorKind Kind;
        union {
            struct FailedToOpenFileError *OpenError;
            struct FailedToStatError *StatError;
            struct FailedToMapError *MapError;
        };

        [[nodiscard]] constexpr static
        auto createOpenError(const std::string_view Reason) noexcept -> Error {
            return Error {
                .Kind = ErrorKind::FailedToOpenFile,
                .OpenError = new FailedToOpenFileError {
                    .Reason = std::string(Reason)
                }
            };
        }

        [[nodiscard]] constexpr static
        auto createStatError(const std::string_view Reason) noexcept -> Error {
            return Error {
                .Kind = ErrorKind::FailedToStatFile,
                .StatError = new FailedToStatError {
                    .Reason = std::string(Reason)
                }
            };
        }

        [[nodiscard]] constexpr static
        auto createMapError(const std::string_view Reason) noexcept -> Error {
            return Error {
                .Kind = ErrorKind::FailedToMapFile,
                .MapError = new FailedToMapError {
                    .Reason = std::string(Reason)
                }
            };
        }
    };
protected:
    void *Base;
    size_t Size;

    bool IsMapped : 1 = false;

    constexpr explicit
    SourceManager(void *const Base,
                  const size_t Size,
                  const bool IsMapped = false) noexcept
    : Base(Base), Size(Size), IsMapped(IsMapped) {}
public:
    [[nodiscard]] static
    auto fromString(const std::string_view Text) noexcept -> SourceManager * {
        const auto Copy = new char[Text.length() + 1]{0};
        memcpy(Copy, Text.data(), Text.length());

        return new SourceManager(const_cast<char *>(Copy), Text.length());
    }

    [[nodiscard]] static auto fromFile(const std::string_view Path) noexcept ->
        ErrorOr<SourceManager *, Error>;

    ~SourceManager() noexcept;

    [[nodiscard]] constexpr auto getText() const noexcept -> std::string_view {
        return std::string_view(static_cast<char *>(Base), Size);
    }
};