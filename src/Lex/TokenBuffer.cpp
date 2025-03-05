/*
 * Lex/TokenBuffer.cpp
 * © suhas pai
 */

#include "Lex/TokenBuffer.h"
#include "Lex/Tokenizer.h"

namespace Lex {
    auto
    TokenBuffer::Create(const ADT::SourceBuffer &SrcBuffer,
                        DiagnosticConsumer &Diag) noexcept
        -> std::optional<TokenBuffer>
    {
        auto Tokenizer = Lex::Tokenizer(SrcBuffer.text(), Diag);
        auto [Token, LineInfo] = Tokenizer.next();

        auto CurrentLineInfo = LineInfo;
        auto LineInfoList = std::vector<Lex::LineInfo>({ CurrentLineInfo });
        auto TokenList = std::vector<Lex::Token>();

        while (!Token.isEof()) {
            TokenList.emplace_back(Token);
            const auto Next = Tokenizer.next();

            Token = Next.first;
            LineInfo = Next.second;

            if (Token.isInvalid()) {
                TokenList.clear();
                return std::nullopt;
            }

            if (CurrentLineInfo.ByteOffset != LineInfo.ByteOffset) {
                LineInfoList.push_back(LineInfo);
                CurrentLineInfo = LineInfo;
            }
        }

        return TokenBuffer(SrcBuffer, std::move(LineInfoList),
                           std::move(TokenList));
    }
}
