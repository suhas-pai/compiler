/*
 * Parse/ParseUnit.h
 * © suhas pai
 */

#pragma once

#include "AST/Decls/LvalueNamedDecl.h"
#include "ADT/StringMap.h"

#include "llvm/Support/Casting.h"
#include "Parse/Context.h"

namespace Parse {
    struct ParseUnit {
    protected:
        std::vector<AST::Stmt *> TopLevelStmtList;
        ADT::UnorderedStringMap<AST::LvalueNamedDecl *> TopLevelDeclList;

        explicit ParseUnit() noexcept = default;
    public:
        [[nodiscard]] static auto
        Create(const Lex::TokenBuffer &TokenBuffer,
               DiagnosticConsumer &Diag,
               ParseOptions Options) noexcept -> ParseUnit;

        [[nodiscard]] constexpr auto getTopLevelStmtList() const noexcept {
            return std::span(this->TopLevelStmtList);
        }

        [[nodiscard]] constexpr auto &getTopLevelDeclList() const noexcept {
            return this->TopLevelDeclList;
        }

        [[nodiscard]]
        inline bool addTopLevelStmt(AST::Stmt *const Stmt) noexcept {
            this->TopLevelStmtList.emplace_back(Stmt);
            if (const auto Decl = llvm::dyn_cast<AST::LvalueNamedDecl>(Stmt)) {
                if (this->TopLevelDeclList.contains(Decl->getName())) {
                    return false;
                }

                this->TopLevelDeclList.emplace(Decl->getName(), Decl);
            }

            return true;
        }

        [[nodiscard]]
        auto findTopLevelDeclByName(const std::string_view Name) noexcept
            -> AST::LvalueNamedDecl *
        {
            if (const auto Iter = this->TopLevelDeclList.find(Name);
                Iter != this->TopLevelDeclList.end())
            {
                return Iter->second;
            }

            return nullptr;
        }
    };
}
