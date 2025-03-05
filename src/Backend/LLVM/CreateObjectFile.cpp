/*
 * Backend/LLVM/CreateObjectFile.cpp
 */

#include "Backend/LLVM/CreateObjectFile.h"

namespace Backend::LLVM {
    CreateObjectFile::CreateObjectFile(
        DiagnosticConsumer &Diag,
        const std::string_view OutputFilePath) noexcept
    : Handler("CreateObjectFile", Diag) {}
}