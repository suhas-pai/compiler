/*
 * Backend/LLVM/CreateObjectFile.h
 */

#include "Interface/DiagnosticsEngine.h"
#include "Handler.h"

namespace Backend::LLVM {
    struct CreateObjectFile : public Handler {
    protected:
        std::string OutputFilePath;
    public:
        explicit
        CreateObjectFile(Interface::DiagnosticsEngine &Diag,
                         std::string_view OutputFilePath) noexcept;
    };
}