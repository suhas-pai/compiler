/*
 * Parse/ParseDecl.h
 * © suhas pai
 */

#pragma once

#include "AST/Decls/DeclStmt.h"
#include "AST/Decls/FunctionDecl.h"
#include "AST/Decls/StructDecl.h"

#include "AST/Qualifiers.h"

#include "Parse/Context.h"
#include "Parse/ParseError.h"

namespace Parse {
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
    ParseStructDecl(ParseContext &Context,
                    std::optional<Lex::Token> &NameTokenOptOut) noexcept
        -> std::expected<AST::StructDecl *, ParseError>;

    [[nodiscard]] auto
    ParseVarDecl(ParseContext &Context,
                 const AST::Qualifiers &PreQualifiers) noexcept
        -> std::expected<AST::DeclStmt *, ParseError>;
}
