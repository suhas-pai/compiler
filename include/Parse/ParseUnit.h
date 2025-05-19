/*
 * Parse/ParseUnit.h
 * © suhas pai
 */

#pragma once

#include "AST/Decls/LvalueNamedDecl.h"
#include "ADT/StringMap.h"

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

        struct AddError {
            enum Code : uint32_t {
                None,
                DeclNameReused
            };

            struct DeclName {
                std::string_view Name;
                SourceLocation Loc;

                constexpr explicit
                DeclName(const std::string_view Name,
                         const SourceLocation Loc) noexcept
                : Name(Name), Loc(Loc) {}
            };

            Code Code;
            std::vector<DeclName> DeclNameList;

            constexpr static auto none() noexcept {
                return AddError{.Code = Code::None, .DeclNameList = {}};
            }

            [[nodiscard]] constexpr static auto
            withNameList(const std::initializer_list<DeclName> &List) noexcept {
                return AddError{
                    .Code = Code::DeclNameReused,
                    .DeclNameList = List
                };
            }

            [[nodiscard]] constexpr
            static auto withNameList(std::vector<DeclName> &&List) noexcept {
                return AddError{
                    .Code = Code::DeclNameReused,
                    .DeclNameList = std::move(List)
                };
            }

            [[nodiscard]] constexpr operator bool() const noexcept {
                return this->Code != Code::None;
            }
        };

        [[nodiscard]]
        auto addTopLevelStmt(AST::Stmt *const Stmt) noexcept -> AddError;

        [[nodiscard]]
        auto findTopLevelDeclByName(const std::string_view Name) const noexcept
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
