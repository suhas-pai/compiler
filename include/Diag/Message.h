/*
 * Diag/Message.h
 * © suhas pai
 */

#pragma once

#include <string>
#include "Source/SourceLocation.h"

enum class DiagnosticLevel {
    Note,
    Warning,
    Error,
};

struct DiagnosticMessage {
    DiagnosticLevel Level;
    SourceLocation Location;

    std::string Message;
};
