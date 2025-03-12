/*
 * Parse/Context.h
 * © suhas pai
 */

#pragma once

#include "Diag/Consumer.h"
#include "Lex/TokenStream.h"

namespace Parse {
    struct ParseOptions {
        bool DontRequireSemicolons : 1 = false;
        bool IgnoreUnusedExpressions : 1 = false;
    };

    struct ParseContext {
        Lex::TokenStream &TokenStream;
        DiagnosticConsumer &Diag;

        ParseOptions Options;

        explicit
        ParseContext(Lex::TokenStream &TokenStream,
                     DiagnosticConsumer &Diag,
                     const ParseOptions Options) noexcept
        : TokenStream(TokenStream), Diag(Diag), Options(Options) {}
    };
}
