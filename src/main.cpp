/*
 * main.cpp
 */

#include <cassert>
#include <cstdio>

#include "AST/Decls/ArrayDecl.h"
#include "AST/Decls/ClosureDecl.h"
#include "AST/Decls/InterfaceDecl.h"
#include "AST/Decls/ObjectBindingVarDecl.h"
#include "AST/Decls/EnumDecl.h"
#include "AST/Decls/EnumMemberDecl.h"
#include "AST/Decls/FieldDecl.h"
#include "AST/Decls/FunctionDecl.h"
#include "AST/Decls/OptionalFieldDecl.h"
#include "AST/Decls/ParamVarDecl.h"
#include "AST/Decls/ShapeDecl.h"
#include "AST/Decls/StructDecl.h"
#include "AST/Decls/UnionDecl.h"
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
#include "AST/DotIdentifierExpr.h"
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

static void PrintAST(AST::Stmt *const Stmt, const uint8_t Depth) noexcept;

static void
PrintArrayBindingItemIndex(const AST::ArrayBindingIndex &Item,
                           const uint8_t Depth) noexcept
{
    PrintDepth(Depth);
    std::print("ArrayBindingIndex\n");

    PrintAST(Item.IndexExpr, Depth + 1);
}

static void
PrintObjectBindingFieldList(
    const std::span<AST::ObjectBindingField *const> FieldList,
    const uint8_t Depth) noexcept;

static void
PrintArrayBindingItemList(
    const std::span<AST::ArrayBindingItem *const> ItemList,
    const uint8_t Depth) noexcept
{
    for (const auto Item : ItemList) {
        PrintDepth(Depth);
        switch (Item->getKind()) {
            case AST::ArrayBindingItemKind::Identifier: {
                const auto IdentifierItem =
                    llvm::cast<AST::ArrayBindingItemIdentifier>(Item);

                std::print("ArrayBindingItemIdentifier<\"{}\">\n",
                           IdentifierItem->Name);

                if (Item->Index.has_value()) {
                    PrintArrayBindingItemIndex(Item->Index.value(), Depth + 1);
                }

                continue;
            }
            case AST::ArrayBindingItemKind::Array: {
                const auto ArrayItem =
                    llvm::cast<AST::ArrayBindingItemArray>(Item);

                std::print("ArrayBindingItemArray\n");
                PrintArrayBindingItemList(ArrayItem->ItemList, Depth + 1);

                if (const auto Index = Item->Index) {
                    PrintArrayBindingItemIndex(Index.value(), Depth + 1);
                }

                continue;
            }
            case AST::ArrayBindingItemKind::Object: {
                const auto ObjectItem =
                    llvm::cast<AST::ArrayBindingItemObject>(Item);

                std::print("ArrayBindingItemObject\n");
                PrintObjectBindingFieldList(ObjectItem->FieldList, Depth + 1);

                if (const auto Index = Item->Index) {
                    PrintArrayBindingItemIndex(Index.value(), Depth + 1);
                }

                continue;
            }
            case AST::ArrayBindingItemKind::Spread: {
                const auto SpreadItem =
                    llvm::cast<AST::ArrayBindingItemSpread>(Item);

                std::print("ArrayBindingItemSpread<\"{}\">\n",
                           SpreadItem->Name);

                if (const auto Index = Item->Index) {
                    PrintArrayBindingItemIndex(Index.value(), Depth + 1);
                }

                continue;
            }
        }

        __builtin_unreachable();
    }
}

static void
PrintObjectBindingFieldList(
    const std::span<AST::ObjectBindingField *const> FieldList,
    const uint8_t Depth) noexcept
{
    for (const auto Item : FieldList) {
        PrintDepth(Depth);
        switch (Item->getKind()) {
            case AST::ObjectBindingFieldKind::Identifier: {
                const auto ObjectBindingIdentifier =
                    llvm::cast<AST::ObjectBindingFieldIdentifier>(Item);

                std::print("ObjectBindingItemIdentifier<\"{}\">\n",
                           ObjectBindingIdentifier->Name);

                continue;
            }
            case AST::ObjectBindingFieldKind::Array: {
                const auto ObjectBindingArray =
                    llvm::cast<AST::ObjectBindingFieldArray>(Item);

                std::print("ObjectBindingItemArray<\"{}\">\n",
                           ObjectBindingArray->Key);

                PrintArrayBindingItemList(ObjectBindingArray->ItemList,
                                          Depth + 1);

                continue;
            }
            case AST::ObjectBindingFieldKind::Object: {
                const auto ObjectBindingObject =
                    llvm::cast<AST::ObjectBindingFieldObject>(Item);

                std::print("ObjectBindingItemObject<\"{}\">\n",
                           ObjectBindingObject->Key);

                PrintObjectBindingFieldList(ObjectBindingObject->FieldList,
                                            Depth + 1);

                continue;
            }
            case AST::ObjectBindingFieldKind::Spread: {
                const auto ObjectBindingSpread =
                    llvm::cast<AST::ObjectBindingFieldSpread>(Item);

                std::print("ObjectBindingItemSpread<\"{}\">\n",
                           ObjectBindingSpread->Key);

                continue;
            }
        }

        __builtin_unreachable();
    }
}

static void PrintAST(AST::Stmt *const Stmt, const uint8_t Depth) noexcept {
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

            PrintAST(&BinaryExpr->getLhs(), Depth + 1);
            PrintAST(&BinaryExpr->getRhs(), Depth + 1);

            return;
        }
        case AST::NodeKind::UnaryOperation: {
            const auto UnaryExpr = llvm::cast<AST::UnaryOperation>(Stmt);
            const auto Lexeme =
                Parse::UnaryOperatorToLexemeMap[UnaryExpr->getOperator()];

            std::print("UnaryOperation<{}>\n", Lexeme.value());
            PrintAST(&UnaryExpr->getOperand(), Depth + 1);

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

            PrintAST(VarDecl->getTypeExpr(), Depth + 2);

            PrintDepth(Depth + 1);
            std::print("InitExpr\n");

            PrintAST(VarDecl->getInitExpr(), Depth + 2);
            return;
        }
        case AST::NodeKind::OptionalUnwrapExpr: {
            const auto OptionalExpr = llvm::cast<AST::OptionalUnwrapExpr>(Stmt);

            std::print("OptionalUnwrapExpr\n");
            PrintAST(OptionalExpr->getBase(), Depth + 1);

            return;
        }
        case AST::NodeKind::ParenExpr: {
            const auto ParenExpr = llvm::cast<AST::ParenExpr>(Stmt);

            std::print("ParenExpr\n");
            PrintAST(ParenExpr->getChildExpr(), Depth + 1);

            return;
        }
        case AST::NodeKind::ParamVarDecl: {
            const auto ParamDecl = llvm::cast<AST::ParamVarDecl>(Stmt);
            std::print("ParamVarDecl<\"{}\">\n", ParamDecl->getName());

            PrintDepth(Depth + 1);

            std::print("TypeExpr\n");
            PrintAST(ParamDecl->getTypeExpr(), Depth + 2);

            PrintDepth(Depth + 1);
            std::print("DefaultExpr\n");

            PrintAST(ParamDecl->getDefaultExpr(), Depth + 2);
            return;
        }
        case AST::NodeKind::FunctionDecl: {
            const auto FuncDecl = llvm::cast<AST::FunctionDecl>(Stmt);
            std::print("FunctionDecl\n");

            PrintDepth(Depth + 1);
            std::print("Args\n");

            const auto &ParamList = FuncDecl->getParamList();
            for (const auto Param : ParamList) {
                PrintAST(Param, Depth + 2);
            }

            const auto RetType = FuncDecl->getReturnTypeExpr();

            PrintDepth(Depth + 1);
            std::print("ReturnTypeExpr\n");

            PrintAST(RetType, Depth + 2);

            PrintDepth(Depth + 1);
            std::print("Body\n");

            PrintAST(FuncDecl->getBody(), Depth + 2);
            return;
        }
        case AST::NodeKind::DeclRefExpr: {
            const auto VarRef = llvm::cast<AST::DeclRefExpr>(Stmt);
            std::print("DeclRefExpr<\"{}\">\n", VarRef->getName());

            return;
        }
        case AST::NodeKind::DotIdentifierExpr: {
            const auto DotIdentExpr = llvm::cast<AST::DotIdentifierExpr>(Stmt);
            std::print("DotIdentifierForExpr<\"{}\">\n",
                       DotIdentExpr->getIdentifier());

            return;
        }
        case AST::NodeKind::CallExpr: {
            const auto CallExpr = llvm::cast<AST::CallExpr>(Stmt);

            std::print("CallExpr\n");

            PrintDepth(Depth + 1);
            std::print("Callee\n");

            PrintAST(CallExpr->getCalleeExpr(), Depth + 2);

            PrintDepth(Depth + 1);
            std::print("Args\n");

            for (const auto Arg : CallExpr->getArgs()) {
                PrintDepth(Depth + 2);
                std::print("Arg\n");

                PrintDepth(Depth + 3);
                std::print("Label\n");

                PrintDepth(Depth + 4);
                std::print("\"{}\"\n", Arg.Label.value_or("<null>"));

                PrintDepth(Depth + 3);
                std::print("Expr\n");

                PrintAST(Arg.Expr, Depth + 4);
            }

            return;
        }
        case AST::NodeKind::IfExpr: {
            const auto IfExpr = llvm::cast<AST::IfExpr>(Stmt);
            std::print("IfExpr\n");

            PrintDepth(Depth + 1);
            std::print("Cond\n");

            PrintAST(IfExpr->getCond(), Depth + 2);

            PrintDepth(Depth + 1);
            std::print("Then\n");

            PrintAST(IfExpr->getThen(), Depth + 2);

            PrintDepth(Depth + 1);
            std::print("Else\n");

            PrintAST(IfExpr->getElse(), Depth + 2);
            return;
        }
        case AST::NodeKind::ReturnStmt: {
            const auto ReturnStmt = llvm::cast<AST::ReturnStmt>(Stmt);

            std::print("ReturnStmt\n");
            PrintAST(ReturnStmt->getValue(), Depth + 1);

            return;
        }
        case AST::NodeKind::CompoundStmt: {
            const auto CompoundStmt = llvm::cast<AST::CompoundStmt>(Stmt);
            std::print("CompoundStmt\n");

            for (const auto Stmt : CompoundStmt->getStmtList()) {
                PrintAST(Stmt, Depth + 1);
            }

            return;
        }
        case AST::NodeKind::InterfaceDecl: {
            const auto InterfaceDecl = llvm::cast<AST::InterfaceDecl>(Stmt);
            std::print("InterfaceDecl\n");

            for (const auto Field : InterfaceDecl->getFieldList()) {
                PrintAST(Field, Depth + 1);
            }

            return;
        }
        case AST::NodeKind::StructDecl: {
            const auto StructDecl = llvm::cast<AST::StructDecl>(Stmt);
            std::print("StructDecl\n");

            for (const auto Field : StructDecl->getFieldList()) {
                PrintAST(Field, Depth + 1);
            }

            return;
        }
        case AST::NodeKind::ShapeDecl: {
            const auto ShapeDecl = llvm::cast<AST::ShapeDecl>(Stmt);
            std::print("ShapeDecl\n");

            for (const auto Field : ShapeDecl->getFieldList()) {
                PrintAST(Field, Depth + 1);
            }

            return;
        }
        case AST::NodeKind::UnionDecl: {
            const auto UnionDecl = llvm::cast<AST::UnionDecl>(Stmt);
            std::print("UnionDecl\n");

            for (const auto Field : UnionDecl->getFieldList()) {
                PrintAST(Field, Depth + 1);
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
            PrintAST(FieldDecl->getTypeExpr(), Depth + 2);

            PrintDepth(Depth + 1);

            std::print("InitExpr\n");
            PrintAST(FieldDecl->getInitExpr(), Depth + 2);

            return;
        }
        case AST::NodeKind::FieldExpr: {
            const auto FieldExpr = llvm::cast<AST::FieldExpr>(Stmt);
            std::print("MemberAccessExpr<'{}', \"{}\">\n",
                       FieldExpr->isArrow() ? "->" : ".",
                       FieldExpr->getMemberName());

            PrintAST(FieldExpr->getBase(), Depth + 1);
            return;
        }
        case AST::NodeKind::ArraySubscriptExpr: {
            const auto ArraySubscriptExpr =
                llvm::cast<AST::ArraySubscriptExpr>(Stmt);

            std::print("ArraySubscriptExpr\n");
            PrintAST(ArraySubscriptExpr->getBase(), Depth + 1);

            PrintDepth(Depth + 1);
            std::print("DetailList\n");

            for (const auto Stmt : ArraySubscriptExpr->getDetailList()) {
                PrintAST(Stmt, Depth + 2);
            }

            return;
        }
        case AST::NodeKind::LvalueNamedDecl: {
            const auto LvalueNamedDecl = llvm::cast<AST::LvalueNamedDecl>(Stmt);
            std::print("LvalueNamedDecl<\"{}\">\n", LvalueNamedDecl->getName());

            PrintAST(LvalueNamedDecl->getRvalueExpr(), Depth + 1);
            return;
        }
        case AST::NodeKind::EnumMemberDecl: {
            const auto EnumMemberDecl = llvm::cast<AST::EnumMemberDecl>(Stmt);
            std::print("EnumMemberDecl<\"{}\">\n", EnumMemberDecl->getName());

            PrintAST(EnumMemberDecl->getInitExpr(), Depth + 1);
            return;
        }
        case AST::NodeKind::EnumDecl: {
            const auto EnumDecl = llvm::cast<AST::EnumDecl>(Stmt);
            std::print("EnumDecl\n");

            for (const auto EnumMember : EnumDecl->getMemberList()) {
                PrintAST(EnumMember, Depth + 1);
            }

            return;
        }
        case AST::NodeKind::ArrayDecl: {
            const auto ArrayDecl = llvm::cast<AST::ArrayDecl>(Stmt);
            std::print("ArrayDecl\n");

            for (const auto Element : ArrayDecl->getElementList()) {
                PrintAST(Element, Depth + 1);
            }

            return;
        }
        case AST::NodeKind::ClosureDecl: {
            const auto ClosureDeck = llvm::cast<AST::ClosureDecl>(Stmt);
            std::print("ClosureDecl\n");

            PrintDepth(Depth + 1);
            std::print("CaptureList\n");

            for (const auto Capture : ClosureDeck->getCaptureList()) {
                PrintAST(Capture, Depth + 2);
            }

            PrintDepth(Depth + 1);
            std::print("Arguments\n");

            for (const auto Param : ClosureDeck->getParamList()) {
                PrintAST(Param, Depth + 2);
            }

            PrintDepth(Depth + 1);
            std::print("Body\n");

            PrintAST(ClosureDeck->getBody(), Depth + 2);
            return;
        }
        case AST::NodeKind::CommaSepStmtList: {
            const auto CommaSepStmtList =
                llvm::cast<AST::CommaSepStmtList>(Stmt);

            std::print("CommaSepStmtList\n");
            for (const auto Stmt : CommaSepStmtList->getStmtList()) {
                PrintAST(Stmt, Depth + 1);
            }

            return;
        }
        case AST::NodeKind::ForStmt: {
            const auto ForStmt = llvm::cast<AST::ForStmt>(Stmt);
            std::print("ForStmt\n");

            PrintAST(ForStmt->getInit(), Depth + 1);
            PrintAST(ForStmt->getCond(), Depth + 1);
            PrintAST(ForStmt->getStep(), Depth + 1);
            PrintAST(ForStmt->getBody(), Depth + 1);

            return;
        }
        case AST::NodeKind::ArrayBindingVarDecl: {
            const auto ArrayBindingVarDecl =
                llvm::cast<AST::ArrayBindingVarDecl>(Stmt);

            std::print("ArrayBindingVarDecl\n");
            PrintArrayBindingItemList(ArrayBindingVarDecl->getItemList(),
                                      Depth + 1);

            PrintDepth(Depth + 1);
            std::print("InitExpr\n");

            PrintAST(ArrayBindingVarDecl->getInitExpr(), Depth + 2);
            return;
        }
        case AST::NodeKind::ObjectBindingVarDecl: {
            auto ObjectBindingVarDecl =
                llvm::cast<AST::ObjectBindingVarDecl>(Stmt);

            std::print("ObjectBindingVarDecl\n");
            PrintObjectBindingFieldList(ObjectBindingVarDecl->getFieldList(),
                                        Depth + 1);

            PrintAST(ObjectBindingVarDecl->getInitExpr(), Depth + 1);
            return;
        }
        case AST::NodeKind::CastExpr: {
            const auto CastExpr = llvm::cast<AST::CastExpr>(Stmt);
            std::print("CastExpr\n");

            PrintDepth(Depth + 1);
            std::print("Operand\n");

            PrintAST(CastExpr->getOperand(), Depth + 2);

            PrintDepth(Depth + 1);
            std::print("Type\n");

            PrintAST(CastExpr->getTypeExpr(), Depth + 2);
            return;
        }
        case AST::NodeKind::DerefExpr: {
            const auto DerefExpr = llvm::cast<AST::DerefExpr>(Stmt);

            std::print("DerefExpr\n");
            PrintAST(DerefExpr->getOperand(), Depth + 1);

            return;
        }
        case AST::NodeKind::CaptureAllByRefExpr:
            std::print("CaptureAllByRefExpr\n");
            return;
        case AST::NodeKind::CaptureAllByValueExpr:
            std::print("CaptureAllByValueExpr\n");
            return;
        case AST::NodeKind::ArrayType: {
            const auto ArrayTypeExpr = llvm::cast<AST::ArrayTypeExpr>(Stmt);

            std::print("ArrayTypeExpr\n");
            PrintAST(ArrayTypeExpr->getBase(), Depth + 1);

            PrintDepth(Depth + 1);
            std::print("DetailList\n");

            for (const auto Detail : ArrayTypeExpr->getDetailList()) {
                PrintAST(Detail, Depth + 2);
            }

            return;
        }
        case AST::NodeKind::FunctionType: {
            const auto FuncTypeExpr = llvm::cast<AST::FunctionTypeExpr>(Stmt);
            std::print("FunctionTypeExpr\n");

            PrintDepth(Depth + 1);
            std::print("ParamList\n");

            for (const auto Param : FuncTypeExpr->getParamList()) {
                PrintAST(Param, Depth + 2);
            }

            PrintDepth(Depth + 1);
            std::print("ReturnType\n");

            PrintAST(FuncTypeExpr->getReturnType(), Depth + 2);
            return;
        }
        case AST::NodeKind::OptionalType: {
            const auto OptionalExpr = llvm::cast<AST::OptionalTypeExpr>(Stmt);

            std::print("OptionalTypeExpr\n");
            PrintAST(OptionalExpr->getOperand(), Depth + 1);

            return;
        }
        case AST::NodeKind::PointerType: {
            const auto PointerExpr = llvm::cast<AST::PointerTypeExpr>(Stmt);

            std::print("PointerTypeExpr\n");
            PrintAST(PointerExpr->getOperand(), Depth + 1);

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
        std::print("Tokens:\n");

        for (const auto Token : TokenList) {
            std::print("\t{}\n", Lex::TokenKindGetName(Token.Kind));
        }

        std::print("\n");
    }

    if (Diag.hasMessages()) {
        Diag.print();
        if (Diag.hasErrors()) {
            return;
        }
    }

    const auto Options = Parse::ParseOptions({
        .DontRequireSemicolons = true,
        .IgnoreUnusedExpressions = true
    });

    auto Unit = Parse::ParseUnit::Create(*TokenBuffer, Diag, Options);
    if (Diag.hasMessages()) {
        Diag.print();
    }

    if (Unit.getTopLevelStmtList().empty()) {
        return;
    }

    if (ArgOptions.PrintAST) {
        const auto Expr = Unit.getTopLevelStmtList().back();

        PrintAST(Expr, ArgOptions.PrintDepth);
        std::print("\n");
    }

    if (Diag.hasErrors()) {
        return;
    }

    auto BackendHandlerExp = Backend::LLVM::JITHandler::create(Diag, Unit);
    if (!BackendHandlerExp.has_value()) {
        return;
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

        if (!SrcBufferOpt.has_value()) {
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
            std::print("Tokens:\n");

            for (const auto Token : TokenList) {
                std::print("\t{}\n", Lex::TokenKindGetName(Token.Kind));
            }

            std::print("\n");
        }

        const auto Options = Parse::ParseOptions();

        auto Unit = Parse::ParseUnit::Create(*TokenBuffer, Diag, Options);
        auto BackendHandler = Backend::LLVM::Handler(Diag);

        if (ArgOptions.PrintAST) {
            for (const auto &[Name, Decl] : Unit.getTopLevelDeclList()) {
                PrintAST(Decl, /*Depth=*/ArgOptions.PrintDepth);
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
    auto FilePaths = std::vector<std::string_view>();

    auto Options = ArgumentOptions();
    auto Lexer = ArgvLexer(Argc, Argv);

    while (const auto ArgOpt = Lexer.consume()) {
        const auto Arg = std::string_view(ArgOpt);
        if (!Arg.starts_with("-")) {
            FilePaths.push_back(Arg);
            break;
        }

        if (Arg == "-h" || Arg == "--help" || Arg == "-u" || Arg == "--usage") {
            PrintUsage(Argv[0]);
            return 0;
        }

        if (Arg == "--print-tokens") {
            Options.PrintTokens = true;
            continue;
        }

        if (Arg == "--print-ast") {
            Options.PrintAST = true;
            continue;
        }

        if (Arg == "--print-ir") {
            Options.PrintIR = true;
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
