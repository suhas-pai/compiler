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

    auto
    ProceedToAndConsumeCommaOrEnd(ParseContext &Context,
                                  const Lex::TokenKind EndKind) noexcept
        -> ProceedResult;

    void
    ParseQualifiers(ParseContext &Context,
                    AST::Qualifiers &Qualifiers) noexcept;

    void
    MargeQualifiers(AST::Qualifiers &Lhs, const AST::Qualifiers &Rhs) noexcept;

    bool ExpectSemicolon(ParseContext &Context) noexcept;
    bool ProceedToSemicolon(ParseContext &Context) noexcept;
}
