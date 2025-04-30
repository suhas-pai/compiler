/*
 * Parse/ParseMisc.cpp
 * © suhas pai
 */

#include "Parse/ParseMisc.h"
#include "Lex/Keyword.h"

namespace Parse {
    auto
    ProceedToAndConsumeCommaOrEnd(ParseContext &Context,
                                  const Lex::TokenKind EndKind) noexcept
        -> ProceedResult
    {
        auto &TokenStream = Context.TokenStream;

        const auto SkipToList = { Lex::TokenKind::Comma, EndKind };
        const auto TokenOpt = TokenStream.findNextAndConsumeOneOf(SkipToList);

        if (!TokenOpt.has_value()) {
            return ProceedResult::Failed;
        }

        const auto Token = TokenOpt.value();
        if (Token.Kind == EndKind) {
            return ProceedResult::EndToken;
        }

        return ProceedResult::Comma;
    }

    void
    ParseQualifiers(ParseContext &Context, AST::Qualifiers &Qualifiers) noexcept
    {
        auto &TokenStream = Context.TokenStream;
        auto TokenOpt = TokenStream.peek();

        while (true) {
            if (!TokenOpt.has_value()) {
                break;
            }

            const auto Token = TokenOpt.value();
            if (Token.Kind != Lex::TokenKind::Keyword) {
                break;
            }

            const auto Keyword = TokenStream.tokenKeyword(Token);
            switch (Keyword) {
                case Lex::Keyword::Mut:
                    Qualifiers.setIsMutable(/*IsMutable=*/true, Token.Loc);
                    break;
                case Lex::Keyword::Volatile:
                    Qualifiers.setIsVolatile(/*IsVolatile=*/true, Token.Loc);
                    break;
                case Lex::Keyword::Inline:
                    Qualifiers.setInlinePolicy(Sema::InlinePolicy::Default,
                                               Token.Loc);
                    break;
                case Lex::Keyword::Comptime:
                    Qualifiers.setIsComptime(/*IsComptime=*/true, Token.Loc);
                    break;
                case Lex::Keyword::Let:
                case Lex::Keyword::Function:
                case Lex::Keyword::If:
                case Lex::Keyword::Else:
                case Lex::Keyword::Return:
                case Lex::Keyword::Struct:
                case Lex::Keyword::Class:
                case Lex::Keyword::Shape:
                case Lex::Keyword::Union:
                case Lex::Keyword::Interface:
                case Lex::Keyword::Impl:
                case Lex::Keyword::Enum:
                case Lex::Keyword::And:
                case Lex::Keyword::Or:
                case Lex::Keyword::For:
                case Lex::Keyword::While:
                case Lex::Keyword::Default:
                case Lex::Keyword::In:
                case Lex::Keyword::As:
                case Lex::Keyword::Discardable:
                    return;
            }

            TokenStream.consume();
            TokenOpt = TokenStream.peek();
        }
    }

    void
    MargeQualifiers(AST::Qualifiers &Lhs, const AST::Qualifiers &Rhs) noexcept {
        if (Rhs.isMutable()) {
            Lhs.setIsMutable(/*IsMutable=*/true, Rhs.getMutableLoc());
        }

        if (Rhs.isVolatile()) {
            Lhs.setIsVolatile(/*IsVolatile=*/true, Rhs.getVolatileLoc());
        }

        if (Rhs.isComptime()) {
            Lhs.setIsComptime(/*IsComptime=*/true, Rhs.getComptimeLoc());
        }

        if (Rhs.getInlinePolicy() != Sema::InlinePolicy::None) {
            Lhs.setInlinePolicy(Rhs.getInlinePolicy(), Rhs.getInlineLoc());
        }

        if (Rhs.isExtern()) {
            Lhs.setIsExtern(/*IsExtern=*/true, Rhs.getExternLoc());
        }
    }

    bool ExpectSemicolon(ParseContext &Context) noexcept {
        auto &TokenStream = Context.TokenStream;
        if (!TokenStream.consumeIfIs(Lex::TokenKind::Semicolon)) {
            if (!Context.Options.DontRequireSemicolons) {
                return false;
            }
        }

        return true;
    }

    bool ProceedToSemicolon(ParseContext &Context) noexcept {
        auto &TokenStream = Context.TokenStream;
        if (!TokenStream.proceedToAndConsume(Lex::TokenKind::Semicolon)) {
            if (!Context.Options.DontRequireSemicolons) {
                return false;
            }
        }

        TokenStream.goBack();
        return true;
    }
}
