/*
 * Parse/ParseUnit.cpp
 * © suhas pai
 */

#include "AST/BinaryOperation.h"
#include "AST/Decls/FunctionDecl.h"
#include "AST/Decls/ObjectBindingVarDecl.h"

#include "llvm/Support/Casting.h"

#include "Parse/Context.h"
#include "Parse/ParseMisc.h"
#include "Parse/ParseStmt.h"
#include "Parse/ParseUnit.h"

namespace Parse {
    static void
    HandleArrayBindingArrayItem(
        std::span<AST::ArrayBindingItem *const> ItemList,
        ADT::UnorderedStringMap<AST::LvalueNamedDecl *> &TopLevelDeclList,
        std::vector<ParseUnit::AddError::DeclName> &DeclNameList) noexcept;

    static void
    HandleObjectBindingField(
        AST::ObjectBindingField *FieldList,
        ADT::UnorderedStringMap<AST::LvalueNamedDecl *> &TopLevelDeclList,
        std::vector<ParseUnit::AddError::DeclName> &DeclNameList) noexcept;

    static void
    HandleObjectBindingFieldList(
        std::span<AST::ObjectBindingField *const> FieldList,
        ADT::UnorderedStringMap<AST::LvalueNamedDecl *> &TopLevelDeclList,
        std::vector<ParseUnit::AddError::DeclName> &DeclNameList) noexcept
    {
        for (const auto &Field : FieldList) {
            HandleObjectBindingField(Field, TopLevelDeclList, DeclNameList);
        }
    }

    static void
    HandleArrayBindingItem(
        AST::ArrayBindingItem *const Item,
        ADT::UnorderedStringMap<AST::LvalueNamedDecl *> &TopLevelDeclList,
        std::vector<ParseUnit::AddError::DeclName> &DeclNameList) noexcept
    {
        switch (Item->getKind()) {
            case AST::ArrayBindingItemKind::Identifier: {
                const auto IdentifierItem =
                    llvm::cast<AST::ArrayBindingItemIdentifier>(Item);

                const auto Name = IdentifierItem->Name;
                if (TopLevelDeclList.contains(Name)) {
                    DeclNameList.emplace_back(Name, IdentifierItem->NameLoc);
                }

                break;
            }
            case AST::ArrayBindingItemKind::Array: {
                const auto ArrayItem =
                    llvm::cast<AST::ArrayBindingItemArray>(Item);

                HandleArrayBindingArrayItem(ArrayItem->ItemList,
                                            TopLevelDeclList, DeclNameList);
                break;
            }
            case AST::ArrayBindingItemKind::Object: {
                const auto ObjectItem =
                    llvm::cast<AST::ArrayBindingItemObject>(Item);

                HandleObjectBindingFieldList(ObjectItem->FieldList,
                                             TopLevelDeclList, DeclNameList);
                break;
            }
            case AST::ArrayBindingItemKind::Spread:
                break;
        }
    }

    static void
    HandleArrayBindingArrayItem(
        std::span<AST::ArrayBindingItem *const> ItemList,
        ADT::UnorderedStringMap<AST::LvalueNamedDecl *> &TopLevelDeclList,
        std::vector<ParseUnit::AddError::DeclName> &DeclNameList) noexcept
    {
        for (const auto &Item : ItemList) {
            HandleArrayBindingItem(Item, TopLevelDeclList, DeclNameList);
        }
    }

    static void
    HandleObjectBindingField(
        AST::ObjectBindingField *const Field,
        ADT::UnorderedStringMap<AST::LvalueNamedDecl *> &TopLevelDeclList,
        std::vector<ParseUnit::AddError::DeclName> &DeclNameList) noexcept
    {
        switch (Field->getKind()) {
            case AST::ObjectBindingFieldKind::Identifier: {
                const auto IdentifierField =
                    llvm::cast<AST::ObjectBindingFieldIdentifier>(Field);

                const auto Name = IdentifierField->Name;
                if (TopLevelDeclList.contains(Name)) {
                    DeclNameList.emplace_back(Name, IdentifierField->NameLoc);
                }

                break;
            }
            case AST::ObjectBindingFieldKind::Array: {
                const auto ArrayField =
                    llvm::cast<AST::ObjectBindingFieldArray>(Field);

                HandleArrayBindingArrayItem(ArrayField->ItemList,
                                            TopLevelDeclList, DeclNameList);
                break;
            }
            case AST::ObjectBindingFieldKind::Object: {
                const auto ObjectField =
                    llvm::cast<AST::ObjectBindingFieldObject>(Field);

                HandleObjectBindingField(ObjectField, TopLevelDeclList,
                                         DeclNameList);
                break;
            }
            case AST::ObjectBindingFieldKind::Spread: {
                const auto SpreadField =
                    llvm::cast<AST::ObjectBindingFieldSpread>(Field);

                if (TopLevelDeclList.contains(SpreadField->Key)) {
                    DeclNameList.emplace_back(SpreadField->Key,
                                              SpreadField->KeyLoc);
                }

                break;
            }
        }
    }

    auto ParseUnit::addTopLevelStmt(AST::Stmt *const Stmt) noexcept -> AddError
    {
        this->TopLevelStmtList.emplace_back(Stmt);
        if (const auto Decl = llvm::dyn_cast<AST::LvalueNamedDecl>(Stmt)) {
            if (!this->TopLevelDeclList.contains(Decl->getName())) {
                return AddError::none();
            }

            this->TopLevelDeclList.emplace(Decl->getName(), Decl);
            return AddError::withNameList({
                ParseUnit::AddError::DeclName(Decl->getName(),
                                              Decl->getNameLoc())
            });
        }

        auto DeclNameList = std::vector<ParseUnit::AddError::DeclName>();
        if (const auto Decl =
                llvm::dyn_cast<AST::ArrayBindingVarDecl>(Stmt))
        {
            for (const auto &Item : Decl->getItemList()) {
                HandleArrayBindingItem(Item, this->TopLevelDeclList,
                                       DeclNameList);
            }

            if (!DeclNameList.empty()) {
                return AddError::withNameList(std::move(DeclNameList));
            }

            return AddError::none();
        }

        return AddError::none();
    }

    auto
    ParseUnit::Create(const Lex::TokenBuffer &TokenBuffer,
                      DiagnosticConsumer &Diag,
                      const ParseOptions Options) noexcept -> ParseUnit
    {
        auto TokenStream = Lex::TokenStream(TokenBuffer);
        auto Context = ParseContext(TokenStream, Diag, Options);
        auto Unit = ParseUnit();

        while (!TokenStream.reachedEof()) {
            const auto StmtOpt = ParseStmt(Context);
            if (!StmtOpt.has_value()) {
                break;
            }

            const auto Stmt = StmtOpt.value();
            if (Stmt == nullptr) {
                continue;
            }

            if (!Options.IgnoreUnusedExpressions) {
                if (const auto Expr = llvm::dyn_cast<AST::Expr>(Stmt)){
                    const auto BinOp =
                        llvm::dyn_cast<AST::BinaryOperation>(Stmt);

                    if (BinOp == nullptr || !BinOp->isAssignmentOperation()) {
                        Diag.consume({
                            .Level = DiagnosticLevel::Warning,
                            .Location = Expr->getLoc(),
                            .Message = "Expression result unused"
                        });
                    }
                }
            }

            // All top level statements need ending semicolons, except top level
            // functions and if-statements.

            if (!llvm::isa<AST::FunctionDecl>(Stmt) &&
                !llvm::isa<AST::IfExpr>(Stmt))
            {
                if (!ExpectSemicolon(Context)) {
                    Diag.consume({
                        .Level = DiagnosticLevel::Error,
                        .Location = TokenStream.getCurrentOrPreviousLocation(),
                        .Message = "Expected a semicolon after declaration"
                    });
                }
            }

            if (const auto Error = Unit.addTopLevelStmt(Stmt)) {
                // TODO:
                // Use ranges::join_with when it is available in llvm's libc++.

                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = Error.DeclNameList.front().Loc,
                    .Message =
                        std::format("Decl name '{}' is reused",
                                    Error.DeclNameList.front().Name)
                });
            }
        }

        return Unit;
    }
}
