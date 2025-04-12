/*
 * Parse/ParseIfExpr.h
 * © suhas pai
 */

#pragma once
#include <expected>

#include "AST/CompoundStmt.h"
#include "AST/IfExpr.h"

#include "Parse/ParseError.h"
#include "Parse/ParseExpr.h"

namespace Parse {
    [[nodiscard]] auto
    ParseIfExpr(
        ParseContext &Context,
        const Lex::Token IfToken,
        auto &&Parser,
        std::optional<Lex::TokenKind> SeparatorOpt) noexcept
            -> std::expected<AST::IfExpr *, ParseError>;

    [[nodiscard]] auto
    ParseIfElseExprCompoundStmt(
        ParseContext &Context,
        const Lex::Token CurlyToken,
        std::optional<Lex::TokenKind> SeparatorOpt,
        auto &&Parser,
        std::optional<SourceLocation> &LastSeparatorLocOut) noexcept
            -> std::expected<AST::CompoundStmt *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        auto StmtList = std::vector<AST::Stmt *>();
        if (TokenStream.consumeIfIs(Lex::TokenKind::CloseCurlyBrace)) {
            return new AST::CompoundStmt(CurlyToken.Loc, std::move(StmtList));
        }

        // Parse the first statement in the compound statement
        while (true) {
            if (const auto IfTokenOpt =
                    TokenStream.consumeIfIsKeyword(Lex::Keyword::If))
            {
                const auto IfToken = IfTokenOpt.value();
                const auto IfExprOpt =
                    ParseIfExpr(Context, IfToken, Parser, SeparatorOpt);

                if (!IfExprOpt.has_value()) {
                    return std::unexpected(IfExprOpt.error());
                }

                StmtList.emplace_back(IfExprOpt.value());
                continue;
            }

            const auto StmtOpt = Parser(Context);
            if (!StmtOpt.has_value()) {
                return std::unexpected(StmtOpt.error());
            }

            auto SeparatorLoc = std::optional<SourceLocation>(std::nullopt);
            if (SeparatorOpt.has_value()) {
                const auto Separator = SeparatorOpt.value();
                if (TokenStream.consumeIfIs(Separator)) {
                    SeparatorLoc = TokenStream.getCurrOrPrevLoc();
                }
            }

            StmtList.emplace_back(StmtOpt.value());
            if (TokenStream.consumeIfIs(Lex::TokenKind::CloseCurlyBrace)) {
                if (SeparatorLoc.has_value()) {
                    const auto Separator = SeparatorOpt.value();
                    const auto Lexeme =
                        Lex::TokenKindGetLexeme(Separator).value();

                    Diag.consume({
                        .Level = DiagnosticLevel::Warning,
                        .Location = SeparatorLoc.value(),
                        .Message =
                            std::format("Extra '{}' in statement", Lexeme)
                    });
                }

                LastSeparatorLocOut = SeparatorLoc;
                break;
            }

            if (SeparatorLoc.has_value()) {
                continue;
            }

            const auto CurrentOpt = TokenStream.peek();
            if (!CurrentOpt.has_value()) {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = CurlyToken.Loc,
                    .Message =
                        "Expected closing curly-bracket to end compound "
                        "statement"
                });

                return std::unexpected(ParseError::FailedCouldNotProceed);
            }

            const auto Current = CurrentOpt.value();
            Diag.consume({
                .Level = DiagnosticLevel::Warning,
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message =
                    std::format("Expected ',' or '{}', found '{}' instead",
                                "}",
                                TokenStream.tokenContent(Current))
            });

            // FIXME: Proceed to next separator or end of statement
            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        return new AST::CompoundStmt(CurlyToken.Loc, std::move(StmtList));
    }

    auto
    ParseElseExpr(ParseContext &Context,
                  const Lex::Token IfToken,
                  auto &&Parser,
                  std::optional<Lex::TokenKind> SeparatorOpt =
                    std::nullopt) noexcept
        -> std::expected<AST::Stmt *, ParseError>
    {
        auto &TokenStream = Context.TokenStream;
        if (const auto IfTokenOpt =
                TokenStream.consumeIfIsKeyword(Lex::Keyword::If))
        {
            const auto IfToken = IfTokenOpt.value();
            return ParseIfExpr(Context, IfToken, Parser,
                               /*SeparatorOpt=*/std::nullopt);
        }

        if (const auto CurlyTokenOpt =
                TokenStream.consumeIfIs(Lex::TokenKind::OpenCurlyBrace))
        {
            auto LastSeparatorLoc =
                std::optional<SourceLocation>(std::nullopt);

            const auto CurlyToken = CurlyTokenOpt.value();
            const auto Result =
                ParseIfElseExprCompoundStmt(Context, CurlyToken, SeparatorOpt,
                                            Parser, LastSeparatorLoc);

            if (!Result.has_value()) {
                return std::unexpected(Result.error());
            }

            if (LastSeparatorLoc.has_value()) {
                auto &Diag = Context.Diag;

                const auto Separator = SeparatorOpt.value();
                const auto Lexeme = Lex::TokenKindGetLexeme(Separator).value();

                Diag.consume({
                    .Level = DiagnosticLevel::Warning,
                    .Location = LastSeparatorLoc.value(),
                    .Message =
                        std::format("Extra '{}' in statement", Lexeme)
                });
            }

            return Result.value();
        }

        return Parser(Context);
    }

    auto
    ParseIfExpr(ParseContext &Context,
                const Lex::Token IfToken,
                auto &&Parser,
                std::optional<Lex::TokenKind> SeparatorOpt) noexcept
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

        auto ConditionOpt = ParseExpression(Context);
        if (!ConditionOpt.has_value()) {
            return std::unexpected(ConditionOpt.error());
        }

        if (Context.Options.RequireParensOnIfExpr) {
            if (!TokenStream.consumeIfIs(Lex::TokenKind::CloseParen)) {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = TokenStream.getCurrOrPrevLoc(),
                    .Message = "Expected ')' after 'if' condition"
                });

                const auto ProceedResult =
                    TokenStream.proceedToAndConsume(Lex::TokenKind::CloseParen);

                if (!ProceedResult.has_value()) {
                    return std::unexpected(ParseError::FailedCouldNotProceed);
                }
            }
        }

        auto Then = static_cast<AST::Stmt *>(nullptr);
        if (const auto CurlyTokenOpt =
                TokenStream.consumeIfIs(Lex::TokenKind::OpenCurlyBrace))
        {
            auto LastSeparatorLoc = std::optional<SourceLocation>(std::nullopt);

            const auto CurlyToken = CurlyTokenOpt.value();
            const auto Result =
                ParseIfElseExprCompoundStmt(Context, CurlyToken, SeparatorOpt,
                                            Parser, LastSeparatorLoc);

            if (!Result.has_value()) {
                return std::unexpected(Result.error());
            }

            if (LastSeparatorLoc.has_value()) {
                auto &Diag = Context.Diag;

                const auto Separator = SeparatorOpt.value();
                const auto Lexeme = Lex::TokenKindGetLexeme(Separator).value();

                Diag.consume({
                    .Level = DiagnosticLevel::Warning,
                    .Location = LastSeparatorLoc.value(),
                    .Message = std::format("Extra '{}' in statement", Lexeme)
                });
            }

            Then = Result.value();
        } else {
            const auto Result = Parser(Context);
            if (!Result.has_value()) {
                return std::unexpected(Result.error());
            }

            Then = Result.value();
        }

        if (const auto IfTokenOpt =
                TokenStream.consumeIfIsKeyword(Lex::Keyword::If))
        {
            const auto IfExprOpt =
                ParseIfExpr(Context, IfTokenOpt.value(), Parser, SeparatorOpt);

            if (!IfExprOpt.has_value()) {
                return std::unexpected(IfExprOpt.error());
            }

            return IfExprOpt.value();
        }

        auto Else = static_cast<AST::Stmt *>(nullptr);
        if (TokenStream.consumeIfIsKeyword(Lex::Keyword::Else)) {
            if (const auto CurlyTokenOpt =
                    TokenStream.consumeIfIs(Lex::TokenKind::OpenCurlyBrace))
            {
                auto LastSeparatorLoc =
                    std::optional<SourceLocation>(std::nullopt);

                const auto CurlyToken = CurlyTokenOpt.value();
                const auto Result =
                    ParseIfElseExprCompoundStmt(Context, CurlyToken,
                                                SeparatorOpt, Parser,
                                                LastSeparatorLoc);

                if (!Result.has_value()) {
                    return std::unexpected(Result.error());
                }

                if (LastSeparatorLoc.has_value()) {
                    auto &Diag = Context.Diag;
                    const auto Lexeme =
                        Lex::TokenKindGetLexeme(SeparatorOpt.value()).value();

                    Diag.consume({
                        .Level = DiagnosticLevel::Warning,
                        .Location = LastSeparatorLoc.value(),
                        .Message =
                            std::format("Extra '{}' in statement", Lexeme)
                    });
                }

                Else = Result.value();
            } else if (TokenStream.consumeIfIsKeyword(Lex::Keyword::If)) {
                const auto Result =
                    ParseIfExpr(Context, IfToken, Parser, SeparatorOpt);

                if (!Result.has_value()) {
                    return std::unexpected(Result.error());
                }

                Else = Result.value();
            } else {
                const auto Result = Parser(Context);
                if (!Result.has_value()) {
                    return std::unexpected(Result.error());
                }

                Else = Result.value();
            }
        }

        const auto Condition = ConditionOpt.value();
        return new AST::IfExpr(IfToken.Loc, *Condition, Then, Else);
    }
}
