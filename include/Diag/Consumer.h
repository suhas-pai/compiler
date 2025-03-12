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
public:
    explicit
    SourceFileDiagnosticConsumer(const std::string_view FilePath) noexcept
    : FilePath(FilePath) {}

    explicit SourceFileDiagnosticConsumer(std::string &&FilePath) noexcept
    : FilePath(std::move(FilePath)) {}

    void consume(const DiagnosticMessage &Message) noexcept override {
        this->MessageList.emplace_back(Message);
    }

    constexpr auto print() const noexcept -> decltype(*this) {
        // FIXME: Add support for printing the location of the message
        for (const auto &DiagMessage : this->MessageList) {
            std::print("{}:{}\n", this->FilePath, DiagMessage.Message);
        }

        return *this;
    }
};
