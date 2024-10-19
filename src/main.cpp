/*
 * main.cpp
 */

#include <cassert>
#include <cstdio>

#include "AST/Decls/EnumDecl.h"
#include "AST/Decls/FunctionDecl.h"
#include "AST/Decls/LambdaDecl.h"
#include "AST/Decls/LvalueNamedDecl.h"
#include "AST/Decls/StructDecl.h"

#include "AST/BinaryOperation.h"
#include "AST/DeclRefExpr.h"

#include "Backend/LLVM/JIT.h"
#include "Basic/ArgvLexer.h"

#include "Interface/ANSI.h"
#include "Interface/Repl.h"

#include "Lex/Tokenizer.h"
#include "Parse/Parser.h"

static void PrintDepth(const uint8_t Depth) noexcept {
    for (auto I = uint8_t(); I != Depth; ++I) {
        printf("\t");
    }
}

static void
PrintTypeQualifiers(const char *const Prefix,
                    const Sema::TypeQualifiers Qual,
                    const char *const Suffix) noexcept
{
    printf("%s%s, %s%s",
           Prefix,
           Qual.isMutable() ? "mutable" : "immutable",
           Qual.isVolatile() ? "volatile" : "non-volatile",
           Suffix);
}

static void PrintTypeRefInstInfo(AST::TypeRef::Inst *const Inst) {
    switch (Inst->getKind()) {
        case AST::TypeRef::InstKind::Pointer: {
            const auto PtrInst = llvm::cast<AST::TypeRef::PointerInst>(Inst);
            const auto Qual = PtrInst->getQualifiers();

            PrintTypeQualifiers("Pointer<", Qual, ">");
            break;
        }
        case AST::TypeRef::InstKind::Type: {
            const auto TypeInst = llvm::cast<AST::TypeRef::TypeInst>(Inst);
            const auto Qual = TypeInst->getQualifiers();

            PrintTypeQualifiers("Type<", Qual, ">");
            break;
        }
        case AST::TypeRef::InstKind::Array: {
            printf("Array<>");
            break;
        }
        case AST::TypeRef::InstKind::Union: {
            printf("Union<>");
            break;
        }
        case AST::TypeRef::InstKind::Intersect:
            printf("Intersect<>");
            break;
    }
}

void
PrintAST(Backend::LLVM::Handler &Handler,
         AST::Stmt *const Stmt,
         const uint8_t Depth) noexcept
{
    if (Stmt == nullptr) {
        return;
    }

    PrintDepth(Depth);
    switch (Stmt->getKind()) {
        case AST::NodeKind::BinaryOperation: {
            const auto BinaryExpr = llvm::cast<AST::BinaryOperation>(Stmt);
            const auto Lexeme =
                Parse::BinaryOperatorToLexemeMap[BinaryExpr->getOperator()];

            printf("BinaryOperation<\"%s\">\n", Lexeme->data());

            PrintAST(Handler, &BinaryExpr->getLhs(), Depth + 1);
            PrintAST(Handler, &BinaryExpr->getRhs(), Depth + 1);

            break;
        }
        case AST::NodeKind::UnaryOperation: {
            const auto UnaryExpr = llvm::cast<AST::UnaryOperation>(Stmt);
            const auto Lexeme =
                Parse::UnaryOperatorToLexemeMap[UnaryExpr->getOperator()];

            printf("UnaryOperation<%s>\n", Lexeme->data());
            PrintAST(Handler, &UnaryExpr->getOperand(), Depth + 1);

            break;
        }
        case AST::NodeKind::CharLiteral: {
            const auto CharLit = llvm::cast<AST::CharLiteral>(Stmt);
            printf("CharLiteral<%c>\n", CharLit->getValue());

            break;
        }
        case AST::NodeKind::NumberLiteral: {
            const auto IntLit = llvm::cast<AST::NumberLiteral>(Stmt);
            switch (IntLit->getNumber().Kind) {
                case Parse::NumberKind::UnsignedInteger:
                    printf("NumberLiteral<%" PRIu64 ", unsigned>\n",
                           IntLit->getNumber().UInt);
                    break;
                case Parse::NumberKind::SignedInteger:
                    printf("NumberLiteral<%" PRId64 ", signed>\n",
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
            const auto StringLit = llvm::cast<AST::StringLiteral>(Stmt);
            printf("StringLiteral<" SV_FMT ">\n",
                   SV_FMT_ARG(StringLit->getValue()));

            break;
        }
        case AST::NodeKind::VarDecl: {
            const auto VarDecl = llvm::cast<AST::VarDecl>(Stmt);
            printf("VarDecl<\"" SV_FMT "\">\n",
                   SV_FMT_ARG(VarDecl->getName()));

            PrintAST(Handler, VarDecl->getTypeRef(), Depth + 1);
            PrintAST(Handler, VarDecl->getInitExpr(), Depth + 1);

            break;
        }
        case AST::NodeKind::Paren: {
            const auto ParenExpr = llvm::cast<AST::ParenExpr>(Stmt);

            printf("ParenExpr\n");
            PrintAST(Handler, ParenExpr->getChildExpr(), Depth + 1);

            break;
        }
        case AST::NodeKind::ParamVarDecl: {
            const auto Param = llvm::cast<AST::ParamVarDecl>(Stmt);
            printf("ParamDecl<\"" SV_FMT "\">\n",
                   SV_FMT_ARG(Param->getName()));

            PrintAST(Handler, Param->getTypeRef(), Depth + 1);
            break;
        }
        case AST::NodeKind::FunctionDecl: {
            const auto FuncDecl = llvm::cast<AST::FunctionDecl>(Stmt);
            printf("FunctionDecl\n");

            const auto &ParamList = FuncDecl->getParamList();
            for (const auto &Param : ParamList) {
                PrintAST(Handler, Param, Depth + 1);
            }

            PrintAST(Handler, FuncDecl->getBody(), Depth + 1);
            break;
        }
        case AST::NodeKind::DeclRefExpr: {
            const auto VarRef = llvm::cast<AST::DeclRefExpr>(Stmt);
            printf("DeclRefExpr<\"" SV_FMT "\">\n",
                   SV_FMT_ARG(VarRef->getName()));

            break;
        }
        case AST::NodeKind::CallExpr: {
            const auto CallExpr = llvm::cast<AST::CallExpr>(Stmt);
            printf("CallExpr\n");

            for (const auto &Arg : CallExpr->getArgs()) {
                PrintAST(Handler, Arg, Depth + 1);
            }

            PrintAST(Handler, CallExpr->getCallee(), Depth + 1);
            break;
        }
        case AST::NodeKind::IfStmt: {
            const auto IfStmt = llvm::cast<AST::IfStmt>(Stmt);
            printf("IfStmt\n");

            PrintAST(Handler, IfStmt->getCond(), Depth + 1);
            PrintAST(Handler, IfStmt->getThen(), Depth + 2);

            printf("\tElseStmt\n");
            PrintAST(Handler, IfStmt->getElse(), Depth + 2);

            break;
        }
        case AST::NodeKind::ReturnStmt: {
            const auto ReturnStmt = llvm::cast<AST::ReturnStmt>(Stmt);

            printf("ReturnStmt\n");
            PrintAST(Handler, ReturnStmt->getValue(), Depth + 1);

            break;
        }
        case AST::NodeKind::CompoundStmt: {
            const auto CompoundStmt = llvm::cast<AST::CompoundStmt>(Stmt);

            printf("CompoundStmt\n");
            for (const auto &Stmt : CompoundStmt->getStmtList()) {
                PrintAST(Handler, Stmt, Depth + 1);
            }

            break;
        }
        case AST::NodeKind::TypeRef: {
            const auto TypeRef = llvm::cast<AST::TypeRef>(Stmt);

            printf("TypeRef\n");
            PrintDepth(Depth + 1);

            const auto &InstList = TypeRef->getInstList();
            printf("Instructions: [");

            if (!InstList.empty()) {
                PrintTypeRefInstInfo(InstList.front());
                for (auto It = std::next(InstList.begin()),
                        End = InstList.end();
                     It != End;
                     ++It)
                {
                    printf(", ");
                    PrintTypeRefInstInfo(*It);
                }

                printf("]\n");
            } else {
                printf("] (empty)\n");
            }

            break;
        }
        case AST::NodeKind::StructDecl: {
            const auto StructDecl = llvm::cast<AST::StructDecl>(Stmt);
            printf("StructDecl\n");

            for (const auto Field : StructDecl->getFieldList()) {
                PrintAST(Handler, Field, Depth + 1);
            }

            break;
        }
        case AST::NodeKind::FieldDecl: {
            const auto FieldDecl = llvm::cast<AST::FieldDecl>(Stmt);
            printf("FieldDecl<\"" SV_FMT "\">\n",
                   SV_FMT_ARG(FieldDecl->getName()));

            PrintAST(Handler, FieldDecl->getTypeRef(), Depth + 1);
            break;
        }
        case AST::NodeKind::FieldExpr: {
            const auto FieldExpr = llvm::cast<AST::FieldExpr>(Stmt);
            printf("MemberAccessExpr<\'%s\', \"" SV_FMT "\">\n",
                   FieldExpr->isArrow() ? "->" : ".",
                   SV_FMT_ARG(FieldExpr->getMemberName()));

            PrintAST(Handler, FieldExpr->getBase(), Depth + 1);
            break;
        }
        case AST::NodeKind::ArraySubscriptExpr: {
            const auto ArraySubscriptExpr =
                llvm::cast<AST::ArraySubscriptExpr>(Stmt);

            printf("ArraySubscriptExpr\n");

            PrintAST(Handler, ArraySubscriptExpr->getBase(), Depth + 1);
            PrintAST(Handler,
                     ArraySubscriptExpr->getBracketedExpr(),
                     Depth + 1);

            break;
        }

        case AST::NodeKind::EnumMemberDecl: {
            const auto EnumMemberDecl = llvm::cast<AST::EnumMemberDecl>(Stmt);
            printf("EnumMemberDecl<\"" SV_FMT "\">\n",
                   SV_FMT_ARG(EnumMemberDecl->getName()));

            PrintAST(Handler, EnumMemberDecl->getInitExpr(), Depth + 1);
            break;
        }
        case AST::NodeKind::EnumDecl: {
            const auto EnumDecl = llvm::cast<AST::EnumDecl>(Stmt);
            printf("EnumDecl\n");

            for (const auto &EnumMember : EnumDecl->getMemberList()) {
                PrintAST(Handler, EnumMember, Depth + 1);
            }

            break;
        }
        case AST::NodeKind::ArrayDecl: {
            const auto ArrayDecl = llvm::cast<AST::ArrayDecl>(Stmt);
            printf("ArrayDecl\n");

            for (const auto &Element : ArrayDecl->getElementList()) {
                PrintAST(Handler, Element, Depth + 1);
            }

            break;
        }
        case AST::NodeKind::LambdaDecl: {
            const auto LambdaDecl = llvm::cast<AST::LambdaDecl>(Stmt);
            printf("LambdaDecl\n");

            PrintAST(Handler, LambdaDecl->getCaptureList(), Depth + 1);
            for (const auto &Param : LambdaDecl->getParamList()) {
                PrintAST(Handler, Param, Depth + 1);
            }

            break;
        }
        case AST::NodeKind::LvalueNamedDecl: {
            const auto LvalueNamedDecl = llvm::cast<AST::LvalueNamedDecl>(Stmt);
            printf("LvalueNamedDecl<\"" SV_FMT "\">\n",
                   SV_FMT_ARG(LvalueNamedDecl->getName()));

            PrintAST(Handler, LvalueNamedDecl->getRvalueExpr(), Depth + 1);
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
            fprintf(stdout, "\t" SV_FMT "\n",
                    SV_FMT_ARG(Lex::TokenKindGetName(Token.Kind)));
        }

        fputc('\n', stdout);
    }

    auto SourceMngr = SourceManager::fromString(Prompt);
    Diag.setSourceManager(SourceMngr);

    auto Parser =
        Parse::Parser(*SourceMngr, Context, TokenList, Diag, ParseOptions);

    auto Expr = Parser.parseTopLevelExpressionOrStmt();
    if (Expr == nullptr) {
        return;
    }

    if (ArgOptions.PrintAST) {
        PrintAST(*BackendHandler, Expr, /*Depth=*/ArgOptions.PrintDepth);
        fputc('\n', stdout);
    }

    const auto Result =
        BackendHandler->evaluateAndPrint(*Expr,
                                          ArgOptions.PrintIR,
                                          BHGRN "Evaluation> " CRESET,
                                          "\n");

    if (!Result) {
        if (const auto Decl = llvm::dyn_cast<AST::LvalueNamedDecl>(Expr)) {
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

        if (ArgOptions.PrintAST) {
            const auto &DeclMap =
                Context.getSymbolTable().getGlobalScope().getDeclMap();

            for (const auto &[Name, Decl] : DeclMap) {
                PrintAST(BackendHandler, Decl, /*Depth=*/ArgOptions.PrintDepth);
            }
        }

        //Context.visitDecls(Diag, AST::Context::VisitOptions());
        BackendHandler.evaluate(Context);
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

        if (Arg == "-h" || Arg == "--help"
         || Arg == "-u" || Arg == "--usage")
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
