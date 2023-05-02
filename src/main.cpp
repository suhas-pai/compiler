/*
 * main.cpp
 */

#include <cassert>
#include <cstdio>
#include <inttypes.h>

#include "AST/BinaryOperation.h"
#include "AST/VariableRef.h"

#include "Backend/LLVM/JIT.h"
#include "Basic/ArgvLexer.h"

#include "Interface/Repl.h"

#include "Lex/Tokenizer.h"
#include "Parse/Parser.h"

void
PrintAST(Backend::LLVM::Handler &Handler,
         AST::Stmt *const Expr,
         const uint8_t Depth) noexcept
{
    for (auto I = uint8_t(); I != Depth; ++I) {
        printf("\t");
    }

    switch (Expr->getKind()) {
        case AST::NodeKind::Base:
            assert(false && "Got Expr-Base while printing AST");
        case AST::NodeKind::BinaryOperation: {
            const auto BinaryExpr = llvm::cast<AST::BinaryOperation>(Expr);
            const auto Lexeme =
                Parse::BinaryOperatorToLexemeMap[BinaryExpr->getOperator()];

            printf("binary-operation<%s>\n", Lexeme->data());

            PrintAST(Handler, BinaryExpr->getLhs(), Depth + 1);
            PrintAST(Handler, BinaryExpr->getRhs(), Depth + 1);

            break;
        }
        case AST::NodeKind::UnaryOperation: {
            const auto UnaryExpr = llvm::cast<AST::UnaryOperation>(Expr);
            const auto Lexeme =
                Parse::UnaryOperatorToLexemeMap[UnaryExpr->getOperator()];

            printf("unary-operation<%s>\n", Lexeme->data());
            PrintAST(Handler, UnaryExpr->getOperand(), Depth + 1);

            break;
        }
        case AST::NodeKind::CharLiteral: {
            const auto CharLit = llvm::cast<AST::CharLiteral>(Expr);
            printf("char-literal<%c>\n", CharLit->getValue());

            break;
        }
        case AST::NodeKind::NumberLiteral: {
            const auto IntLit = llvm::cast<AST::NumberLiteral>(Expr);
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
        case AST::NodeKind::FloatLiteral:
            assert(false && "Got float-literal while printing AST");
        case AST::NodeKind::StringLiteral: {
            const auto StringLit = llvm::cast<AST::StringLiteral>(Expr);
            printf("string-literal<%s>\n", StringLit->getValue().data());

            break;
        }
        case AST::NodeKind::VarDecl: {
            const auto VarDecl = llvm::cast<AST::VarDecl>(Expr);

            printf("var-decl<\"%s\">\n", VarDecl->getName().data());
            PrintAST(Handler, VarDecl->getInitExpr(), Depth + 1);

            break;
        }
        case AST::NodeKind::Paren: {
            const auto ParenExpr = llvm::cast<AST::ParenExpr>(Expr);

            printf("paren-expr\n");
            PrintAST(Handler, ParenExpr->getChildExpr(), Depth + 1);

            break;
        }
        case AST::NodeKind::FunctionDecl: {
            const auto FuncDecl = llvm::cast<AST::FunctionDecl>(Expr);
            printf("function-decl\n");

            PrintAST(Handler, FuncDecl->getPrototype(), Depth + 1);
            PrintAST(Handler, FuncDecl->getBody(), Depth + 2);

            break;
        }
        case AST::NodeKind::FunctionPrototype: {
            const auto FuncProt = llvm::cast<AST::FunctionPrototype>(Expr);
            printf("function-prototype<\"%s\", Args=[",
                   FuncProt->getName().data());

            for (const auto &Param : FuncProt->getParamList()) {
                printf("\"%s\"", Param.getName().data());
                if (&Param != &FuncProt->getParamList().back()) {
                    printf(", ");
                }
            }

            printf("]>\n");
            break;
        }
        case AST::NodeKind::VariableRef: {
            const auto VarRef = llvm::cast<AST::VariableRef>(Expr);
            printf("var-ref<\"%s\">\n", VarRef->getName().data());

            break;
        }
        case AST::NodeKind::FunctionCall: {
            const auto FuncCall = llvm::cast<AST::FunctionCall>(Expr);
            printf("function-call<\"%s\">\n", FuncCall->getName().data());

            for (const auto &Arg : FuncCall->getArgs()) {
                PrintAST(Handler, Arg, Depth + 1);
            }

            break;
        }
    }
}

struct ArgumentOptions {
    bool PrintTokens : 1 = false;
    bool PrintAST : 1 = false;
    bool PrintIR : 1 = false;

    uint64_t PrintDepth = 0;
};

void
HandlePrompt(const std::string_view &Prompt,
             Backend::LLVM::Handler &BackendHandler,
             const Parse::ParserOptions &ParseOptions,
             const ArgumentOptions ArgOptions) noexcept
{
    auto Diag = BackendHandler.getDiag();
    auto Tokenizer = Lex::Tokenizer(Prompt, *Diag);
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

        fputc('\n', stdout);
    }

    auto SourceMngr = SourceManager::fromString(Prompt);
    Diag->setSourceManager(SourceMngr);

    auto Parser = Parse::Parser(*SourceMngr, TokenList, *Diag, ParseOptions);
    auto Expr = Parser.startParsing();

    // We got an error while parsing.
    if (Expr == nullptr) {
        return;
    }

    if (ArgOptions.PrintAST) {
        PrintAST(BackendHandler, Expr, /*Depth=*/ArgOptions.PrintDepth);
        fputc('\n', stdout);
    }

    BackendHandler.evalulateAndPrint(*Expr,
                                     ArgOptions.PrintIR,
                                     "Evaluated to ",
                                     "\n");
}

void PrintUsage(const char *const Name) noexcept {
    fprintf(stdout,
            "Usage: %s [<prompt>] [-h/--help/-u/--usage] [--print-tokens] "
            "[--print-ast]\n",
            Name);
}

void
HandleReplOption(const ArgumentOptions ArgOptions,
                 Backend::LLVM::Handler &BackendHandler) noexcept
{
    auto ParserOptions = Parse::ParserOptions();

    ParserOptions.DontRequireSemicolons = true;
    ParserOptions.ParseTopLevelExpressionsAsStmts = true;

    Interface::SetupRepl("Compiler",
                         [&](const std::string_view Input) noexcept {
                            HandlePrompt(Input,
                                         BackendHandler,
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
        }

        if (Option == "--print-ast") {
            Options.PrintAST = true;
            Lexer.consume();

            continue;
        }

        if (Option == "--print-ir") {
            Options.PrintIR = true;
            Lexer.consume();

            continue;
        }

        fprintf(stderr, "Unrecognized option: %s\n", Option.data());
        return 1;
    }

    auto Diag = Interface::DiagnosticsEngine();
    const auto BackendHandler = Backend::LLVM::JITHandler::Create(&Diag);

    if (BackendHandler == nullptr) {
        printf("Failed to create JITHandler\n");
        exit(1);
    }

    if (Prompt.empty()) {
        HandleReplOption(Options, *BackendHandler);
        return 0;
    }

    HandlePrompt(Prompt, *BackendHandler, Parse::ParserOptions(), Options);
    return 0;
}