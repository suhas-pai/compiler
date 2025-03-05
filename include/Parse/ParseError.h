/*
 * Parse/ParseError.h
 * © suhas pai
 */

#pragma once

namespace Parse {
    enum class ParseError {
        None,
        FailedAndProceeded,
        FailedCouldNotProceed
    };
}
