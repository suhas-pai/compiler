/*
 * Lex/TokenBuffer.h
 * © suhas pai
 */

#pragma once

#include <span>
#include <vector>

#include "Diag/Consumer.h"

#include "Lex/Infos.h"
#include "Lex/Token.h"

#include "Source/SourceBuffer.h"

namespace Lex {
    struct TokenBuffer {
    protected:
        const ADT::SourceBuffer &SrcBuffer;

        std::vector<LineInfo> LineInfoList;
        std::vector<Token> TokenList;

        explicit
        TokenBuffer(const ADT::SourceBuffer &SrcBuffer,
                    std::vector<LineInfo> &&LineInfoList,
                    std::vector<Token> &&TokenList) noexcept
        : SrcBuffer(SrcBuffer),
          LineInfoList(std::move(LineInfoList)),
          TokenList(std::move(TokenList)) {}
    public:
        static auto
        Create(const ADT::SourceBuffer &SrcBuffer,
               DiagnosticConsumer &DiagConsumer) noexcept
            -> std::optional<TokenBuffer>;

        [[nodiscard]] constexpr auto &getSourceBuffer() const noexcept {
            return this->SrcBuffer;
        }

        [[nodiscard]] constexpr auto getLineInfoList() const noexcept {
            return std::span(this->LineInfoList);
        }

        [[nodiscard]] constexpr auto getTokenList() const noexcept {
            return std::span(this->TokenList);
        }
    };
}
