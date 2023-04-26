/*
 * main.cpp
 */

#include <cassert>
#include <cstdio>

#include <inttypes.h>
#include "AST/BinaryOperation.h"

#include "Backend/LLVM/JIT.h"
#include "Basic/ArgvLexer.h"

#include "Interface/DiagnosticsEngine.h"
#include "Interface/Repl.h"

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
                    printf("number-literal<%llu, unsigned>\n",
                           IntLit->getNumber().UInt);
                    break;
                case Parse::NumberKind::SignedInteger:
                    printf("number-literal<%lld, signed>\n",
                           IntLit->getNumber().SInt);
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

struct ArgumentOptions {
    bool PrintTokens : 1 = false;
    uint64_t PrintDepth = 0;
};

void
HandlePrompt(const std::string_view &Prompt,
             Interface::DiagnosticsEngine &Diag,
             const Parse::ParserOptions &ParseOptions,
             const ArgumentOptions ArgOptions) noexcept
{
    auto Tokenizer = Lex::Tokenizer(Prompt, Diag);
    auto TokenList = std::vector<Lex::Token>();

    while (true) {
        const auto Token = Tokenizer.next();
        if (Token.Kind == Lex::TokenKind::EOFToken) {
            break;
        }

        if (Token.Kind == Lex::TokenKind::Invalid) {
            return;
        }

        TokenList.emplace_back(std::move(Token));
    }

    if (ArgOptions.PrintTokens) {
        fputs("Tokens:\n", stdout);
        for (const auto &Token : TokenList) {
            fprintf(stdout, "\t%s\n", Lex::TokenKindGetName(Token.Kind).data());
        }
    }

    auto Parser =
        Parse::Parser(std::string(Prompt), TokenList, Diag, ParseOptions);

    auto Expr = Parser.startParsing();

    // We got an error while parsing.
    if (Expr == nullptr) {
        return;
    }

    const auto BackendHandler = Backend::LLVM::JITHandler::Create();
    if (BackendHandler == nullptr) {
        printf("Failed to create JITHandler\n");
        exit(1);
    }

    PrintAST(*BackendHandler, Expr, /*Depth=*/ArgOptions.PrintDepth);

    const auto Value = Expr->codegen(*BackendHandler);
    if (const auto Constant = llvm::dyn_cast<llvm::ConstantFP>(Value)) {
        printf("\nEvaluated to %f\n", Constant->getValue().convertToDouble());
    }
}

void PrintUsage(const char *const Name) noexcept {
    fprintf(stdout,
            "Usage: %s [<prompt>] [-h/--help/-u/--usage] [--print-tokens]\n",
            Name);
}

void HandleReplOption(const ArgumentOptions ArgOptions) noexcept {
    auto Diag = Interface::DiagnosticsEngine();
    auto ParserOptions = Parse::ParserOptions();

    ParserOptions.DontRequireSemicolons = true;
    Interface::SetupRepl("Compiler",
                         [&](const std::string_view Input) noexcept {
                            HandlePrompt(Input,
                                         Diag,
                                         ParserOptions,
                                         ArgOptions);
                            return true;
                         });
}

int main(const int argc, const char *const argv[]) {
    auto Options = ArgumentOptions();
    auto Prompt = std::string_view();
    auto Lexer = ArgvLexer(argc, argv);

    while (const auto OptionOpt = Lexer.peek()) {
        const auto Option = std::string_view(OptionOpt);
        if (!Option.starts_with("-")) {
            Prompt = Option.data();
            break;
        }

        if (Option == "-h" || Option == "--help" ||
            Option == "-u" || Option == "--usage")
        {
            PrintUsage(argv[0]);
            return 0;
        }

        if (Option == "--print-tokens") {
            Options.PrintTokens = true;
            Lexer.consume();

            continue;
        } else {
            fprintf(stderr, "Unrecognized option: %s\n", Option.data());
            return 1;
        }
    }

    if (Prompt.empty()) {
        HandleReplOption(Options);
        return 0;
    }

    auto Diag = Interface::DiagnosticsEngine();
    HandlePrompt(Prompt, Diag, Parse::ParserOptions(), Options);

    return 0;
}