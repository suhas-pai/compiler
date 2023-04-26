/*
 * main.cpp
 */

#include <cassert>
#include <cstdio>

#include <inttypes.h>

#include "AST/BinaryOperation.h"
#include "Backend/LLVM/JIT.h"
#include "Lex/Tokenizer.h"
#include "Parse/Parser.h"

static void
PrintResultIfAvailable(Backend::LLVM::Handler &Handler,
                       AST::Expr &Expr) noexcept
{
    fputs(" (Result: ", stdout);
    Handler.evalulateAndPrint(Expr);
    fputs(")\n", stdout);
}

void
PrintAST(Backend::LLVM::Handler &Handler,
         AST::Expr *const Expr,
         const uint8_t Depth) noexcept
{
    for (auto I = uint8_t(); I != Depth; ++I) {
        printf("\t");
    }

    switch (Expr->getKind()) {
        case AST::ExprKind::Base:
            assert(false && "Got Expr-Base while printing AST");
        case AST::ExprKind::BinaryOperation: {
            const auto BinaryExpr = static_cast<AST::BinaryOperation *>(Expr);
            const auto Lexeme =
                Parse::BinaryOperatorToLexemeMap[BinaryExpr->getOperator()];

            printf("binary-operation<%s>", Lexeme->data());
            PrintResultIfAvailable(Handler, *Expr);

            PrintAST(Handler, BinaryExpr->getLhs(), Depth + 1);
            PrintAST(Handler, BinaryExpr->getRhs(), Depth + 1);

            break;
        }
        case AST::ExprKind::UnaryOperation: {
            const auto UnaryExpr = static_cast<AST::UnaryOperation *>(Expr);
            const auto Lexeme =
                Parse::UnaryOperatorToLexemeMap[UnaryExpr->getOperator()];

            printf("unary-operation<%s>", Lexeme->data());
            PrintResultIfAvailable(Handler, *Expr);

            PrintAST(Handler, UnaryExpr->getOperand(), Depth + 1);
            break;
        }
        case AST::ExprKind::CharLiteral: {
            const auto CharLit = static_cast<AST::CharLiteral *>(Expr);
            printf("char-literal<%c>\n", CharLit->getValue());

            break;
        }
        case AST::ExprKind::IntegerLiteral: {
            const auto IntLit = static_cast<AST::NumberLiteral *>(Expr);
            switch (IntLit->getNumber().Kind) {
                case Parse::NumberKind::UnsignedInteger:
                    printf("integer-literal<%llu>\n", IntLit->getNumber().UInt);
                    break;
                case Parse::NumberKind::SignedInteger:
                    printf("integer-literal<%lld>\n", IntLit->getNumber().SInt);
                    break;
                case Parse::NumberKind::FloatingPoint32:
                case Parse::NumberKind::FloatingPoint64:
                    assert(false && "Floating point literals not supported");
            }

            break;
        }
        case AST::ExprKind::FloatLiteral:
            assert(false && "Got float-literal while printing AST");
        case AST::ExprKind::StringLiteral: {
            const auto StringLit = static_cast<AST::StringLiteral *>(Expr);
            printf("string-literal<%s>\n", StringLit->getValue().data());

            break;
        }
        case AST::ExprKind::VarDecl: {
            const auto VarDecl = static_cast<AST::VarDecl *>(Expr);

            printf("var-decl<\"%s\">", VarDecl->getName().data());
            PrintResultIfAvailable(Handler, *Expr);
            PrintAST(Handler, VarDecl->getInitExpr(), Depth + 1);

            break;
        }
        case AST::ExprKind::Paren: {
            const auto ParenExpr = static_cast<AST::ParenExpr *>(Expr);

            printf("paren-expr\n");
            PrintAST(Handler, ParenExpr->getChildExpr(), Depth + 1);

            break;
        }
        case AST::ExprKind::FunctionDecl:
        case AST::ExprKind::FunctionPrototype:
        case AST::ExprKind::Parameter:
            break;
    }
}

int main(int argc, const char *const argv[]) {
    if (argc != 2) {
        printf("Usage: %s <prompt>\n", argv[0]);
        return 1;
    }

    const char *const Prompt = argv[1];

    auto Tokenizer = Lex::Tokenizer(Prompt);
    auto TokenList = std::vector<Lex::Token>();

    while (true) {
        const auto Token = Tokenizer.next();
        if (Token.Kind == Lex::TokenKind::EOFToken) {
            break;
        }

        TokenList.emplace_back(std::move(Token));
    }

    for (const auto &Token : TokenList) {
        printf("%s\n", Lex::TokenKindGetName(Token.Kind).data());
    }

    auto Parser = Parse::Parser(Prompt, TokenList);
    auto Expr = Parser.startParsing();

    auto BackendHandler = Backend::LLVM::JITHandler::Create();
    if (BackendHandler == nullptr) {
        printf("Failed to create JITHandler\n");
        return 1;
    }

    PrintAST(*BackendHandler, Expr, /*Depth=*/0);
    return 0;
}