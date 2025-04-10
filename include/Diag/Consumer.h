/*
 * Diag/Consumer.h
 * © suhas pai
 */

#pragma once

#include <print>
#include <vector>

#include "Diag/Message.h"

struct DiagnosticConsumer {
    virtual void consume(const DiagnosticMessage &Message) noexcept = 0;
};

struct SourceFileDiagnosticConsumer : public DiagnosticConsumer {
protected:
    std::string FilePath;
    std::vector<DiagnosticMessage> MessageList;

    bool HasErrors : 1 = false;
public:
    explicit
    SourceFileDiagnosticConsumer(const std::string_view FilePath) noexcept
    : FilePath(FilePath) {}

    explicit SourceFileDiagnosticConsumer(std::string &&FilePath) noexcept
    : FilePath(std::move(FilePath)) {}

    void consume(const DiagnosticMessage &Message) noexcept override {
        if (Message.Level == DiagnosticLevel::Error) {
            this->HasErrors = true;
        }

        this->MessageList.emplace_back(Message);
    }

    [[nodiscard]] constexpr auto hasMessages() const noexcept {
        return !this->MessageList.empty();
    }

    [[nodiscard]] constexpr auto hasErrors() const noexcept {
        return this->HasErrors;
    }

    [[nodiscard]] constexpr auto getFilePath() const noexcept {
        return this->FilePath;
    }

    constexpr auto print() const noexcept -> decltype(*this) {
        // FIXME: Add support for printing the location of the message
        for (const auto &DiagMessage : this->MessageList) {
            std::print("{}:{}\n", this->FilePath, DiagMessage.Message);
        }

        return *this;
    }
};
