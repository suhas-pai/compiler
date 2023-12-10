/*
 * main.cpp
 */

#include <cassert>
#include <cstdio>

#include "AST/BinaryOperation.h"
#include "AST/CompoundStmt.h"
#include "AST/NodeKind.h"
#include "AST/VariableRef.h"

#include "Backend/LLVM/JIT.h"
#include "Basic/ArgvLexer.h"

#include "Interface/ANSI.h"
#include "Interface/Repl.h"

#include "Lex/Tokenizer.h"
#include "Parse/Parser.h"

void
PrintAST(Backend::LLVM::Handler &Handler,
         AST::Stmt *const Expr,
         const uint8_t Depth) noexcept
{
    if (Expr == nullptr) {
        return;
    }

    for (auto I = uint8_t(); I != Depth; ++I) {
        printf("\t");
    }

    switch (Expr->getKind()) {
        case AST::NodeKind::Base:
            __builtin_unreachable();
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
            PrintAST(Handler, &UnaryExpr->getOperand(), Depth + 1);

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
        case AST::NodeKind::IfStmt: {
            const auto IfStmt = llvm::cast<AST::IfStmt>(Expr);
            printf("if-stmt\n");

            PrintAST(Handler, IfStmt->getCond(), Depth + 1);
            PrintAST(Handler, IfStmt->getThen(), Depth + 2);
            PrintAST(Handler, IfStmt->getElse(), Depth + 1);

            break;
        }
        case AST::NodeKind::ReturnStmt: {
            const auto ReturnStmt = llvm::cast<AST::ReturnStmt>(Expr);

            printf("return-stmt\n");
            PrintAST(Handler, ReturnStmt->getValue(), Depth + 1);

            break;
        }
        case AST::NodeKind::CompountStmt: {
            const auto CompoundStmt = llvm::cast<AST::CompoundStmt>(Expr);

            printf("compound-stmt\n");
            for (const auto &Stmt : CompoundStmt->getStmtList()) {
                PrintAST(Handler, Stmt, Depth + 1);
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
             AST::Context &Context,
             const Parse::ParserOptions &ParseOptions,
             const ArgumentOptions ArgOptions) noexcept
{
    auto Diag = Interface::DiagnosticsEngine();
    auto BackendHandler = Backend::LLVM::JITHandler::create(Diag, Context);
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

        fputc('\n', stdout);
    }

    auto SourceMngr = SourceManager::fromString(Prompt);
    Diag.setSourceManager(SourceMngr);

    auto Parser =
        Parse::Parser(*SourceMngr, Context, TokenList, Diag, ParseOptions);

    // We got an error while parsing.
    auto Expr = Parser.parseTopLevelExpressionOrStmt();
    if (Expr == nullptr) {
        return;
    }

    if (ArgOptions.PrintAST) {
        PrintAST(*BackendHandler, Expr, /*Depth=*/ArgOptions.PrintDepth);
        fputc('\n', stdout);
    }

    const auto Result =
        BackendHandler->evalulateAndPrint(*Expr,
                                          ArgOptions.PrintIR,
                                          BHGRN "Evaluation> " CRESET,
                                          "\n");

    if (!Result) {
        if (const auto Decl = llvm::dyn_cast<AST::Decl>(Expr)) {
            Context.removeDecl(Decl);
        }
    }
}

void PrintUsage(const char *const Name) noexcept {
    fprintf(stdout,
            "Usage: %s [<prompt>] [-h/--help/-u/--usage] [--print-tokens] "
            "[--print-ast]\n",
            Name);
}

void HandleReplOption(const ArgumentOptions ArgOptions) {
    auto Context = AST::Context();
    auto Diag = Interface::DiagnosticsEngine();
    auto ParserOptions = Parse::ParserOptions();

    ParserOptions.DontRequireSemicolons = true;
    Interface::SetupRepl("Compiler",
                         [&](const std::string_view Input) noexcept {
                            HandlePrompt(Input,
                                         Context,
                                         ParserOptions,
                                         ArgOptions);
                            return true;
                         });
}

void
HandleFileOptions(const ArgumentOptions ArgOptions,
                  const std::vector<std::string_view> &FilePaths) noexcept
{
    auto Diag = Interface::DiagnosticsEngine();
    for (const auto Path : FilePaths) {
        const auto SrcMngrOpt = SourceManager::fromFile(Path);
        if (const auto SrcMngrError = SrcMngrOpt.getError()) {
            switch (SrcMngrError->Kind) {
                case SourceManager::Error::ErrorKind::FailedToOpenFile: {
                    fprintf(stderr,
                            "Failed to open file: %s, reason: %s\n",
                            Path.data(),
                            SrcMngrError->OpenError->Reason.c_str());
                    exit(1);
                }
                case SourceManager::Error::ErrorKind::FailedToStatFile:
                    fprintf(stderr,
                            "Failed to stat file: %s, reason: %s\n",
                            Path.data(),
                            SrcMngrError->OpenError->Reason.c_str());
                    exit(1);
                case SourceManager::Error::ErrorKind::FailedToMapFile:
                    fprintf(stderr,
                            "Failed to map file: %s, reason: %s\n",
                            Path.data(),
                            SrcMngrError->OpenError->Reason.c_str());
                    exit(1);
            }
        }

        const auto SrcMngr = SrcMngrOpt.getResult();
        const auto TokensOpt =
            Lex::Tokenizer(SrcMngr->getText(), Diag).createList();

        if (!TokensOpt.has_value()) {
            exit(1);
        }

        const auto TokenList = TokensOpt.value();

        auto Context = AST::Context();
        auto BackendHandler = Backend::LLVM::Handler(Diag);
        auto Parser = Parse::Parser(*SrcMngr, Context, TokenList, Diag);

        if (!Parser.startParsing()) {
            exit(1);
        }

        Context.visitDecls(Diag, AST::Context::VisitOptions());
        BackendHandler.evalulate(Context);

        if (ArgOptions.PrintAST) {
            for (const auto &[Name, Decl] : Context.getDeclMap()) {
                PrintAST(BackendHandler, Decl, /*Depth=*/ArgOptions.PrintDepth);
            }
        }

        if (ArgOptions.PrintIR) {
            BackendHandler.getModule().print(llvm::outs(), nullptr);
        }
    }
}

int main(const int argc, const char *const argv[]) {
    auto Options = ArgumentOptions();
    auto FilePaths = std::vector<std::string_view>();
    auto Lexer = ArgvLexer(argc, argv);

    while (const auto ArgOpt = Lexer.peek()) {
        const auto Arg = std::string_view(ArgOpt);
        if (!Arg.starts_with("-")) {
            FilePaths.push_back(Arg);
            Lexer.consume();

            break;
        }

        if (Arg == "-h" || Arg == "--help" ||
            Arg == "-u" || Arg == "--usage")
        {
            PrintUsage(argv[0]);
            return 0;
        }

        if (Arg == "--print-tokens") {
            Options.PrintTokens = true;
            Lexer.consume();

            continue;
        }

        if (Arg == "--print-ast") {
            Options.PrintAST = true;
            Lexer.consume();

            continue;
        }

        if (Arg == "--print-ir") {
            Options.PrintIR = true;
            Lexer.consume();

            continue;
        }

        fprintf(stderr, "Unrecognized option: %s\n", Arg.data());
        return 1;
    }

    while (const auto Path = Lexer.consume()) {
        FilePaths.push_back(Path);
    }

    if (FilePaths.empty()) {
        HandleReplOption(Options);
        return 0;
    }

    HandleFileOptions(Options, FilePaths);
    return 0;
}