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

            const auto Stmt = ParseStmt(Context);
            if (!Stmt.has_value()) {
                return nullptr;
            }

            if (!ExpectSemicolon(Context)) {
                return nullptr;
            }

            StmtList.emplace_back(Stmt.value());
        }

        return new AST::CompoundStmt(CurlyToken.Loc, std::move(StmtList));
    }

    auto ParseReturnStmt(ParseContext &Context, Lex::Token ReturnToken) noexcept
        -> std::expected<AST::ReturnStmt *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        const auto Expr = ParseExpression(Context);
        if (Expr != nullptr) {
            if (ExpectSemicolon(Context)) {
                return new AST::ReturnStmt(ReturnToken.Loc, Expr);
            }
        }

        const auto CurrentOpt = TokenStream.current();
        Diag.consume({
            .Level = DiagnosticLevel::Error,
            .Location = TokenStream.getCurrOrPrevLoc(),
            .Message =
                std::format("Expected ';', found {} instead",
                            CurrentOpt.has_value() ?
                                TokenStream.tokenContent(CurrentOpt.value()) :
                                "end of file")
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

        auto Qualifiers = AST::Qualifiers();
        ParseQualifiers(Context, Qualifiers);

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
                        return ParseVarDecl(Context, Qualifiers);
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

                        const auto NameTok = NameTokOpt.value();
                        const auto Result = ResultOpt.value();
                        const auto Name = TokenStream.tokenContent(NameTok);

                        return new AST::LvalueNamedDecl(Name, NameTok.Loc,
                                                        Result);
                    }
                    case Lex::Keyword::If:
                        //return this->parseIfStmt(Token);
                    case Lex::Keyword::Else:
                        // return this->parseExpressionAndEnd();
                    case Lex::Keyword::Return:
                        return ParseReturnStmt(Context, Token).value();
                    case Lex::Keyword::Struct:
                    case Lex::Keyword::Class:
                    case Lex::Keyword::Interface:
                    case Lex::Keyword::Impl: {
                        auto Name = std::optional<Lex::Token>(std::nullopt);
                        return ParseStructDecl(Context, Name);
                    }
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
                if (const auto Expr = ParseExpression(Context)) {
                    return Expr;
                }

                return nullptr;
        }

        return nullptr;
    }
}
