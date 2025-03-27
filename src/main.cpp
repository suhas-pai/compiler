/*
 * main.cpp
 */

#include <cassert>
#include <cstdio>

#include "AST/Decls/ArrayDecl.h"
#include "AST/Decls/ArrayDestructredVarDecl.h"
#include "AST/Decls/ClosureDecl.h"
#include "AST/Decls/ObjectDestructuredVarDecl.h"
#include "AST/Decls/EnumDecl.h"
#include "AST/Decls/EnumMemberDecl.h"
#include "AST/Decls/FieldDecl.h"
#include "AST/Decls/FunctionDecl.h"
#include "AST/Decls/OptionalFieldDecl.h"
#include "AST/Decls/ParamVarDecl.h"
#include "AST/Decls/ShapeDecl.h"
#include "AST/Decls/StructDecl.h"
#include "AST/Decls/VarDecl.h"

#include "AST/Types/ArrayType.h"
#include "AST/Types/FunctionType.h"
#include "AST/Types/OptionalType.h"
#include "AST/Types/PointerType.h"

#include "AST/ArraySubscriptExpr.h"
#include "AST/BinaryOperation.h"
#include "AST/CallExpr.h"
#include "AST/CastExpr.h"
#include "AST/CharLiteral.h"
#include "AST/CommaSepStmtList.h"
#include "AST/CompoundStmt.h"
#include "AST/DeclRefExpr.h"
#include "AST/DerefExpr.h"
#include "AST/FieldExpr.h"
#include "AST/ForStmt.h"
#include "AST/IfExpr.h"
#include "AST/OptionalUnwrapExpr.h"
#include "AST/NumberLiteral.h"
#include "AST/ParenExpr.h"
#include "AST/ReturnStmt.h"
#include "AST/StringLiteral.h"
#include "AST/UnaryOperation.h"

#include "Backend/LLVM/Handler.h"
#include "Backend/LLVM/JIT.h"

#include "Basic/ArgvLexer.h"
#include "Diag/Consumer.h"
#include "Lex/TokenBuffer.h"

#include "Misc/Repl.h"
#include "Parse/ParseUnit.h"
#include "Source/SourceBuffer.h"

static void PrintDepth(const uint8_t Depth) noexcept {
    for (auto I = uint8_t(); I != Depth; ++I) {
        std::print("    ");
    }
}

static void
PrintAST(Backend::LLVM::Handler &Handler,
         AST::Stmt *const Stmt,
         const uint8_t Depth) noexcept;

static void
PrintArrayDestructureItemIndex(Backend::LLVM::Handler &Handler,
                               const AST::ArrayDestructureIndex &Item,
                               const uint8_t Depth) noexcept
{
    PrintDepth(Depth);
    std::print("ArrayDestructureIndex\n");

    PrintAST(Handler, Item.IndexExpr, Depth + 1);
}

static void
PrintObjectDestructureFieldList(
    Backend::LLVM::Handler &Handler,
    const std::span<AST::ObjectDestructureField *const> FieldList,
    const uint8_t Depth) noexcept;

static void
PrintArrayDestructureItemList(
    Backend::LLVM::Handler &Handler,
    const std::span<AST::ArrayDestructureItem *const> ItemList,
    const uint8_t Depth) noexcept
{
    for (const auto Item : ItemList) {
        PrintDepth(Depth);
        switch (Item->getKind()) {
            case AST::ArrayDestructureItemKind::Identifier: {
                const auto IdentifierItem =
                    llvm::cast<AST::ArrayDestructureItemIdentifier>(Item);

                std::print("ArrayDestructureItemIdentifier<\"{}\">\n",
                           IdentifierItem->Name);

                if (Item->Index.has_value()) {
                    PrintArrayDestructureItemIndex(Handler, Item->Index.value(),
                                                   Depth + 1);
                }

                continue;
            }
            case AST::ArrayDestructureItemKind::Array: {
                const auto ArrayItem =
                    llvm::cast<AST::ArrayDestructureItemArray>(Item);

                std::print("ArrayDestructureItemArray\n");
                PrintArrayDestructureItemList(Handler, ArrayItem->ItemList,
                                              Depth + 1);

                if (const auto Index = Item->Index) {
                    PrintArrayDestructureItemIndex(Handler, Index.value(),
                                                   Depth + 1);
                }

                continue;
            }
            case AST::ArrayDestructureItemKind::Object: {
                const auto ObjectItem =
                    llvm::cast<AST::ArrayDestructureItemObject>(Item);

                std::print("ArrayDestructureItemObject\n");
                PrintObjectDestructureFieldList(Handler, ObjectItem->FieldList,
                                                Depth + 1);

                if (const auto Index = Item->Index) {
                    PrintArrayDestructureItemIndex(Handler, Index.value(),
                                                   Depth + 1);
                }

                continue;
            }
            case AST::ArrayDestructureItemKind::Spread: {
                const auto SpreadItem =
                    llvm::cast<AST::ArrayDestructureItemSpread>(Item);

                std::print("ArrayDestructureItemSpread<\"{}\">\n",
                           SpreadItem->Name);

                if (const auto Index = Item->Index) {
                    PrintArrayDestructureItemIndex(Handler, Index.value(),
                                                   Depth + 1);
                }

                continue;
            }
        }

        __builtin_unreachable();
    }
}

static void
PrintObjectDestructureFieldList(
    Backend::LLVM::Handler &Handler,
    const std::span<AST::ObjectDestructureField *const> FieldList,
    const uint8_t Depth) noexcept
{
    for (const auto Item : FieldList) {
        PrintDepth(Depth);
        switch (Item->getKind()) {
            case AST::ObjectDestructureFieldKind::Identifier: {
                const auto ObjectDestructureIdentifier =
                    llvm::cast<AST::ObjectDestructureFieldIdentifier>(Item);

                std::print("ObjectDestructureItemIdentifier<\"{}\">\n",
                           ObjectDestructureIdentifier->Name);

                continue;
            }
            case AST::ObjectDestructureFieldKind::Array: {
                const auto ObjectDestructureArray =
                    llvm::cast<AST::ObjectDestructureFieldArray>(Item);

                std::print("ObjectDestructureItemArray<\"{}\">\n",
                           ObjectDestructureArray->Key);

                PrintArrayDestructureItemList(Handler,
                                              ObjectDestructureArray->ItemList,
                                              Depth + 1);

                continue;
            }
            case AST::ObjectDestructureFieldKind::Object: {
                const auto ObjectDestructureObject =
                    llvm::cast<AST::ObjectDestructureFieldObject>(Item);

                std::print("ObjectDestructureItemObject<\"{}\">\n",
                           ObjectDestructureObject->Key);

                PrintObjectDestructureFieldList(
                    Handler, ObjectDestructureObject->FieldList, Depth + 1);

                continue;
            }
            case AST::ObjectDestructureFieldKind::Spread: {
                const auto ObjectDestructureSpread =
                    llvm::cast<AST::ObjectDestructureFieldSpread>(Item);

                std::print("ObjectDestructureItemSpread<\"{}\">\n",
                           ObjectDestructureSpread->Key);

                continue;
            }
        }

        __builtin_unreachable();
    }
}

static void
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

            std::print("BinaryOperation<\"{}\">\n", Lexeme.value());

            PrintAST(Handler, &BinaryExpr->getLhs(), Depth + 1);
            PrintAST(Handler, &BinaryExpr->getRhs(), Depth + 1);

            return;
        }
        case AST::NodeKind::UnaryOperation: {
            const auto UnaryExpr = llvm::cast<AST::UnaryOperation>(Stmt);
            const auto Lexeme =
                Parse::UnaryOperatorToLexemeMap[UnaryExpr->getOperator()];

            std::print("UnaryOperation<{}>\n", Lexeme.value());
            PrintAST(Handler, &UnaryExpr->getOperand(), Depth + 1);

            return;
        }
        case AST::NodeKind::CharLiteral: {
            const auto CharLit = llvm::cast<AST::CharLiteral>(Stmt);
            std::print("CharLiteral<{}>\n", CharLit->getValue());

            return;
        }
        case AST::NodeKind::NumberLiteral: {
            const auto IntLit = llvm::cast<AST::NumberLiteral>(Stmt);
            switch (IntLit->getNumber().Success.Kind) {
                case Parse::NumberKind::UnsignedInteger:
                    std::print("NumberLiteral<{}, unsigned>\n",
                               IntLit->getNumber().Success.UInt);
                    return;
                case Parse::NumberKind::SignedInteger:
                    std::print("NumberLiteral<{}, signed>\n",
                               IntLit->getNumber().Success.SInt);
                    return;
                case Parse::NumberKind::FloatingPoint:
                    assert(false && "Floating point literals not supported");
            }

            return;
        }
        case AST::NodeKind::FloatLiteral:
            assert(false && "Got float-literal while printing AST");
        case AST::NodeKind::StringLiteral: {
            const auto StringLit = llvm::cast<AST::StringLiteral>(Stmt);
            std::print("StringLiteral<\"{}\">\n", StringLit->value());

            return;
        }
        case AST::NodeKind::VarDecl: {
            const auto VarDecl = llvm::cast<AST::VarDecl>(Stmt);
            std::print("VarDecl<\"{}\">\n", VarDecl->getName());

            PrintDepth(Depth + 1);
            std::print("TypeExpr\n");

            PrintAST(Handler, VarDecl->getTypeExpr(), Depth + 2);

            PrintDepth(Depth + 1);
            std::print("InitExpr\n");

            PrintAST(Handler, VarDecl->getInitExpr(), Depth + 2);
            return;
        }
        case AST::NodeKind::OptionalUnwrapExpr: {
            const auto OptionalExpr = llvm::cast<AST::OptionalUnwrapExpr>(Stmt);

            std::print("OptionalUnwrapExpr\n");
            PrintAST(Handler, OptionalExpr->getBase(), Depth + 1);

            return;
        }
        case AST::NodeKind::ParenExpr: {
            const auto ParenExpr = llvm::cast<AST::ParenExpr>(Stmt);

            std::print("ParenExpr\n");
            PrintAST(Handler, ParenExpr->getChildExpr(), Depth + 1);

            return;
        }
        case AST::NodeKind::ParamVarDecl: {
            const auto ParamDecl = llvm::cast<AST::ParamVarDecl>(Stmt);
            std::print("ParamVarDecl<\"{}\">\n", ParamDecl->getName());

            PrintDepth(Depth + 1);

            std::print("TypeExpr\n");
            PrintAST(Handler, ParamDecl->getTypeExpr(), Depth + 2);

            PrintDepth(Depth + 1);
            std::print("DefaultExpr\n");

            PrintAST(Handler, ParamDecl->getDefaultExpr(), Depth + 2);
            return;
        }
        case AST::NodeKind::FunctionDecl: {
            const auto FuncDecl = llvm::cast<AST::FunctionDecl>(Stmt);
            std::print("FunctionDecl\n");

            PrintDepth(Depth + 1);
            std::print("Args\n");

            const auto &ParamList = FuncDecl->getParamList();
            for (const auto &Param : ParamList) {
                PrintAST(Handler, Param, Depth + 2);
            }

            const auto RetType = FuncDecl->getReturnTypeExpr();

            PrintDepth(Depth + 1);
            std::print("ReturnTypeExpr\n");

            PrintAST(Handler, RetType, Depth + 2);

            PrintDepth(Depth + 1);
            std::print("Body\n");

            PrintAST(Handler, FuncDecl->getBody(), Depth + 2);
            return;
        }
        case AST::NodeKind::DeclRefExpr: {
            const auto VarRef = llvm::cast<AST::DeclRefExpr>(Stmt);
            std::print("DeclRefExpr<\"{}\">\n", VarRef->getName());

            return;
        }
        case AST::NodeKind::CallExpr: {
            const auto CallExpr = llvm::cast<AST::CallExpr>(Stmt);

            std::print("CallExpr\n");

            PrintDepth(Depth + 1);
            std::print("Callee\n");

            PrintAST(Handler, CallExpr->getCalleeExpr(), Depth + 2);

            PrintDepth(Depth + 1);
            std::print("Args\n");

            for (const auto &Arg : CallExpr->getArgs()) {
                PrintAST(Handler, Arg, Depth + 2);
            }

            return;
        }
        case AST::NodeKind::IfStmt: {
            const auto IfStmt = llvm::cast<AST::IfExpr>(Stmt);

            std::print("IfStmt\n");
            PrintAST(Handler, IfStmt->getCond(), Depth + 1);

            std::print("\tThen\n");
            PrintAST(Handler, IfStmt->getThen(), Depth + 2);

            std::print("\tElseStmt\n");
            PrintAST(Handler, IfStmt->getElse(), Depth + 2);

            return;
        }
        case AST::NodeKind::ReturnStmt: {
            const auto ReturnStmt = llvm::cast<AST::ReturnStmt>(Stmt);

            std::print("ReturnStmt\n");
            PrintAST(Handler, ReturnStmt->getValue(), Depth + 1);

            return;
        }
        case AST::NodeKind::CompoundStmt: {
            const auto CompoundStmt = llvm::cast<AST::CompoundStmt>(Stmt);
            std::print("CompoundStmt\n");

            for (const auto &Stmt : CompoundStmt->getStmtList()) {
                PrintAST(Handler, Stmt, Depth + 1);
            }

            return;
        }
        case AST::NodeKind::StructDecl: {
            const auto StructDecl = llvm::cast<AST::StructDecl>(Stmt);
            std::print("StructDecl\n");

            for (const auto Field : StructDecl->getFieldList()) {
                PrintAST(Handler, Field, Depth + 1);
            }

            return;
        }
        case AST::NodeKind::ShapeDecl: {
            const auto ShapeDecl = llvm::cast<AST::ShapeDecl>(Stmt);
            std::print("ShapeDecl\n");

            for (const auto Field : ShapeDecl->getFieldList()) {
                PrintAST(Handler, Field, Depth + 1);
            }

            return;
        }
        case AST::NodeKind::FieldDecl:
        case AST::NodeKind::OptionalFieldDecl: {
            const auto FieldDecl = llvm::cast<AST::FieldDecl>(Stmt);
            if (llvm::isa<AST::OptionalFieldDecl>(FieldDecl)) {
                std::print("OptionalFieldDecl\n");
            } else {
                std::print("FieldDecl<\"{}\">\n", FieldDecl->getName());
            }

            PrintDepth(Depth + 1);

            std::print("TypeExpr\n");
            PrintAST(Handler, FieldDecl->getTypeExpr(), Depth + 2);

            PrintDepth(Depth + 1);

            std::print("InitExpr\n");
            PrintAST(Handler, FieldDecl->getInitExpr(), Depth + 2);

            return;
        }
        case AST::NodeKind::FieldExpr: {
            const auto FieldExpr = llvm::cast<AST::FieldExpr>(Stmt);
            std::print("MemberAccessExpr<'{}', \"{}\">\n",
                       FieldExpr->isArrow() ? "->" : ".",
                       FieldExpr->getMemberName());

            PrintAST(Handler, FieldExpr->getBase(), Depth + 1);
            return;
        }
        case AST::NodeKind::ArraySubscriptExpr: {
            const auto ArraySubscriptExpr =
                llvm::cast<AST::ArraySubscriptExpr>(Stmt);

            std::print("ArraySubscriptExpr\n");
            PrintAST(Handler, ArraySubscriptExpr->getBase(), Depth + 1);

            PrintDepth(Depth + 1);
            std::print("DetailList\n");

            for (const auto Stmt : ArraySubscriptExpr->getDetailList()) {
                PrintAST(Handler, Stmt, Depth + 2);
            }

            return;
        }
        case AST::NodeKind::LvalueNamedDecl: {
            const auto LvalueNamedDecl = llvm::cast<AST::LvalueNamedDecl>(Stmt);
            std::print("LvalueNamedDecl<\"{}\">\n", LvalueNamedDecl->getName());

            PrintAST(Handler, LvalueNamedDecl->getRvalueExpr(), Depth + 1);
            return;
        }
        case AST::NodeKind::EnumMemberDecl: {
            const auto EnumMemberDecl = llvm::cast<AST::EnumMemberDecl>(Stmt);
            std::print("EnumMemberDecl<\"{}\">\n", EnumMemberDecl->getName());

            PrintAST(Handler, EnumMemberDecl->getInitExpr(), Depth + 1);
            return;
        }
        case AST::NodeKind::EnumDecl: {
            const auto EnumDecl = llvm::cast<AST::EnumDecl>(Stmt);
            std::print("EnumDecl\n");

            for (const auto EnumMember : EnumDecl->getMemberList()) {
                PrintAST(Handler, EnumMember, Depth + 1);
            }

            return;
        }
        case AST::NodeKind::ArrayDecl: {
            const auto ArrayDecl = llvm::cast<AST::ArrayDecl>(Stmt);
            std::print("ArrayDecl\n");

            for (const auto Element : ArrayDecl->getElementList()) {
                PrintAST(Handler, Element, Depth + 1);
            }

            return;
        }
        case AST::NodeKind::ClosureDecl: {
            const auto LambdaDecl = llvm::cast<AST::ClosureDecl>(Stmt);
            std::print("LambdaDecl\n");

            PrintDepth(Depth + 1);
            std::print("CaptureList\n");

            for (const auto Capture : LambdaDecl->getCaptureList()) {
                PrintAST(Handler, Capture, Depth + 2);
            }

            PrintDepth(Depth + 1);
            std::print("Arguments\n");

            for (const auto Param : LambdaDecl->getParamList()) {
                PrintAST(Handler, Param, Depth + 2);
            }

            PrintDepth(Depth + 1);
            std::print("Body\n");

            PrintAST(Handler, LambdaDecl->getBody(), Depth + 2);
            return;
        }
        case AST::NodeKind::CommaSepStmtList: {
            const auto CommaSepStmtList =
                llvm::cast<AST::CommaSepStmtList>(Stmt);

            std::print("CommaSepStmtList\n");
            for (const auto Stmt : CommaSepStmtList->getStmtList()) {
                PrintAST(Handler, Stmt, Depth + 1);
            }

            return;
        }
        case AST::NodeKind::ForStmt: {
            const auto ForStmt = llvm::cast<AST::ForStmt>(Stmt);
            std::print("ForStmt\n");

            PrintAST(Handler, ForStmt->getInit(), Depth + 1);
            PrintAST(Handler, ForStmt->getCond(), Depth + 1);
            PrintAST(Handler, ForStmt->getStep(), Depth + 1);
            PrintAST(Handler, ForStmt->getBody(), Depth + 1);

            return;
        }
        case AST::NodeKind::ArrayDestructuredVarDecl: {
            const auto ArrayDestructuredVarDecl =
                llvm::cast<AST::ArrayDestructuredVarDecl>(Stmt);

            std::print("ArrayDestructuredVarDecl\n");
            PrintArrayDestructureItemList(
                Handler, ArrayDestructuredVarDecl->getItemList(), Depth + 1);

            PrintDepth(Depth + 1);
            std::print("InitExpr\n");

            PrintAST(Handler, ArrayDestructuredVarDecl->getInitExpr(),
                     Depth + 2);

            return;
        }
        case AST::NodeKind::ObjectDestructuredVarDecl: {
            auto ObjectDestructuredVarDecl =
                llvm::cast<AST::ObjectDestructuredVarDecl>(Stmt);

            std::print("ObjectDestructuredVarDecl\n");
            PrintObjectDestructureFieldList(
                Handler, ObjectDestructuredVarDecl->getFieldList(), Depth + 1);

            PrintAST(Handler, ObjectDestructuredVarDecl->getInitExpr(),
                     Depth + 1);

            return;
        }
        case AST::NodeKind::CastExpr: {
            const auto CastExpr = llvm::cast<AST::CastExpr>(Stmt);
            std::print("CastExpr\n");

            PrintDepth(Depth + 1);
            std::print("Operand\n");

            PrintAST(Handler, CastExpr->getOperand(), Depth + 2);

            PrintDepth(Depth + 1);
            std::print("Type\n");

            PrintAST(Handler, CastExpr->getTypeExpr(), Depth + 2);
            return;
        }
        case AST::NodeKind::DerefExpr: {
            const auto DerefExpr = llvm::cast<AST::DerefExpr>(Stmt);

            std::print("DerefExpr\n");
            PrintAST(Handler, DerefExpr->getOperand(), Depth + 1);

            return;
        }
        case AST::NodeKind::ArrayType: {
            const auto ArrayTypeExpr = llvm::cast<AST::ArrayTypeExpr>(Stmt);

            std::print("ArrayTypeExpr\n");
            PrintAST(Handler, ArrayTypeExpr->getBase(), Depth + 1);

            PrintDepth(Depth + 1);
            std::print("Detail-List\n");

            for (const auto &Detail : ArrayTypeExpr->getDetailList()) {
                PrintAST(Handler, Detail, Depth + 2);
            }

            return;
        }
        case AST::NodeKind::FunctionType: {
            const auto FuncTypeExpr = llvm::cast<AST::FunctionTypeExpr>(Stmt);
            std::print("FunctionTypeExpr\n");

            PrintDepth(Depth + 1);
            std::print("ParamList\n");

            for (const auto &Param : FuncTypeExpr->getParamList()) {
                PrintAST(Handler, Param, Depth + 2);
            }

            PrintDepth(Depth + 1);
            std::print("ReturnType\n");

            PrintAST(Handler, FuncTypeExpr->getReturnType(), Depth + 2);
            return;
        }
        case AST::NodeKind::OptionalType: {
            const auto OptionalExpr = llvm::cast<AST::OptionalTypeExpr>(Stmt);

            std::print("OptionalTypeExpr\n");
            PrintAST(Handler, OptionalExpr->getOperand(), Depth + 1);

            return;
        }
        case AST::NodeKind::PointerType: {
            const auto PointerExpr = llvm::cast<AST::PointerTypeExpr>(Stmt);

            std::print("PointerTypeExpr\n");
            PrintAST(Handler, PointerExpr->getOperand(), Depth + 1);

            return;
        }
        case AST::NodeKind::ClosureType:
        case AST::NodeKind::StructType:
        case AST::NodeKind::EnumType:
        case AST::NodeKind::ShapeType:
        case AST::NodeKind::UnionType:
            __builtin_unreachable();
    }

    __builtin_unreachable();
}

struct ArgumentOptions {
    bool PrintTokens : 1 = false;
    bool PrintAST : 1 = false;
    bool PrintIR : 1 = false;

    uint32_t PrintDepth = 0;
};

void
HandlePrompt(const std::string_view &Prompt,
             const ArgumentOptions ArgOptions) noexcept
{
    auto SrcBuffer = ADT::SourceBuffer::FromString(Prompt);
    if (SrcBuffer == nullptr) {
        std::print("Failed to open source-buffer from input");
        return;
    }

    auto Diag = SourceFileDiagnosticConsumer(std::string_view("<input>"));
    auto TokenBuffer = Lex::TokenBuffer::Create(*SrcBuffer, Diag);

    if (ArgOptions.PrintTokens) {
        auto TokenList = TokenBuffer->getTokenList();
        fputs("Tokens:\n", stdout);

        for (const auto &Token : TokenList) {
            std::print("\t{}\n", Lex::TokenKindGetName(Token.Kind));
        }

        fputc('\n', stdout);
    }

    if (Diag.hasMessages()) {
        return;
    }

    const auto Options = Parse::ParseOptions({
        .DontRequireSemicolons = true,
        .IgnoreUnusedExpressions = true
    });

    auto Unit = Parse::ParseUnit::Create(*TokenBuffer, Diag, Options);
    if (Unit.getTopLevelStmtList().empty()) {
        return;
    }

    if (Diag.hasMessages()) {
        return;
    }

    auto BackendHandlerExp = Backend::LLVM::JITHandler::create(Diag, Unit);
    if (!BackendHandlerExp.has_value()) {
        return;
    }

    const auto BackendHandler = BackendHandlerExp->get();
    const auto Expr = Unit.getTopLevelStmtList().back();

    if (ArgOptions.PrintAST) {
        PrintAST(*BackendHandler, Expr, /*Depth=*/ArgOptions.PrintDepth);
        fputc('\n', stdout);
    }

#if 0
    BackendHandler->evaluateAndPrint(*Expr,
                                     ArgOptions.PrintIR,
                                     BHGRN "Evaluation> " CRESET,
                                     "\n");
#endif
}

void PrintUsage(const char *const Name) noexcept {
    std::print("Usage: {} [<prompt>] [-h/--help/-u/--usage] [--print-tokens] "
               "[--print-ast]\n",
               Name);
}

void HandleReplOption(const ArgumentOptions ArgOptions) {
    Interface::SetupRepl("Compiler",
                         [&](const std::string_view Input) noexcept {
                            HandlePrompt(Input, ArgOptions);
                            return true;
                         });
}

void
HandleFileOptions(const ArgumentOptions ArgOptions,
                  const std::span<std::string_view> FilePaths) noexcept
{
    for (const auto Path : FilePaths) {
        auto Diag = SourceFileDiagnosticConsumer(Path);
        const auto SrcBufferOpt = ADT::SourceBuffer::FromFile(Path);

        if (SrcBufferOpt.has_value()) {
            const auto Error = SrcBufferOpt.error();
            switch (Error.Kind) {
                case ADT::SourceBuffer::Error::Kind::FailedToOpenFile:
                    std::print(stderr,
                               "Failed to open file: {}, reason: {}\n",
                               Path,
                               Error.Reason);
                    exit(1);
                case ADT::SourceBuffer::Error::Kind::FailedToStatFile:
                    std::print(stderr,
                               "Failed to stat file: {}, reason: {}\n",
                               Path,
                               Error.Reason);
                    exit(1);
                case ADT::SourceBuffer::Error::Kind::FailedToMapFile:
                    std::print(stderr,
                               "Failed to map file: {}, reason: {}\n",
                               Path,
                               Error.Reason);
                    exit(1);
            }
        }

        const auto SrcBuffer = SrcBufferOpt.value();
        const auto TokenBuffer = Lex::TokenBuffer::Create(*SrcBuffer, Diag);

        if (ArgOptions.PrintTokens) {
            auto TokenList = TokenBuffer->getTokenList();
            fputs("Tokens:\n", stdout);

            for (const auto &Token : TokenList) {
                std::print("\t{}\n", Lex::TokenKindGetName(Token.Kind));
            }

            fputc('\n', stdout);
        }

        const auto Options = Parse::ParseOptions();

        auto Unit = Parse::ParseUnit::Create(*TokenBuffer, Diag, Options);
        auto BackendHandler = Backend::LLVM::Handler(Diag);

        if (ArgOptions.PrintAST) {
            for (const auto &[Name, Decl] : Unit.getTopLevelDeclList()) {
                PrintAST(BackendHandler, Decl, /*Depth=*/ArgOptions.PrintDepth);
            }
        }

        // Context.visitDecls(Diag, AST::Context::VisitOptions());
        // BackendHandler.evaluate(Diag);
        if (ArgOptions.PrintIR) {
            BackendHandler.getModule().print(llvm::outs(), nullptr);
        }
    }
}

int main(const int Argc, const char *const Argv[]) {
    auto Options = ArgumentOptions();
    auto FilePaths = std::vector<std::string_view>();
    auto Lexer = ArgvLexer(Argc, Argv);

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
            PrintUsage(Argv[0]);
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

        std::print(stderr, "Unrecognized option: {}\n", Arg);
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
