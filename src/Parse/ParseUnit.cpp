/*
 * Parse/ParseUnit.cpp
 * © suhas pai
 */

#include "AST/Decls/FunctionDecl.h"

#include "Parse/Context.h"
#include "Parse/ParseUnit.h"
#include "Parse/ParseMisc.h"
#include "Parse/ParseStmt.h"

namespace Parse {
    auto
    ParseUnit::Create(const Lex::TokenBuffer &TokenBuffer,
                      DiagnosticConsumer &Diag,
                      const Parse::ParseOptions Options) noexcept -> ParseUnit
    {
        auto Stream = Lex::TokenStream(TokenBuffer);
        auto Context = ParseContext(Stream, Diag, Options);
        auto Unit = ParseUnit();

        while (!Stream.reachedEof()) {
            const auto StmtOpt = ParseStmt(Context);
            if (!StmtOpt.has_value()) {
                break;
            }

            const auto Stmt = StmtOpt.value();

            // Every top level statement, except top level functions, need an
            // ending semicolon.

            if (llvm::isa<AST::DeclStmt>(Stmt) &&
                !llvm::isa<AST::FunctionDecl>(Stmt))
            {
                if (!ExpectSemicolon(Context)) {
                    Diag.consume({
                        .Level = DiagnosticLevel::Error,
                        .Location = Stream.getCurrOrPrevLoc(),
                        .Message = "Expected a semicolon after variable "
                                   "declaration"
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
