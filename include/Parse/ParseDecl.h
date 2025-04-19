/*
 * Parse/ParseDecl.h
 * © suhas pai
 */

#pragma once

#include "AST/Decls/ClosureDecl.h"
#include "AST/Decls/FunctionDecl.h"
#include "AST/Decls/InterfaceDecl.h"
#include "AST/Decls/ShapeDecl.h"
#include "AST/Decls/StructDecl.h"
#include "AST/Decls/UnionDecl.h"

#include "AST/Qualifiers.h"

#include "Parse/Context.h"
#include "Parse/ParseError.h"

namespace Parse {
    auto
    ParseClosureDecl(ParseContext &Context,
                     const Lex::Token BracketToken) noexcept
        -> std::expected<AST::ClosureDecl *, ParseError>;

    [[nodiscard]] auto
    ParseFunctionDecl(ParseContext &Context,
                      Lex::Token ParenToken,
                      std::optional<Lex::Token> &NameTokenOptOut) noexcept
        -> std::expected<AST::FunctionDecl *, ParseError>;

    [[nodiscard]] auto
    ParseArrowFunctionDeclOrFunctionType(ParseContext &Context,
                                         Lex::Token ParenToken) noexcept
        -> std::expected<AST::Expr *, ParseError>;

    [[nodiscard]] auto
    ParseInterfaceDecl(ParseContext &Context,
                       Lex::Token InterfaceKeywordToken,
                       std::optional<Lex::Token> &NameTokenOptOut) noexcept
        -> std::expected<AST::InterfaceDecl *, ParseError>;

    [[nodiscard]] auto
    ParseShapeDecl(ParseContext &Context,
                   Lex::Token StructKeywordToken,
                   std::optional<Lex::Token> &NameTokenOptOut) noexcept
        -> std::expected<AST::ShapeDecl *, ParseError>;

    [[nodiscard]] auto
    ParseStructDecl(ParseContext &Context,
                    Lex::Token StructKeywordToken,
                    std::optional<Lex::Token> &NameTokenOptOut) noexcept
        -> std::expected<AST::StructDecl *, ParseError>;

    [[nodiscard]] auto
    ParseUnionDecl(ParseContext &Context,
                   Lex::Token UnionKeywordToken,
                   std::optional<Lex::Token> &NameTokenOptOut) noexcept
        -> std::expected<AST::UnionDecl *, ParseError>;

    [[nodiscard]] auto
    ParseVarDecl(ParseContext &Context,
                 const AST::Qualifiers &PreIntroducerQualifiers) noexcept
        -> std::expected<AST::Stmt *, ParseError>;
}
