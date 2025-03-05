/*
 * Backend/LLVM/CreateObjectFile.h
 */

#include "Diag/Consumer.h"
#include "Handler.h"

namespace Backend::LLVM {
    struct CreateObjectFile : public Handler {
    protected:
        std::string OutputFilePath;
    public:
        explicit
        CreateObjectFile(DiagnosticConsumer &Diag,
                         std::string_view OutputFilePath) noexcept;
    };
}