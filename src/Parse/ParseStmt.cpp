/*
 * Parse/ParseStmt.cpp
 * © suhas pai
 */

#include "Parse/ParseDecl.h"
#include "Parse/ParseExpr.h"
#include "Parse/ParseMisc.h"
#include "Parse/ParseStmt.h"

namespace Parse {
    auto
    ParseCompoundStmt(ParseContext &Context,
                      const Lex::Token CurlyToken) noexcept
        -> AST::CompoundStmt *
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        auto StmtList = std::vector<AST::Stmt *>();
        while (true) {
            const auto TokenOpt = TokenStream.peek();
            if (!TokenOpt.has_value()) {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = CurlyToken.Loc,
                    .Message = "Expected closing curly-bracket to end closing "
                               "statement"
                });

                return nullptr;
            }

            if (TokenStream.consumeIfIs(Lex::TokenKind::CloseCurlyBrace)) {
                break;
            }

            const auto StmtOpt = ParseStmt(Context);
            if (!StmtOpt.has_value()) {
                return nullptr;
            }

            if (!ExpectSemicolon(Context)) {
                return nullptr;
            }

            StmtList.emplace_back(StmtOpt.value());
        }

        return new AST::CompoundStmt(CurlyToken.Loc, std::move(StmtList));
    }

    auto ParseIfStmt(ParseContext &Context, const Lex::Token IfToken) noexcept
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

        const auto Condition = ParseExpression(Context);
        if (Condition == nullptr) {
            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        if (Context.Options.RequireParensOnIfExpr) {
            if (!TokenStream.consumeIfIs(Lex::TokenKind::CloseParen)) {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = TokenStream.getCurrOrPrevLoc(),
                    .Message = "Expected ')' to close condition of 'if' "
                               "expression"
                });

                return std::unexpected(ParseError::FailedCouldNotProceed);
            }
        }

        const auto ThenOpt = ParseStmt(Context);
        if (!ThenOpt.has_value()) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message = "Expected a statement for 'then' part of "
                           "if-expression"
            });

            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        auto Else = static_cast<AST::Stmt *>(nullptr);
        if (TokenStream.consumeIfIsKeyword(Lex::Keyword::If)) {
            const auto ElseOpt = ParseStmt(Context);
            if (!ElseOpt.has_value()) {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = TokenStream.getCurrOrPrevLoc(),
                    .Message = "Expected a statement after 'else' part of "
                                "if-expression"
                });

                return std::unexpected(ParseError::FailedCouldNotProceed);
            }
        }

        const auto Then = ThenOpt.value();
        return new AST::IfExpr(IfToken.Loc, *Condition, Then, Else);
    }

    auto
    ParseReturnStmt(ParseContext &Context,
                    const Lex::Token ReturnToken) noexcept
        -> std::expected<AST::ReturnStmt *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        if (const auto Expr = ParseExpression(Context)) {
            if (ExpectSemicolon(Context)) {
                return new AST::ReturnStmt(ReturnToken.Loc, Expr);
            }
        }

        const auto CurrentOpt = TokenStream.current();
        if (!CurrentOpt.has_value()) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message = "Expected ';', found end of file"
            });

            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        const auto Current = CurrentOpt.value();
        const auto CurrentString = TokenStream.tokenContent(Current);

        Diag.consume({
            .Level = DiagnosticLevel::Error,
            .Location = TokenStream.getCurrOrPrevLoc(),
            .Message =
                std::format("Expected ';', found \"{}\" instead", CurrentString)
        });

        if (ProceedToSemicolon(Context)) {
            return std::unexpected(ParseError::FailedAndProceeded);
        }

        Diag.consume({
            .Level = DiagnosticLevel::Error,
            .Location = TokenStream.getCurrOrPrevLoc(),
            .Message = "Failed to find ';' to end return statement"
        });

        return std::unexpected(ParseError::FailedCouldNotProceed);
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
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message = "Unexpected end of file"
            });

            return nullptr;
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

                            return nullptr;
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
                        // return this->parseExpressionAndEnd();
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
                            ParseStructDecl(Context, Token, /*IsLValue=*/true,
                                            NameTokOpt);

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

                            return nullptr;
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
                            ParseUnionDecl(Context, Token, /*IsLValue=*/true,
                                           NameTokOpt);

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

                            return nullptr;
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
                            ParseShapeDecl(Context, Token, /*IsLValue=*/true,
                                           NameTokOpt);

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

                            return nullptr;
                        }

                        const auto NameTok = NameTokOpt.value();
                        const auto Name = TokenStream.tokenContent(NameTok);

                        return new AST::LvalueNamedDecl(Name, NameTok.Loc,
                                                        Result.value());
                    }
                    case Lex::Keyword::Interface:
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

                        return nullptr;
                    case Lex::Keyword::Or:
                        Diag.consume({
                            .Level = DiagnosticLevel::Error,
                            .Location = Token.Loc,
                            .Message = "Unexpected 'or' keyword"
                        });

                        return nullptr;
                    case Lex::Keyword::In:
                        Diag.consume({
                            .Level = DiagnosticLevel::Error,
                            .Location = Token.Loc,
                            .Message = "Unexpected 'in' keyword"
                        });

                        return nullptr;
                    case Lex::Keyword::While:
                        __builtin_unreachable();
                    case Lex::Keyword::Default:
                        Diag.consume({
                            .Level = DiagnosticLevel::Error,
                            .Location = Token.Loc,
                            .Message = "Unexpected 'default' keyword"
                        });

                        return nullptr;
                    case Lex::Keyword::As:
                        Diag.consume({
                            .Level = DiagnosticLevel::Error,
                            .Location = Token.Loc,
                            .Message = "Unexpected 'as' keyword"
                        });

                        return nullptr;
                }

                [[fallthrough]];
            }
            case Lex::TokenKind::OpenCurlyBrace:
                return ParseCompoundStmt(Context, Token);
            default:
                TokenStream.goBack();
                if (const auto Expr = ParseExpressionInPlaceOfStmt(Context)) {
                    return Expr;
                }

                return nullptr;
        }

        return nullptr;
    }
}
