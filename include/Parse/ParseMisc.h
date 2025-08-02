/*
 * Parse/ParseMisc.h
 * © suhas pai
 */

#pragma once

#include "AST/CompoundStmt.h"
#include "AST/IfExpr.h"
#include "AST/Qualifiers.h"

#include "Parse/Context.h"
#include "Parse/ParseExpr.h"

namespace Parse {
    enum class ProceedResult {
        Failed,
        Separator,
        EndToken
    };

    [[nodiscard]] auto
    ProceedToAndConsumeSeparatorOrEnd(ParseContext &Context,
                                      Lex::TokenKind SeparatorKind,
                                      Lex::TokenKind EndKind) noexcept
        -> ProceedResult;

    void
    ParseQualifiers(ParseContext &Context,
                    AST::Qualifiers &Qualifiers) noexcept;

    void
    MargeQualifiers(AST::Qualifiers &Lhs, const AST::Qualifiers &Rhs) noexcept;

    bool ExpectSemicolon(ParseContext &Context) noexcept;
    [[nodiscard]] bool ProceedToSemicolon(ParseContext &Context) noexcept;

    [[nodiscard]] auto
    ParseIfExpr(ParseContext &Context,
                const Lex::Token IfToken,
                auto &&Parser,
                std::optional<Lex::TokenKind> SeparatorOpt,
                bool WarnOnTrailingSeparator) noexcept
        -> std::expected<AST::IfExpr *, ParseError>;

    struct ParseListOptions {
        std::string_view Name;

        bool WarnOnTrailingSeparator = false;
        bool RequireSeparatorOnControlFlowExpr = false;
        bool RequireSeparatorInControlFlowExpr = false;
    };

    template <typename T>
    [[nodiscard]] static auto
    ParseListWithSeparator(
        ParseContext &Context,
        const Lex::TokenKind CloseTokenKind,
        const std::optional<Lex::TokenKind> SeparatorTokenKindOpt,
        auto &&Parser,
        const ParseListOptions &Options) noexcept
            -> std::expected<std::vector<T>, ParseError>
    {
        auto &TokenStream = Context.TokenStream;
        auto ResultList = std::vector<T>();

        if (TokenStream.consumeIfIs(CloseTokenKind)) {
            return ResultList;
        }

        auto &Diag = Context.Diag;
        if (SeparatorTokenKindOpt.has_value()) {
            if (const auto SeparatorTokenOpt =
                    TokenStream.consumeIfIs(SeparatorTokenKindOpt.value()))
            {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = SeparatorTokenOpt->Loc,
                    .Message =
                        std::format("Expected an {} before '{}'",
                                    Options.Name,
                                    Lex::TokenKindGetLexeme(
                                        SeparatorTokenKindOpt.value()).value(),
                                    Options.Name)
                });

                // Skip to the next stmt or close-token
                const auto TokenOpt =
                    TokenStream.findNextNotOneOfAndGoToToken({
                        SeparatorTokenKindOpt.value()
                    });

                if (!TokenOpt.has_value()) {
                    return std::unexpected(ParseError::FailedCouldNotProceed);
                }
            }
        }

        do {
            auto SeparatorLocOpt = std::optional<SourceLocation>(std::nullopt);
            const auto ResultOpt = Parser(Context);

            if (!ResultOpt.has_value()) {
                return std::unexpected(ResultOpt.error());
            }

            ResultList.emplace_back(ResultOpt.value());
            if (SeparatorTokenKindOpt.has_value()) {
                if (const auto SeparatorTokenOpt =
                        TokenStream.consumeIfIs(SeparatorTokenKindOpt.value()))
                {
                    SeparatorLocOpt = SeparatorTokenOpt->Loc;
                }

                if (const auto RepeatSeparatorTokenOpt =
                        TokenStream.consumeIfIs(SeparatorTokenKindOpt.value()))
                {
                    Diag.consume({
                        .Level = DiagnosticLevel::Warning,
                        .Location = RepeatSeparatorTokenOpt->Loc,
                        .Message =
                            std::format("Multiple '{}' found after {}",
                                        Lex::TokenKindGetLexeme(
                                            SeparatorTokenKindOpt.value())
                                                .value(),
                                        Options.Name)
                    });

                    // Skip to the next stmt or close-token
                    const auto TokenOpt =
                        TokenStream.findNextNotOneOfAndGoToToken({
                            SeparatorTokenKindOpt.value()
                        });

                    if (!TokenOpt.has_value()) {
                        return std::unexpected(
                            ParseError::FailedCouldNotProceed);
                    }
                }
            }

            if (TokenStream.consumeIfIs(CloseTokenKind)) {
                if (SeparatorLocOpt.has_value()) {
                    const auto SepKind = SeparatorTokenKindOpt.value();
                    if (Options.WarnOnTrailingSeparator)  {
                        // Emit errors for trailing separators before an end
                        // token

                        Diag.consume({
                            .Level = DiagnosticLevel::Error,
                            .Location = SeparatorLocOpt.value(),
                            .Message =
                                std::format("Trailing '{}' before "
                                            "closing '{}' ",
                                            Lex::TokenKindGetLexeme(SepKind)
                                                .value(),
                                            Lex::TokenKindGetLexeme(
                                                CloseTokenKind).value(),
                                            Options.Name)
                        });
                    }
                }

                break;
            }
        } while (true);

        return ResultList;
    }

    template <std::derived_from<AST::Stmt> T = AST::Stmt>
    [[nodiscard]] static auto
    ParseMultiExprListWithSeparator(
        ParseContext &Context,
        const Lex::TokenKind CloseTokenKind,
        const std::optional<Lex::TokenKind> SeparatorTokenKindOpt,
        auto &&Parser,
        const ParseListOptions &Options) noexcept
            -> std::expected<std::vector<T *>, ParseError>
    {
        auto &TokenStream = Context.TokenStream;
        auto ResultList = std::vector<T *>();

        if (TokenStream.consumeIfIs(CloseTokenKind)) {
            return ResultList;
        }

        auto &Diag = Context.Diag;
        if (SeparatorTokenKindOpt.has_value()) {
            if (const auto SeparatorTokenOpt =
                    TokenStream.consumeIfIs(SeparatorTokenKindOpt.value()))
            {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = SeparatorTokenOpt->Loc,
                    .Message =
                        std::format("Expected an {} before '{}'",
                                    Options.Name,
                                    Lex::TokenKindGetLexeme(
                                        SeparatorTokenKindOpt.value()).value(),
                                    Options.Name)
                });

                // Skip to the next stmt or close-token
                const auto TokenOpt =
                    TokenStream.findNextNotOneOfAndGoToToken({
                        SeparatorTokenKindOpt.value()
                    });

                if (!TokenOpt.has_value()) {
                    return std::unexpected(ParseError::FailedCouldNotProceed);
                }
            }
        }

        do {
            auto WasControlFlowExpr = false;
            auto SeparatorLocOpt = std::optional<SourceLocation>(std::nullopt);

            if (const auto IfTokenOpt =
                    TokenStream.consumeIfIsKeyword(Lex::Keyword::If))
            {
                const auto IfExprOpt =
                    ParseIfExpr(Context, IfTokenOpt.value(), Parser,
                                SeparatorTokenKindOpt,
                                Options.WarnOnTrailingSeparator);

                if (!IfExprOpt.has_value()) {
                    return std::unexpected(IfExprOpt.error());
                }

                ResultList.emplace_back(IfExprOpt.value());
                WasControlFlowExpr = true;
            } else {
                const auto ExprOpt = Parser(Context);
                if (!ExprOpt.has_value()) {
                    return std::unexpected(ExprOpt.error());
                }

                ResultList.emplace_back(ExprOpt.value());
            }

            if (SeparatorTokenKindOpt.has_value()) {
                if (const auto SeparatorTokenOpt =
                        TokenStream.consumeIfIs(SeparatorTokenKindOpt.value()))
                {
                    SeparatorLocOpt = SeparatorTokenOpt->Loc;
                }

                if (const auto RepeatSeparatorTokenOpt =
                        TokenStream.consumeIfIs(SeparatorTokenKindOpt.value()))
                {
                    Diag.consume({
                        .Level = DiagnosticLevel::Warning,
                        .Location = RepeatSeparatorTokenOpt->Loc,
                        .Message =
                            std::format("Multiple '{}' found after {}",
                                        Lex::TokenKindGetLexeme(
                                            SeparatorTokenKindOpt.value())
                                                .value(),
                                        Options.Name)
                    });

                    // Skip to the next stmt or close-token
                    const auto TokenOpt =
                        TokenStream.findNextNotOneOfAndGoToToken({
                            SeparatorTokenKindOpt.value()
                        });

                    if (!TokenOpt.has_value()) {
                        return std::unexpected(
                            ParseError::FailedCouldNotProceed);
                    }

                    TokenStream.goBack();
                }

                continue;
            }

            if (TokenStream.consumeIfIs(CloseTokenKind)) {
                if (SeparatorLocOpt.has_value()) {
                    const auto SepKind = SeparatorTokenKindOpt.value();
                    if (Options.WarnOnTrailingSeparator)  {
                        // Allow trailing separators before an end-token, but
                        // emit a warning for it.

                        Diag.consume({
                            .Level = DiagnosticLevel::Warning,
                            .Location = SeparatorLocOpt.value(),
                            .Message =
                                std::format("Trailing '{}' before closing '{}' "
                                            "in list",
                                            Lex::TokenKindGetLexeme(SepKind)
                                                .value(),
                                            Lex::TokenKindGetLexeme(
                                                CloseTokenKind).value(),
                                            Options.Name)
                        });
                    }

                    if (Options.RequireSeparatorInControlFlowExpr &&
                        WasControlFlowExpr)
                    {
                        Diag.consume({
                            .Level = DiagnosticLevel::Error,
                            .Location =
                                TokenStream.getCurrentOrPreviousLocation(),
                            .Message =
                                std::format("Expected a '{}' after {}",
                                            Lex::TokenKindGetLexeme(SepKind)
                                                .value(),
                                            Options.Name)
                        });

                        return std::unexpected(
                            ParseError::FailedCouldNotProceed);
                    }
                }

                break;
            }

            // Allow missing separators for control-flow expressions, but not
            // for other cases.

            if (WasControlFlowExpr &&
                !Options.RequireSeparatorOnControlFlowExpr)
            {
                continue;
            }

            if (SeparatorTokenKindOpt.has_value()) {
                const auto SepKind = SeparatorTokenKindOpt.value();
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = TokenStream.getCurrentOrPreviousLocation(),
                    .Message =
                        std::format("Expected a '{}' after {}",
                                    Lex::TokenKindGetLexeme(SepKind).value(),
                                    Options.Name)
                });
            } else {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = TokenStream.getCurrentOrPreviousLocation(),
                    .Message =
                        std::format("Expected a '{}' after {}",
                                    Lex::TokenKindGetLexeme(CloseTokenKind)
                                        .value(),
                                    Options.Name)
                });
            }

            return std::unexpected(ParseError::FailedCouldNotProceed);
        } while (true);

        return ResultList;
    }

    [[nodiscard]] auto
    ParseIfElseExprCompoundStmt(
        ParseContext &Context,
        const Lex::Token CurlyToken,
        std::optional<Lex::TokenKind> SeparatorOpt,
        auto &&Parser,
        std::optional<SourceLocation> &LastSeparatorLocOut) noexcept
            -> std::expected<AST::CompoundStmt *, ParseError>
    {
        auto Result =
            ParseMultiExprListWithSeparator(
                Context, Lex::TokenKind::CloseCurlyBrace, SeparatorOpt, Parser,
                ParseListOptions{
                    .Name = "if-else",
                    .WarnOnTrailingSeparator = true,
                    .RequireSeparatorOnControlFlowExpr = false,
                    .RequireSeparatorInControlFlowExpr =
                        SeparatorOpt.has_value() &&
                        SeparatorOpt.value() == Lex::TokenKind::Semicolon
                });

        if (!Result.has_value()) {
            return std::unexpected(Result.error());
        }

        auto &ResultList = Result.value();
        return new AST::CompoundStmt(CurlyToken.Loc, std::move(ResultList));
    }

    auto
    ParseIfExpr(ParseContext &Context,
                const Lex::Token IfToken,
                auto &&Parser,
                std::optional<Lex::TokenKind> SeparatorOpt,
                const bool WarnOnTrailingSeparator) noexcept
        -> std::expected<AST::IfExpr *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        auto SawOpenParen = false;
        if (Context.Options.RequireParensOnControlFlowExpr) {
            if (!TokenStream.consumeIfIs(Lex::TokenKind::OpenParen)) {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = IfToken.Loc,
                    .Message =
                        "Expected condition after 'if' to be wrapped in "
                        "parentheses"
                });
            } else {
                SawOpenParen = true;
            }
        }

        auto ConditionOpt = ParseExpression(Context);
        if (!ConditionOpt.has_value()) {
            return std::unexpected(ConditionOpt.error());
        }

        if (Context.Options.RequireParensOnControlFlowExpr) {
            if (!TokenStream.consumeIfIs(Lex::TokenKind::CloseParen)) {
                if (SawOpenParen) {
                    Diag.consume({
                        .Level = DiagnosticLevel::Error,
                        .Location = IfToken.Loc,
                        .Message = "Expected '(' after 'if' keyword"
                    });
                }

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

            if (LastSeparatorLoc.has_value() && WarnOnTrailingSeparator) {
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
                ParseIfExpr(Context, IfTokenOpt.value(), Parser, SeparatorOpt,
                            WarnOnTrailingSeparator);

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
                    ParseIfExpr(Context, IfToken, Parser, SeparatorOpt,
                                WarnOnTrailingSeparator);

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
