/*
 * AST/FunctionPrototype.h
 */

#pragma once

#include <string>
#include <vector>

#include "AST/Stmt.h"
#include "Backend/LLVM/Handler.h"
#include "Basic/SourceLocation.h"

namespace AST {
    struct FunctionPrototype : public Stmt {
    public:
        struct ParamDecl {
        protected:
            SourceLocation NameLoc;
            std::string Name;
        public:
            constexpr explicit
            ParamDecl(const SourceLocation NameLoc,
                      const std::string_view Name) noexcept
            : NameLoc(NameLoc), Name(Name) {}

            [[nodiscard]] constexpr auto getName() const noexcept {
                return std::string_view(Name);
            }

            [[nodiscard]] constexpr auto getNameLoc() const noexcept {
                return NameLoc;
            }

            constexpr auto setName(const std::string_view Name) noexcept
                -> decltype(*this)
            {
                this->Name = std::string_view(Name);
                return *this;
            }

            constexpr auto setNameLoc(const SourceLocation NameLoc) noexcept
                -> decltype(*this)
            {
                this->NameLoc = NameLoc;
                return *this;
            }

            [[nodiscard]]
            constexpr auto getType(Backend::LLVM::Handler &Handler) noexcept
                -> llvm::Type *
            {
                return llvm::Type::getDoubleTy(Handler.getContext());
            }
        };

        constexpr static auto ObjKind = NodeKind::FunctionPrototype;
    protected:
        SourceLocation NameLoc;

        std::string Name;
        std::vector<ParamDecl> ParamList;
    public:
        constexpr explicit
        FunctionPrototype(const SourceLocation NameLoc,
                          const std::string_view Name,
                          std::vector<ParamDecl> &&ParamList) noexcept
        : Stmt(ObjKind), NameLoc(NameLoc), Name(Name),
          ParamList(std::move(ParamList)) {}

        constexpr explicit
        FunctionPrototype(const SourceLocation NameLoc,
                          const std::string_view Name,
                          const std::vector<ParamDecl> &ParamList) noexcept
        : Stmt(ObjKind), NameLoc(NameLoc), Name(Name),
          ParamList(std::move(ParamList)) {}

        constexpr explicit
        FunctionPrototype(const SourceLocation NameLoc,
                          std::string &&Name,
                          const std::vector<ParamDecl> &ParamList) noexcept
        : Stmt(ObjKind), NameLoc(NameLoc), Name(Name),
          ParamList(std::move(ParamList)) {}

        [[nodiscard]] static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]]
        constexpr auto getName() const noexcept -> std::string_view {
            return Name;
        }

        [[nodiscard]] constexpr auto getNameLoc() const noexcept {
            return NameLoc;
        }

        constexpr auto setName(const std::string_view Name) noexcept
            -> decltype(*this)
        {
            this->Name = Name;
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
    };
}
