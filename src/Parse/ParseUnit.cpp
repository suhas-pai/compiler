/*
 * Parse/ParseUnit.cpp
 * © suhas pai
 */

#include "AST/Decls/FunctionDecl.h"
#include "AST/BinaryOperation.h"

#include "Parse/Context.h"
#include "Parse/ParseMisc.h"
#include "Parse/ParseStmt.h"
#include "Parse/ParseUnit.h"

namespace Parse {
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

            if (!Unit.addTopLevelStmt(Stmt)) {
                const auto Decl = llvm::cast<AST::LvalueNamedDecl>(Stmt);
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = Decl->getNameLoc(),
                    .Message = "Decl name is reused"
                });
            }
        }

        return Unit;
    }
}
