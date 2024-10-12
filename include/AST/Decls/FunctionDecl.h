/*
 * AST/Decls/FunctionDecl.h
 */

#pragma once
#include <vector>

#include "AST/Decls/ParamVarDecl.h"
#include "llvm/Support/Casting.h"
#include "Sema/Types/FunctionProtoype.h"

namespace AST {
    struct FunctionDecl : public ValueDecl {
    public:
        constexpr static auto ObjKind = NodeKind::FunctionDecl;
    protected:
        std::vector<ParamVarDecl *> ParamList;
        std::variant<Sema::Type *, TypeRef *> ReturnTypeOrRef;

        Stmt *Body;
    public:
        constexpr explicit
        FunctionDecl(const std::string_view Name,
                     const SourceLocation NameLoc,
                     std::vector<ParamVarDecl *> &&ParamList,
                     TypeRef &ReturnType,
                     Stmt *const Body,
                     const Linkage Linkage) noexcept
        : ValueDecl(ObjKind, Name, NameLoc, Linkage,
                    static_cast<Sema::Type *>(nullptr)),
          ParamList(std::move(ParamList)), ReturnTypeOrRef(&ReturnType),
          Body(Body) {}

        constexpr explicit
        FunctionDecl(const std::string_view Name,
                     const SourceLocation NameLoc,
                     const std::vector<ParamVarDecl *> &ParamList,
                     TypeRef &ReturnType,
                     Stmt *const Body,
                     const Linkage Linkage) noexcept
        : ValueDecl(ObjKind, Name, NameLoc, Linkage,
                    static_cast<Sema::Type *>(nullptr)),
          ParamList(ParamList), ReturnTypeOrRef(&ReturnType), Body(Body) {}

        constexpr explicit
        FunctionDecl(const std::string_view Name,
                     const SourceLocation NameLoc,
                     const std::vector<ParamVarDecl *> &ParamList,
                     Sema::Type &ReturnType,
                     Stmt *const Body,
                     const Linkage Linkage) noexcept
        : ValueDecl(ObjKind, Name, NameLoc, Linkage,
                    static_cast<Sema::Type *>(nullptr)),
          ParamList(ParamList), ReturnTypeOrRef(&ReturnType), Body(Body) {}

        constexpr explicit
        FunctionDecl(const std::string_view Name,
                     const SourceLocation NameLoc,
                     std::vector<ParamVarDecl *> &&ParamList,
                     Sema::Type &ReturnType,
                     Stmt *const Body,
                     const Linkage Linkage) noexcept
        : ValueDecl(ObjKind, Name, NameLoc, Linkage,
                    static_cast<Sema::Type *>(nullptr)),
          ParamList(std::move(ParamList)), ReturnTypeOrRef(&ReturnType),
          Body(Body) {}

        [[nodiscard]] static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getReturnType() const noexcept {
            return std::get<Sema::Type *>(ReturnTypeOrRef);
        }

        [[nodiscard]] constexpr auto getReturnTypeRef() const noexcept {
            return std::get<TypeRef *>(ReturnTypeOrRef);
        }

        [[nodiscard]] constexpr auto getParamList() const noexcept {
            return ParamList;
        }

        [[nodiscard]] constexpr auto &getParamListRef() noexcept {
            return ParamList;
        }

        [[nodiscard]] constexpr auto &getBody() const noexcept {
            return *Body;
        }

        [[nodiscard]] constexpr auto &getPrototype() const noexcept {
            return llvm::cast<Sema::FunctionPrototype>(*getSemaType());
        }

        constexpr auto setBody(Stmt *const Body) noexcept -> decltype(*this) {
            this->Body = Body;
            return *this;
        }

        constexpr auto setReturnTypeRef(TypeRef *const ReturnType) noexcept
            -> decltype(*this)
        {
            this->ReturnTypeOrRef = ReturnType;
            return *this;
        }

        constexpr auto setReturnType(Sema::Type *const ReturnType) noexcept
            -> decltype(*this)
        {
            this->ReturnTypeOrRef = ReturnType;
            return *this;
        }
    };
}