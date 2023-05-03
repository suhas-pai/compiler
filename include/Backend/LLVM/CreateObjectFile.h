/*
 * Backend/LLVM/CreateObjectFile.h
 */

#include "Backend/LLVM/Handler.h"
#include "Interface/DiagnosticsEngine.h"

namespace Backend::LLVM {
    struct CreateObjectFile : public Handler {
    protected:
        std::string OutputFilePath;
    public:
        explicit CreateObjectFile(
            Interface::DiagnosticsEngine &Diag,
            std::string_view OutputFilePath) noexcept;
    };
}