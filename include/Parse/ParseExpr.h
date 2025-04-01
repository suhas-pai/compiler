/*
 * Parse/ParseExpr.h
 * © suhas pai
 */

#pragma once

#include "AST/Expr.h"
#include "AST/IfExpr.h"

#include "Parse/Context.h"
#include "Parse/ParseError.h"

namespace Parse {
    [[nodiscard]]
    auto ParseLhs(ParseContext &Context, bool InPlaceOfStmt) noexcept
        -> AST::Expr *;

    [[nodiscard]] auto
    ParseBinaryOperationWithRhsPrec(ParseContext &Context,
                                    AST::Expr *Lhs,
                                    uint32_t MinPrec,
                                    bool InPlaceOfStmt) noexcept -> AST::Expr *;

    [[nodiscard]]
    auto ParseExpression(ParseContext &Context) noexcept -> AST::Expr *;

    [[nodiscard]]
    auto ParseExpressionInPlaceOfStmt(ParseContext &Context) noexcept
        -> AST::Expr *;

    template <typename P>
    [[nodiscard]] auto
    ParseIfExpr(ParseContext &Context,
                const Lex::Token IfToken,
                const P &Parser) noexcept
        -> std::expected<AST::IfExpr *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        if (Context.Options.RequireParensOnIfExpr) {
            if (!TokenStream.consumeIfIs(Lex::TokenKind::OpenParen)) {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = IfToken.Loc,
                    .Message = "Expected '(' after 'if' keyword"
                });

                return std::unexpected(ParseError::FailedCouldNotProceed);
            }
        }

        auto Condition = ParseExpression(Context);
        if (Condition == nullptr) {
            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        if (Context.Options.RequireParensOnIfExpr) {
            if (!TokenStream.consumeIfIs(Lex::TokenKind::CloseParen)) {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = TokenStream.getCurrOrPrevLoc(),
                    .Message = "Expected ')' after 'if' condition"
                });

                return std::unexpected(ParseError::FailedCouldNotProceed);
            }
        }

        auto Then = Parser(Context);
        if (Then == nullptr) {
            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        auto Else = static_cast<AST::Stmt *>(nullptr);
        if (TokenStream.peekIs(Lex::TokenKind::Keyword)) {
            const auto ElseToken = TokenStream.consume().value();
            if (TokenStream.tokenKeyword(ElseToken) == Lex::Keyword::Else) {
                Else = Parser(Context);
                if (Else == nullptr) {
                    return std::unexpected(ParseError::FailedCouldNotProceed);
                }
            }
        }

        return new AST::IfExpr(IfToken.Loc, *Condition, Then, Else);
    }
}
