/*
 * AST/FunctionPrototype.h
 */

#pragma once

#include <string>
#include <vector>

#include "AST/Expr.h"
#include "AST/ParamDecl.h"
#include "Basic/SourceLocation.h"

namespace AST {
    struct FunctionPrototype : public Expr {
    protected:
        SourceLocation NameLoc;

        std::string Name;
        std::vector<ParamDecl> ParamList;
    public:
        constexpr explicit
        FunctionPrototype(const SourceLocation NameLoc,
                          const std::string_view Name,
                          std::vector<ParamDecl> &&ParamList) noexcept
        : Expr(ExprKind::FunctionPrototype), NameLoc(NameLoc), Name(Name),
          ParamList(std::move(ParamList)) {}

        constexpr explicit
        FunctionPrototype(const SourceLocation NameLoc,
                          const std::string_view Name,
                          const std::vector<ParamDecl> &ParamList) noexcept
        : Expr(ExprKind::FunctionPrototype), NameLoc(NameLoc), Name(Name),
          ParamList(std::move(ParamList)) {}

        constexpr explicit
        FunctionPrototype(const SourceLocation NameLoc,
                          std::string &&Name,
                          const std::vector<ParamDecl> &ParamList) noexcept
        : Expr(ExprKind::FunctionPrototype), NameLoc(NameLoc), Name(Name),
          ParamList(std::move(ParamList)) {}

        [[nodiscard]] constexpr auto getName() const noexcept {
            return std::string_view(Name);
        }

        constexpr auto setName(std::string_view Name) noexcept
            -> decltype(*this)
        {
            this->Name = std::string_view(Name);
            return *this;
        }

        constexpr auto setName(std::string &&Name) noexcept -> decltype(*this) {
            this->Name = std::move(Name);
            return *this;
        }

        [[nodiscard]] constexpr auto &getParamList() const noexcept {
            return ParamList;
        }

        [[nodiscard]] constexpr auto &getParamListRef() noexcept {
            return ParamList;
        }

        llvm::Value *codegen(Backend::LLVM::Handler &Handler) noexcept override;
    };
}
