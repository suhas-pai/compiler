/*
 * Parse/ParseStmt.cpp
 * © suhas pai
 */

#include "AST/Decls/LvalueNamedDecl.h"

#include "Parse/ParseDecl.h"
#include "Parse/ParseExpr.h"
#include "Parse/ParseMisc.h"
#include "Parse/ParseStmt.h"

namespace Parse {
    auto
    ParseCompoundStmt(ParseContext &Context,
                      const Lex::Token CurlyToken) noexcept
        -> std::expected<AST::CompoundStmt *, ParseError>
    {
        auto Result =
            ParseMultiExprListWithSeparator(
                Context, Lex::TokenKind::CloseCurlyBrace,
                Lex::TokenKind::Semicolon, ParseStmt, ParseListOptions{
                    .Name = "compound statement",
                    .WarnOnTrailingSeparator = false,
                    .RequireSeparatorOnControlFlowExpr = false,
                    .RequireSeparatorInControlFlowExpr = true
                });

        if (!Result.has_value()) {
            return std::unexpected(Result.error());
        }

        return new AST::CompoundStmt(CurlyToken.Loc, std::move(Result.value()));
    }

    auto ParseIfStmt(ParseContext &Context, const Lex::Token IfToken) noexcept
        -> std::expected<AST::IfExpr *, ParseError>
    {
        return ParseIfExpr(Context, IfToken, ParseStmt,
                           Lex::TokenKind::Semicolon,
                           /*WarnOnTrailingSeparator=*/false);
    }

    auto
    ParseReturnStmt(ParseContext &Context,
                    const Lex::Token ReturnToken) noexcept
        -> std::expected<AST::ReturnStmt *, ParseError>
    {
        const auto ExprOpt = ParseExpression(Context);
        if (!ExprOpt.has_value()) {
            return std::unexpected(ExprOpt.error());
        }

        return new AST::ReturnStmt(ReturnToken.Loc, ExprOpt.value());
    }

    auto ParseStmt(ParseContext &Context) noexcept
        -> std::expected<AST::Stmt *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        auto PreIntroducerQualifiers = AST::Qualifiers();
        ParseQualifiers(Context, PreIntroducerQualifiers);

        const auto TokenOpt = TokenStream.consume();
        if (!TokenOpt.has_value()) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getCurrentOrPreviousLocation(),
                .Message = "Unexpected end of file"
            });

            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        const auto Token = TokenOpt.value();
        switch (Token.Kind) {
            case Lex::TokenKind::Keyword: {
                switch (TokenStream.tokenKeyword(Token)) {
                    case Lex::Keyword::Let:
                        return ParseVarDecl(Context, PreIntroducerQualifiers);
                    case Lex::Keyword::Mut:
                    case Lex::Keyword::Volatile:
                    case Lex::Keyword::Inline:
                    case Lex::Keyword::Comptime:
                    case Lex::Keyword::Discardable:
                        // This should've been parsed earlier in qualifiers
                        __builtin_unreachable();
                    case Lex::Keyword::Function: {
                        auto NameTokOpt =
                            std::optional<Lex::Token>(std::nullopt);
                        const auto ResultOpt =
                            ParseFunctionDecl(Context, Token, NameTokOpt);

                        if (!ResultOpt.has_value()) {
                            return std::unexpected(ResultOpt.error());
                        }

                        if (!NameTokOpt.has_value()) {
                            Diag.consume({
                                .Level = DiagnosticLevel::Error,
                                .Location = Token.Loc,
                                .Message =
                                    "Expected name for top-level function "
                                    "declaration"
                            });

                            return std::unexpected(
                                ParseError::FailedCouldNotProceed);
                        }

                        const auto NameTok = NameTokOpt.value();
                        const auto Result = ResultOpt.value();
                        const auto Name = TokenStream.tokenContent(NameTok);

                        return new AST::LvalueNamedDecl(Name, NameTok.Loc,
                                                        Result);
                    }
                    case Lex::Keyword::If:
                        return ParseIfStmt(Context, Token);
                    case Lex::Keyword::Else:
                        __builtin_unreachable();
                    case Lex::Keyword::Return: {
                        const auto ResultOpt = ParseReturnStmt(Context, Token);
                        if (!ResultOpt.has_value()) {
                            return std::unexpected(ResultOpt.error());
                        }

                        return ResultOpt.value();
                    }
                    case Lex::Keyword::Struct: {
                        auto NameTokOpt =
                            std::optional<Lex::Token>(std::nullopt);

                        const auto Result =
                            ParseStructDecl(Context, Token, NameTokOpt);

                        if (!Result.has_value()) {
                            return std::unexpected(Result.error());
                        }

                        if (!NameTokOpt.has_value()) {
                            Diag.consume({
                                .Level = DiagnosticLevel::Error,
                                .Location = Token.Loc,
                                .Message =
                                    "Expected name for top-level struct "
                                    "declaration"
                            });

                            return std::unexpected(
                                ParseError::FailedCouldNotProceed);
                        }

                        const auto NameTok = NameTokOpt.value();
                        const auto Name = TokenStream.tokenContent(NameTok);

                        return new AST::LvalueNamedDecl(Name, NameTok.Loc,
                                                        Result.value());
                    }
                    case Lex::Keyword::Union: {
                        auto NameTokOpt =
                            std::optional<Lex::Token>(std::nullopt);

                        const auto Result =
                            ParseUnionDecl(Context, Token, NameTokOpt);

                        if (!Result.has_value()) {
                            return std::unexpected(Result.error());
                        }

                        if (!NameTokOpt.has_value()) {
                            Diag.consume({
                                .Level = DiagnosticLevel::Error,
                                .Location = Token.Loc,
                                .Message =
                                    "Expected name for top-level union "
                                    "declaration"
                            });

                            return std::unexpected(
                                ParseError::FailedCouldNotProceed);
                        }

                        const auto NameTok = NameTokOpt.value();
                        const auto Name = TokenStream.tokenContent(NameTok);

                        return new AST::LvalueNamedDecl(Name, NameTok.Loc,
                                                        Result.value());
                    }
                    case Lex::Keyword::Class:
                        __builtin_unreachable();
                    case Lex::Keyword::Shape: {
                        auto NameTokOpt =
                            std::optional<Lex::Token>(std::nullopt);

                        const auto Result =
                            ParseShapeDecl(Context, Token, NameTokOpt);

                        if (!Result.has_value()) {
                            return std::unexpected(Result.error());
                        }

                        if (!NameTokOpt.has_value()) {
                            Diag.consume({
                                .Level = DiagnosticLevel::Error,
                                .Location = Token.Loc,
                                .Message =
                                    "Expected name for top-level shape "
                                    "declaration"
                            });

                            return std::unexpected(
                                ParseError::FailedCouldNotProceed);
                        }

                        const auto NameTok = NameTokOpt.value();
                        const auto Name = TokenStream.tokenContent(NameTok);

                        return new AST::LvalueNamedDecl(Name, NameTok.Loc,
                                                        Result.value());
                    }
                    case Lex::Keyword::Interface: {
                        auto NameTokOpt =
                            std::optional<Lex::Token>(std::nullopt);

                        const auto Result =
                            ParseInterfaceDecl(Context, Token, NameTokOpt);

                        if (!Result.has_value()) {
                            return std::unexpected(Result.error());
                        }

                        if (!NameTokOpt.has_value()) {
                            Diag.consume({
                                .Level = DiagnosticLevel::Error,
                                .Location = Token.Loc,
                                .Message =
                                    "Expected name for top-level interface "
                                    "declaration"
                            });
                        }

                        const auto NameTok = NameTokOpt.value();
                        const auto Name = TokenStream.tokenContent(NameTok);

                        return new AST::LvalueNamedDecl(Name, NameTok.Loc,
                                                        Result.value());
                    }
                    case Lex::Keyword::Impl:
                        __builtin_unreachable();
                    case Lex::Keyword::Enum:
                        // return this->parseEnumDeclStmt();
                    case Lex::Keyword::For:
                        // return this->parseForStmt(Token);
                    case Lex::Keyword::And:
                        Diag.consume({
                            .Level = DiagnosticLevel::Error,
                            .Location = Token.Loc,
                            .Message = "Unexpected 'and' keyword"
                        });

                        return std::unexpected(
                            ParseError::FailedCouldNotProceed);
                    case Lex::Keyword::Or:
                        Diag.consume({
                            .Level = DiagnosticLevel::Error,
                            .Location = Token.Loc,
                            .Message = "Unexpected 'or' keyword"
                        });

                        return std::unexpected(
                            ParseError::FailedCouldNotProceed);
                    case Lex::Keyword::In:
                        Diag.consume({
                            .Level = DiagnosticLevel::Error,
                            .Location = Token.Loc,
                            .Message = "Unexpected 'in' keyword"
                        });

                        return std::unexpected(
                            ParseError::FailedCouldNotProceed);
                    case Lex::Keyword::While:
                        __builtin_unreachable();
                    case Lex::Keyword::Default:
                        Diag.consume({
                            .Level = DiagnosticLevel::Error,
                            .Location = Token.Loc,
                            .Message = "Unexpected 'default' keyword"
                        });

                        return std::unexpected(
                            ParseError::FailedCouldNotProceed);
                    case Lex::Keyword::As:
                        Diag.consume({
                            .Level = DiagnosticLevel::Error,
                            .Location = Token.Loc,
                            .Message = "Unexpected 'as' keyword"
                        });

                        return std::unexpected(
                            ParseError::FailedCouldNotProceed);
                }

                [[fallthrough]];
            }
            case Lex::TokenKind::OpenCurlyBrace:
                return ParseCompoundStmt(Context, Token);
            default: {
                TokenStream.goBack();

                const auto ExprOpt = ParseExpressionInPlaceOfStmt(Context);
                if (!ExprOpt.has_value()) {
                    return std::unexpected(ExprOpt.error());
                }

                return ExprOpt.value();
            }
        }

        __builtin_unreachable();
    }
}
