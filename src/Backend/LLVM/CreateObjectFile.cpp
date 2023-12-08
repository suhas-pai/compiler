/*
 * Backend/LLVM/CreateObjectFile.cpp
 */

#include "Backend/LLVM/CreateObjectFile.h"

namespace Backend::LLVM {
    CreateObjectFile::CreateObjectFile(
        Interface::DiagnosticsEngine &Diag,
        const std::string_view OutputFilePath) noexcept
    : Handler("CreateObjectFile", Diag)
    {}
}