/*
 * Parse/ParseMisc.h
 * © suhas pai
 */

#pragma once

#include "AST/Qualifiers.h"
#include "Parse/Context.h"

namespace Parse {
    enum class ProceedResult {
        Failed,
        Comma,
        EndToken
    };

    [[nodiscard]] auto
    ProceedToAndConsumeCommaOrEnd(ParseContext &Context,
                                  const Lex::TokenKind EndKind) noexcept
        -> ProceedResult;

    void
    ParseQualifiers(ParseContext &Context,
                    AST::Qualifiers &Qualifiers) noexcept;

    void
    MargeQualifiers(AST::Qualifiers &Lhs, const AST::Qualifiers &Rhs) noexcept;

    [[nodiscard]] bool ExpectSemicolon(ParseContext &Context) noexcept;
    [[nodiscard]] bool ProceedToSemicolon(ParseContext &Context) noexcept;
}
