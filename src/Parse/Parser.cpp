/*
 * Parse/Parser.cpp
 */

#include "AST/BinaryOperation.h"
#include "AST/DeclRefExpr.h"

#include "AST/Decls/EnumDecl.h"
#include "AST/Decls/FunctionDecl.h"
#include "AST/Decls/LvalueNamedDecl.h"
#include "AST/Decls/ParamVarDecl.h"
#include "AST/Decls/StructDecl.h"

#include "Lex/Token.h"
#include "Parse/OperatorPrecedence.h"
#include "Parse/Parser.h"
#include "Parse/String.h"

#include "llvm/Support/Casting.h"
#include "Sema/Types/Builtin.h"

struct AllQualifiers {
    bool IsMutable : 1 = false;
    bool IsVolatile : 1 = false;
    bool IsRestrict : 1 = false;
    bool IsAtomic : 1 = false;
    bool IsAligned : 1 = false;
    bool IsComptime : 1 = false;
    bool isInline : 1 = false;
    bool isNoReturn : 1 = false;
    bool isNoInline : 1 = false;
};

namespace Parse {
    auto Parser::peek() const noexcept -> std::optional<Lex::Token> {
        if (this->position() >= this->getTokenList().size()) {
            return std::nullopt;
        }

        return this->getTokenList().at(this->position());
    }

    auto Parser::prev() const noexcept -> std::optional<Lex::Token> {
        if (this->position() - 1 >= this->getTokenList().size()) {
            return std::nullopt;
        }

        return this->getTokenList().at(this->position() - 1);
    }

    auto Parser::current() const noexcept -> std::optional<Lex::Token> {
        if (this->position() - 1 >= this->getTokenList().size()) {
            return std::nullopt;
        }

        return this->getTokenList().at(this->position());
    }

    auto Parser::goBack(const uint32_t Skip) noexcept -> bool {
        if (this->position() - 1 - Skip < 0) {
            return false;
        }

        this->Index -= Skip + 1;
        return true;
    }

    auto Parser::consume(const uint32_t Skip) noexcept
        -> std::optional<Lex::Token>
    {
        if (this->position() + Skip >= this->getTokenList().size()) {
            return std::nullopt;
        }

        this->Index += Skip + 1;
        return this->getTokenList().at(this->position() - 1);
    }

    auto Parser::peekIs(Lex::TokenKind Kind) const noexcept -> bool {
        if (const auto TokenOpt = this->peek()) {
            return TokenOpt->Kind == Kind;
        }

        return false;
    }

    auto Parser::peekIsOneOf(
        const std::initializer_list<Lex::TokenKind> &KindList) const noexcept
            -> bool
    {
        if (const auto TokenOpt = this->peek()) {
            return std::find(KindList.begin(), KindList.end(), TokenOpt->Kind)
                != KindList.end();
        }

        return false;
    }

    auto Parser::consumeIfToken(const Lex::TokenKind Kind)
        -> std::optional<Lex::Token>
    {
        if (const auto TokenOpt = this->peek()) {
            const auto Token = TokenOpt.value();
            if (Token.Kind == Kind) {
                Index++;
                return Token;
            }
        }

        return std::nullopt;
    }

    auto
    Parser::expect(const Lex::TokenKind Kind, const bool Optional) noexcept
        -> bool
    {
        auto TokenOpt = this->consume();
        if (TokenOpt.has_value()) {
            if (TokenOpt.value().Kind == Kind) {
                return true;
            }

            if (Kind == Lex::TokenKind::EOFToken) {
                return false;
            }

            this->Index--;
        } else if (Kind == Lex::TokenKind::EOFToken) {
            return true;
        }

        return Optional;
    }

    auto Parser::tokenContent(const Lex::Token Token) const noexcept
        -> std::string_view
    {
        return Token.getString(SourceMngr.getText());
    }

    auto Parser::tokenKeyword(const Lex::Token Token) const noexcept
        -> Lex::Keyword
    {
        assert(Token.Kind == Lex::TokenKind::Keyword);
        return Lex::KeywordTokenGetKeyword(this->tokenContent(Token));
    }

    auto Parser::getCurrOrPrevLoc() const noexcept -> SourceLocation {
        if (const auto Current = this->current()) {
            return Current->Loc;
        }

        if (const auto Prev = this->prev()) {
            return Prev->Loc;
        }

        return SourceLocation::invalid();
    }

    auto Parser::parseUnaryOper(const Lex::Token Token) noexcept
        -> AST::UnaryOperation *
    {
        const auto TokenString = this->tokenContent(Token);
        const auto UnaryOpOpt =
            Parse::UnaryOperatorToLexemeMap.keyFor(TokenString);

        if (!UnaryOpOpt.has_value()) {
            Diag.emitInternalError("unary operator not lexed correctly");
            return nullptr;
        }

        return new AST::UnaryOperation(Token.Loc, UnaryOpOpt.value());
    }

    auto Parser::parseNumberLiteral(const Lex::Token Token) noexcept
        -> AST::NumberLiteral *
    {
        const auto ParseResult = Parse::ParseNumber(this->tokenContent(Token));
        switch (ParseResult.Error) {
            case ParseNumberError::None:
                break;
            case ParseNumberError::EmptyString:
                assert(false && "NumberLiteral token-string is empty");
            case ParseNumberError::NegativeNumber:
                Diag.emitError(Token.Loc, "Signed integers can't be negative");
                break;
            case ParseNumberError::PositiveSign:
                Diag.emitError(Token.Loc, "Integers can't have a +sign");
                break;
            case ParseNumberError::InvalidDigit:
                Diag.emitError(Token.Loc, "Invalid digit in number");
                break;
            case ParseNumberError::UnrecognizedChar:
                Diag.emitError(Token.Loc, "Unrecognized character in number");
                break;
            case ParseNumberError::UnrecognizedBase:
                Diag.emitError(Token.Loc, "Unrecognized base in number");
                break;
            case ParseNumberError::TooLarge:
                Diag.emitError(Token.Loc, "Number is too large");
                break;
            case ParseNumberError::LeadingZero:
                Diag.emitError(Token.Loc, "Number cannot have a leading zero");
                break;
            case ParseNumberError::NegativeZero:
                Diag.emitError(Token.Loc, "Negative zero cannot be valid");
                break;
            case ParseNumberError::FloatingPoint:
                Diag.emitError(Token.Loc,
                               "Floating-point number not supported");
                break;
            case ParseNumberError::Overflow:
                Diag.emitError(Token.Loc, "Number is too large");
                break;
        }

        return new AST::NumberLiteral(Token.Loc, ParseResult);
    }

    auto Parser::parseCharLiteral(const Lex::Token Token) noexcept
        -> AST::CharLiteral *
    {
        const auto TokenString = this->tokenContent(Token);
        if (TokenString.size() == 2) {
            Diag.emitError(Token.Loc,
                           "Character Literals cannot be empty. Use \'\\0\' to "
                           "store a null-character");
            return nullptr;
        }

        const auto FirstChar = TokenString.at(1);
        if (FirstChar != '\\') {
            if (TokenString.size() != 3) {
                Diag.emitError(Token.Loc,
                               "Use double quotes to store strings with "
                               "multiple characters");
                return nullptr;
            }

            return new AST::CharLiteral(Token.Loc, FirstChar);
        }

        if (FirstChar == '\\' && TokenString.size() == 3) {
            Diag.emitError(Token.Loc,
                           "Expected another character to parse "
                           "escape-sequence. Use '\\\\' to store a "
                           "back-slash");
            return nullptr;
        }

        const auto EscapedChar = VerifyEscapeSequence(TokenString.at(2));
        if (EscapedChar == '\0') {
            Diag.emitError(Token.Loc, "Invalid escape-sequence");
            return nullptr;
        }

        return new AST::CharLiteral(Token.Loc, EscapedChar);
    }

    auto Parser::parseStringLiteral(const Lex::Token Token) noexcept
        -> AST::StringLiteral *
    {
        const auto TokenString = this->tokenContent(Token);
        const auto TokenStringLiteral =
            TokenString.substr(1, TokenString.size() - 1);

        auto String = std::string();
        String.reserve(TokenStringLiteral.size());

        for (auto I = uint32_t(); I != TokenStringLiteral.size(); ++I) {
            auto Char = TokenStringLiteral.at(I);
            if (Char != '\\') {
                String.append(1, Char);
                continue;
            }

            I++;
            if (I == TokenStringLiteral.size()) {
                Diag.emitError(Token.Loc,
                               "Expected another character to parse "
                               "escape-sequence");
                return nullptr;
            }

            const auto NextChar =
                VerifyEscapeSequence(TokenStringLiteral.at(I));

            if (NextChar == '\0') {
                Diag.emitError(Token.Loc, "Invalid escape-sequence");
                return nullptr;
            }

            String.append(1, NextChar);
        }

        return new AST::StringLiteral(Token.Loc, String);
    }

    auto Parser::parseParenExpr(const Lex::Token Token) noexcept
        -> AST::ParenExpr *
    {
        const auto ChildExprOpt = this->parseExpression();
        if (!ChildExprOpt.has_value()) {
            Diag.emitError(Token.Loc, "Unexpected semicolon/eof");
            return nullptr;
        }

        if (!this->expect(Lex::TokenKind::CloseParen)) {
            Diag.emitError(Token.Loc, "Expected closing parenthesis");
            return nullptr;
        }

        return new AST::ParenExpr(Token, ChildExprOpt.value());
    }

    auto
    Parser::parseFieldExpr(const Lex::Token DotToken,
                           AST::Expr *const Lhs) noexcept -> AST::FieldExpr *
    {
        auto MemberToken = Lex::Token();
        if (DotToken.Kind != Lex::TokenKind::DotIdentifier) {
            const auto MemberTokenOpt =
                this->consumeIfToken(Lex::TokenKind::Identifier);

            if (!MemberTokenOpt.has_value()) {
                Diag.emitError(DotToken.Loc, "Expected member name");
                return nullptr;
            }

            MemberToken = MemberTokenOpt.value();
        } else {
            MemberToken = DotToken;
        }

        if (MemberToken.Kind != Lex::TokenKind::Identifier
         && MemberToken.Kind != Lex::TokenKind::IntegerLiteral
         && MemberToken.Kind != Lex::TokenKind::Star
         && MemberToken.Kind != Lex::TokenKind::DotIdentifier)
        {
            Diag.emitError(MemberToken.Loc,
                           "Expected member name, got '%s' instead",
                           this->tokenContent(MemberToken).data());

            return nullptr;
        }

        auto MemberName = this->tokenContent(MemberToken);
        if (MemberToken.Kind == Lex::TokenKind::DotIdentifier) {
            MemberName = MemberName.substr(1);
        }

        const auto IsArrow = MemberToken.Kind == Lex::TokenKind::ThinArrow;
        return new AST::FieldExpr(MemberToken.Loc, Lhs, IsArrow,
                                  std::move(MemberName));
    }

    auto
    Parser::parseCallExpr(AST::Expr *const Callee,
                          const Lex::Token ParenToken) noexcept
        -> AST::CallExpr *
    {
        auto ArgList = std::vector<AST::Expr *>();
        if (this->consumeIfToken(Lex::TokenKind::CloseParen)) {
            goto done;
        }

        do {
            if (const auto Expr = this->parseExpression()) {
                if (Expr.value() == nullptr) {
                    return nullptr;
                }

                ArgList.emplace_back(Expr.value());
            } else {
                return nullptr;
            }

            const auto ArgEndTokenOpt = this->consume();
            if (!ArgEndTokenOpt.has_value()) {
                Diag.emitError(ParenToken.Loc,
                               "Expected end of function parameter list");
                return nullptr;
            }

            const auto ArgEndToken = ArgEndTokenOpt.value();
            if (ArgEndToken.Kind == Lex::TokenKind::CloseParen) {
                break;
            }

            if (ArgEndToken.Kind != Lex::TokenKind::Comma) {
                Diag.emitError(ArgEndToken.Loc,
                               "Expected comma or closing parenthesis");
                return nullptr;
            }
        } while (true);

done:
        return new AST::CallExpr(Callee, ParenToken.Loc,
                                 AST::CallExpr::Qualifiers(),
                                 std::move(ArgList));
    }

    auto
    Parser::parseArraySubscriptExpr(AST::Expr *const Lhs,
                                    const Lex::Token BracketToken) noexcept
        -> AST::ArraySubscriptExpr *
    {
        const auto Expr = this->parseExpression();
        if (!Expr.has_value()) {
            return nullptr;
        }

        if (!this->expect(Lex::TokenKind::RightSquareBracket)) {
            Diag.emitError(BracketToken.Loc,
                           "Expected closing square bracket");
            return nullptr;
        }

        return new AST::ArraySubscriptExpr(BracketToken.Loc, Lhs, Expr.value());
    }

    auto
    Parser::parseCallAndFieldExpr(AST::Expr *Root,
                                  const Lex::Token Token) noexcept
        -> AST::Expr *
    {
        const auto TokenList = {
            Lex::TokenKind::Dot,
            Lex::TokenKind::DotIdentifier,
            Lex::TokenKind::ThinArrow,
            Lex::TokenKind::LeftSquareBracket,
            Lex::TokenKind::OpenParen
        };

        while (this->peekIsOneOf(TokenList)) {
            const auto Token = this->consume().value();
            if (Token.Kind == Lex::TokenKind::Dot
             || Token.Kind == Lex::TokenKind::ThinArrow
             || Token.Kind == Lex::TokenKind::DotIdentifier)
            {
                Root = this->parseFieldExpr(Token, Root);
                if (Root == nullptr) {
                    return nullptr;
                }

                continue;
            }

            if (Token.Kind == Lex::TokenKind::LeftSquareBracket) {
                Root = this->parseArraySubscriptExpr(Root, Token);
                if (Root == nullptr) {
                    return nullptr;
                }

                continue;
            }

            if (Token.Kind == Lex::TokenKind::OpenParen) {
                Root = this->parseCallExpr(Root, Token);
                if (Root == nullptr) {
                    return nullptr;
                }

                continue;
            }

            __builtin_unreachable();
        }

        return Root;
    }

    auto Parser::parseIdentifierForLhs(const Lex::Token IdentToken) noexcept
        -> AST::Expr *
    {
        auto Root = static_cast<AST::Expr *>(nullptr);
        if (IdentToken.Kind == Lex::TokenKind::Identifier) {
            Root =
                new AST::DeclRefExpr(this->tokenContent(IdentToken),
                                     IdentToken.Loc);
        } else if (IdentToken.Kind == Lex::TokenKind::DotIdentifier) {
            this->goBack();
        } else {
            __builtin_unreachable();
        }

        return this->parseCallAndFieldExpr(Root, IdentToken);
    }

    auto Parser::parseKeywordForLhs(const Lex::Token KeywordTok) noexcept
        -> AST::Expr *
    {
        auto UnusedNameTokenOpt = std::optional<Lex::Token>();
        auto Root = static_cast<AST::Expr *>(nullptr);

        const auto Keyword = this->tokenKeyword(KeywordTok);
        if (Keyword == Lex::Keyword::Struct) {
            Root = this->parseStructDecl(UnusedNameTokenOpt);
            if (Root == nullptr) {
                return nullptr;
            }
        } else if (Keyword == Lex::Keyword::Enum) {
            Root = this->parseEnumDecl(UnusedNameTokenOpt);
            if (Root == nullptr) {
                return nullptr;
            }
        } else if (Keyword == Lex::Keyword::If) {
            Root = this->parseIfStmt(KeywordTok);
            if (Root == nullptr) {
                return nullptr;
            }
        } else {
            Diag.emitError(this->getCurrOrPrevLoc(),
                           "Keyword \"" SV_FMT "\" cannot be used in an "
                           "expression",
                           SV_FMT_ARG(this->tokenContent(KeywordTok)));

            return nullptr;
        }

        return this->parseCallAndFieldExpr(Root, KeywordTok);
    }

    auto Parser::parseLhs() noexcept -> AST::Expr * {
        // Parse Prefixes and then Lhs
        auto Root = static_cast<AST::Expr *>(nullptr);
        auto CurrentOper = static_cast<AST::UnaryOperation *>(nullptr);

        while (true) {
            const auto TokenOpt = this->consume();
            if (!TokenOpt.has_value()) {
                return Root;
            }

            const auto Token = TokenOpt.value();
            auto Expr = static_cast<AST::Expr *>(nullptr);

            if (!Lex::TokenKindIsUnaryOp(Token.Kind)
             && Token.Kind != Lex::TokenKind::Minus)
            {
                switch (Token.Kind) {
                    case Lex::TokenKind::Identifier:
                    case Lex::TokenKind::DotIdentifier:
                        Expr = this->parseIdentifierForLhs(Token);
                        break;
                    case Lex::TokenKind::Keyword:
                        Expr = this->parseKeywordForLhs(Token);
                        break;
                    case Lex::TokenKind::IntegerLiteral:
                        Expr = this->parseNumberLiteral(Token);
                        break;
                    case Lex::TokenKind::FloatLiteral:
                        Diag.emitError(Token.Loc,
                                       "Floating-point is not supported");
                        return nullptr;
                    case Lex::TokenKind::CharLiteral:
                        Expr = this->parseCharLiteral(Token);
                        break;
                    case Lex::TokenKind::StringLiteral:
                        Expr = this->parseStringLiteral(Token);
                        break;
                    case Lex::TokenKind::OpenParen:
                        Expr = this->parseParenExpr(Token);
                        break;
                    case Lex::TokenKind::LeftSquareBracket:
                        Expr = this->parseArrayDecl(Token);
                        break;
                    default:
                        Diag.emitError(Token.Loc,
                                       "Unexpected token \"" SV_FMT "\"",
                                       SV_FMT_ARG(this->tokenContent(Token)));
                        return nullptr;
                }
            } else {
                Expr = this->parseUnaryOper(Token);
            }

            if (Expr == nullptr) {
                return nullptr;
            }

            if (Root == nullptr) {
                Root = Expr;
            } else {
                CurrentOper->setOperand(*Expr);
            }

            if (Expr->getKind() == AST::NodeKind::UnaryOperation) {
                CurrentOper = static_cast<AST::UnaryOperation *>(Expr);
                continue;
            }

            return Root;
        }

        __builtin_unreachable();
    }

    auto Parser::parseBinOpRhs(AST::Expr *Lhs, const uint32_t MinPrec) noexcept
        -> AST::Expr *
    {
        do {
            auto TokenOpt = this->peek();
            if (!TokenOpt.has_value()) {
                return Lhs;
            }

            auto Token = TokenOpt.value();
            if (!Lex::TokenKindIsBinOp(Token.Kind)) {
                return Lhs;
            }

            const auto ThisOperInfoOpt = Parse::OperatorInfoMap[Token.Kind];
            DIAG_ASSERT(Diag,
                        ThisOperInfoOpt.has_value(),
                        "OperatorInfoMap is missing a bin-op");

            const auto ThisOperInfo = ThisOperInfoOpt.value();
            if (static_cast<uint64_t>(ThisOperInfo.Precedence) < MinPrec) {
                return Lhs;
            }

            this->consume();
            if (!this->peek().has_value()) {
                Diag.emitError(Token.Loc,
                               "Expected an expression after \"" SV_FMT "\"",
                               SV_FMT_ARG(this->tokenContent(Token)));
                return nullptr;
            }

            auto Rhs = this->parseLhs();
            if (Rhs == nullptr) {
                return nullptr;
            }

            const auto BinOpToken = Token;
            if ((TokenOpt = this->peek())
             && Lex::TokenKindIsBinOp(TokenOpt.value().Kind))
            {
                Token = TokenOpt.value();

                const auto ThisIsRightAssoc =
                    ThisOperInfo.Assoc == Parse::OperatorAssoc::Right;
                const auto &NextOperInfoOpt =
                    Parse::OperatorInfoMap[Token.Kind];

                DIAG_ASSERT(Diag,
                            NextOperInfoOpt.has_value(),
                            "OperatorInfoMap missing entry for token %s",
                            Lex::TokenKindGetName(Token.Kind).data());

                const auto NextOperInfo = NextOperInfoOpt.value();
                if (ThisOperInfo.Precedence < NextOperInfo.Precedence
                 || (ThisOperInfo.Precedence == NextOperInfo.Precedence
                  && ThisIsRightAssoc))
                {
                    const auto NextMinPrec =
                        static_cast<uint64_t>(
                            static_cast<uint64_t>(ThisOperInfo.Precedence)
                          + !ThisIsRightAssoc);

                    Rhs = this->parseBinOpRhs(Rhs, NextMinPrec);
                    if (Rhs == nullptr) {
                        return Rhs;
                    }
                }
            }

            const auto BinOp =
                LexTokenToBinaryOperator(BinOpToken,
                                         this->tokenContent(BinOpToken));
            DIAG_ASSERT(Diag,
                        BinOp.has_value(),
                        "LexTokenKindToBinaryOperatorMap missing BinOp");

            Lhs = new AST::BinaryOperation(BinOp.value(), Token.Loc, Lhs, Rhs);
        } while (true);
    }

    auto Parser::parseExpression(const bool ExprIsOptional) noexcept
        -> std::optional<AST::Expr *>
    {
        const auto Peek = this->peek();
        if (!Peek.has_value()
         || Peek.value().Kind == Lex::TokenKind::Semicolon
         || Peek.value().Kind == Lex::TokenKind::EOFToken)
        {
            if (!ExprIsOptional) {
                Diag.emitError(Peek->Loc, "Expected an expression");
                return std::nullopt;
            }

            return nullptr;
        }

        return this->parseBinOpRhs(this->parseLhs(), /*MinPrec=*/0);
    }

    auto
    Parser::parseExpressionAndEnd(const bool ExprIsOptional,
                                  const bool ParseMultipleSemicolons) noexcept
        -> AST::Expr *
    {
        const auto ExprOpt = this->parseExpression(ExprIsOptional);
        if (!ExprOpt.has_value()) {
            return nullptr;
        }

        if (!this->expect(Lex::TokenKind::Semicolon,
                          /*Optional=*/Options.DontRequireSemicolons))
        {
            Diag.emitError(this->getCurrOrPrevLoc(),
                           "Expected a semicolon after expression");
            return nullptr;
        }

        if (ParseMultipleSemicolons) {
            // Skip past multiple semicolons
            while (this->consumeIfToken(Lex::TokenKind::Semicolon)) {
                continue;
            }
        }

        return ExprOpt.value();
    }

    auto Parser::parseTypeQualifiers() noexcept
        -> std::optional<Sema::TypeQualifiers>
    {
        // TODO: Enforce specific ordering of qualifiers.

        auto seenMutable = false;
        auto seenVolatile = false;

        auto Qual = Sema::TypeQualifiers();
        while (true) {
            if (this->peekIs(Lex::TokenKind::Keyword)) {
                const auto NextToken = this->consume().value();
                const auto Keyword = this->tokenKeyword(NextToken);

                if (Keyword == Lex::Keyword::Mut) {
                    if (seenMutable) {
                        Diag.emitError(NextToken.Loc,
                                       "Duplicate 'mut' qualifier");
                        return std::nullopt;
                    }

                    Qual.setIsMutable(true);
                    seenMutable = true;
                } else if (Keyword == Lex::Keyword::Volatile) {
                    if (seenVolatile) {
                        Diag.emitError(NextToken.Loc,
                                       "Duplicate 'volatile' qualifier");
                        return std::nullopt;
                    }

                    Qual.setIsVolatile(true);
                    seenVolatile = true;
                } else {
                    break;
                }

                continue;
            }

            break;
        }

        return Qual;
    }

    auto
    Parser::parseTypeExprInstList(
        std::vector<AST::TypeRef::Inst *> &InstList) noexcept -> bool
    {
        const auto NextTokenOpt = this->peek();
        if (!NextTokenOpt.has_value()) {
            Diag.emitError(this->getCurrOrPrevLoc(),
                           "Expected a type expression");
            return false;
        }

        while (true) {
            auto Qual = Sema::TypeQualifiers();
            if (this->peekIs(Lex::TokenKind::Keyword)) {
                const auto QualOpt = this->parseTypeQualifiers();
                if (!QualOpt.has_value()) {
                    return false;
                }

                Qual = QualOpt.value();
            }

            if (const auto Token = this->consumeIfToken(Lex::TokenKind::Star)) {
                InstList.emplace_back(
                    new AST::TypeRef::PointerInst(Token->Loc, Qual));

                continue;
            }

            if (const auto TokenOpt =
                    this->consumeIfToken(Lex::TokenKind::Identifier))
            {
                const auto Token = TokenOpt.value();
                const auto Name = this->tokenContent(Token);
                const auto Inst =
                    new AST::TypeRef::TypeInst(Name, Token.Loc, Qual);

                InstList.emplace_back(Inst);
                continue;
            }

            if (this->consumeIfToken(Lex::TokenKind::LeftSquareBracket)) {
                if (this->expect(Lex::TokenKind::RightSquareBracket,
                                 /*Optional=*/true))
                {
                    InstList.emplace_back(
                        new AST::TypeRef::ArrayInst(this->prev()->Loc));

                    continue;
                }

                Diag.emitError(this->getCurrOrPrevLoc(),
                               "Unexpected info in array type");
                return false;
            }

            if (const auto TokenOpt =
                    this->consumeIfToken(Lex::TokenKind::Keyword))
            {
                const auto Token = TokenOpt.value();
                Diag.emitError(Token.Loc, "Expected a type expression");

                return false;
            }

            if (this->peekIs(Lex::TokenKind::Semicolon)
             || this->peekIs(Lex::TokenKind::Equal)
             || this->peekIs(Lex::TokenKind::Comma)
             || this->peekIs(Lex::TokenKind::CloseParen)
             || this->peekIs(Lex::TokenKind::EOFToken))
            {
                break;
            }

            const auto Token = this->peek().value();
            Diag.emitError(this->getCurrOrPrevLoc(),
                           "Unexpected token '" SV_FMT "' when parsing type",
                           SV_FMT_ARG(this->tokenContent(Token)));

            return false;
        }

        return true;
    }

    auto Parser::parseTypeExpr() noexcept -> AST::TypeRef * {
        auto InstList = std::vector<AST::TypeRef::Inst *>();
        if (!parseTypeExprInstList(InstList)) {
            return nullptr;
        }

        return new AST::TypeRef(std::move(InstList));
    }

    auto Parser::parseVarQualifiers() noexcept
        -> std::optional<AST::VarQualifiers>
    {
        // TODO: Enforce specific ordering of qualifiers.

        auto seenMutable = false;
        auto seenVolatile = false;
        auto seenInline = false;
        auto seenComptime = false;

        auto Qual = AST::VarQualifiers();
        while (true) {
            const auto NextTokenOpt = this->peek();
            if (!NextTokenOpt.has_value()) {
                Diag.emitError(this->getCurrOrPrevLoc(),
                               "Expected either a qualifier or a variable "
                               "name");
                return std::nullopt;
            }

            const auto NextToken = NextTokenOpt.value();
            if (NextToken.Kind == Lex::TokenKind::Keyword) {
                const auto KeywordToken = this->consume().value();
                const auto Keyword = this->tokenKeyword(KeywordToken);

                if (Keyword == Lex::Keyword::Mut) {
                    if (seenMutable) {
                        Diag.emitError(NextToken.Loc,
                                       "Duplicate 'mut' qualifier");
                        return std::nullopt;
                    }

                    Qual.setIsMutable(true);
                    seenMutable = true;
                } else if (Keyword == Lex::Keyword::Volatile) {
                    if (seenVolatile) {
                        Diag.emitError(NextToken.Loc,
                                       "Duplicate 'volatile' qualifier");
                        return std::nullopt;
                    }

                    Qual.setIsVolatile(true);
                    seenVolatile = true;
                } else if (Keyword == Lex::Keyword::Inline) {
                    if (seenInline) {
                        Diag.emitError(NextToken.Loc,
                                       "Duplicate 'inline' qualifier");
                        return std::nullopt;
                    }

                    if (seenComptime) {
                        Diag.emitError(NextToken.Loc,
                                       "'comptime' qualifier implies 'inline' "
                                       "qualifier");
                        return std::nullopt;
                    }

                    Qual.setIsInline(true);
                    seenInline = true;
                } else if (Keyword == Lex::Keyword::Comptime) {
                    if (seenComptime) {
                        Diag.emitError(NextToken.Loc,
                                       "Duplicate 'comptime' qualifier");
                        return std::nullopt;
                    }

                    if (seenInline) {
                        Diag.emitError(NextToken.Loc,
                                       "'comptime' qualifier implies 'inline' "
                                       "qualifier");
                        return std::nullopt;
                    }

                    Qual.setIsComptime(true);
                    seenComptime = true;
                } else {
                    Diag.emitError(NextToken.Loc,
                                   "Keyword '" SV_FMT "' is not a valid "
                                   "variable qualifier and cannot be used as a "
                                   "variable name",
                                   SV_FMT_ARG(
                                    this->tokenContent(KeywordToken)));
                    return std::nullopt;
                }

                continue;
            }

            break;
        }

        return Qual;
    }

    auto
    Parser::parseVarDecl(const Lex::Token Token, const bool ParseEnd) noexcept
        -> AST::VarDecl *
    {
        const auto QualifiersOpt = this->parseVarQualifiers();
        if (!QualifiersOpt.has_value()) {
            return nullptr;
        }

        const auto NameTokenOpt = this->consume();
        if (!NameTokenOpt.has_value()) {
            Diag.emitError(Token.Loc,
                           "Expected a name for variable declaration");
            return nullptr;
        }

        const auto NameToken = NameTokenOpt.value();
        switch (NameToken.Kind) {
            case Lex::TokenKind::Identifier:
                break;
            case Lex::TokenKind::Keyword:
                Diag.emitError(NameToken.Loc,
                               "Keyword \"" SV_FMT "\" cannot be used as a "
                               "variable name",
                               SV_FMT_ARG(this->tokenContent(NameToken)));
                return nullptr;
            case Lex::TokenKind::IntegerLiteral:
            case Lex::TokenKind::FloatLiteral:
                Diag.emitError(NameToken.Loc,
                               "Number Literal \"" SV_FMT "\" cannot be used "
                               "as a variable name",
                               SV_FMT_ARG(this->tokenContent(NameToken)));
                return nullptr;
            default:
                Diag.emitError(NameToken.Loc,
                               "Expected a variable name, got \"" SV_FMT "\" "
                               "instead",
                               SV_FMT_ARG(this->tokenContent(NameToken)));
                return nullptr;
        }

        auto TypeRef = static_cast<AST::TypeRef *>(nullptr);
        if (this->consumeIfToken(Lex::TokenKind::Colon)) {
            TypeRef = this->parseTypeExpr();
            if (TypeRef == nullptr) {
                return nullptr;
            }
        }

        if (!this->expect(Lex::TokenKind::Equal)) {
            Diag.emitError(NameToken.Loc,
                           "Variable declarations must have an initial value");
            return nullptr;
        }

        const auto Expr = this->parseExpression();
        if (!Expr.has_value()) {
            return nullptr;
        }

        if (ParseEnd) {
            if (!this->expect(Lex::TokenKind::Semicolon)) {
                Diag.emitError(this->getCurrOrPrevLoc(),
                               "Expected ';' after variable declaration");
                return nullptr;
            }
        }

        return new AST::VarDecl(this->tokenContent(NameToken),
                                NameToken.Loc, QualifiersOpt.value(), TypeRef,
                                Expr.value());
    }

    auto Parser::parseArrayDecl(const Lex::Token LeftBracketToken) noexcept
        -> AST::ArrayDecl *
    {
        if (this->consumeIfToken(Lex::TokenKind::Comma)) {
            Diag.emitError(this->getCurrOrPrevLoc(),
                           "Expected first array element, found ',' "
                           "instead");
            return nullptr;
        }

        auto ElementList = std::vector<AST::Expr *>();
        while (true) {
            const auto Element = this->parseExpression();
            if (!Element.has_value()) {
                return nullptr;
            }

            ElementList.emplace_back(Element.value());
            if (!this->consumeIfToken(Lex::TokenKind::Comma)) {
                break;
            }

            if (this->consumeIfToken(Lex::TokenKind::RightSquareBracket)) {
                Diag.emitError(this->getCurrOrPrevLoc(),
                               "Expected an additional array element after "
                               "comma, but found ']' instead");

                return nullptr;
            }
        }

        if (!this->expect(Lex::TokenKind::RightSquareBracket)) {
            Diag.emitError(this->getCurrOrPrevLoc(),
                           "Expected either ',' for additional elements, or "
                           "']' to terminate array declaration");

            return nullptr;
        }

        return new AST::ArrayDecl(LeftBracketToken.Loc, ElementList);
    }

    auto Parser::parseFuncDecl() noexcept -> AST::LvalueNamedDecl * {
        const auto NameTokenOpt = this->consume();
        if (!NameTokenOpt.has_value()) {
            Diag.emitError(this->getCurrOrPrevLoc(),
                           "Expected a name for function declaration");
            return nullptr;
        }

        const auto NameToken = NameTokenOpt.value();
        switch (NameToken.Kind) {
            case Lex::TokenKind::Identifier:
                break;
            case Lex::TokenKind::Keyword:
                Diag.emitError(NameToken.Loc,
                               "Keyword \"" SV_FMT "\" cannot be used as a "
                               "function name",
                               SV_FMT_ARG(this->tokenContent(NameToken)));
                return nullptr;
            case Lex::TokenKind::IntegerLiteral:
            case Lex::TokenKind::FloatLiteral:
                Diag.emitError(NameToken.Loc,
                               "Number Literal \"" SV_FMT "\" cannot be used "
                               "as a function name",
                               SV_FMT_ARG(this->tokenContent(NameToken)));
                return nullptr;
            default:
                Diag.emitError(NameToken.Loc,
                               "Expected a function name, got \"" SV_FMT "\""
                               "instead",
                               SV_FMT_ARG(this->tokenContent(NameToken)));
                return nullptr;
        }

        if (!this->expect(Lex::TokenKind::OpenParen)) {
            Diag.emitError(NameToken.Loc,
                           "Expected an parenthesis after function name");
            return nullptr;
        }

        auto ParamList = std::vector<AST::ParamVarDecl *>();
        if (!this->consumeIfToken(Lex::TokenKind::CloseParen)) {
            do {
                const auto TokenOpt = this->consume();
                if (!TokenOpt.has_value()) {
                    Diag.emitError(NameToken.Loc,
                                   "Unexpected end of file while parsing "
                                   "function parameter list");
                    return nullptr;
                }

                const auto Token = TokenOpt.value();
                if (Token.Kind != Lex::TokenKind::Identifier) {
                    Diag.emitError(Token.Loc,
                                   "Unexpected token \"" SV_FMT "\" while "
                                   "parsing ",
                                   SV_FMT_ARG(this->tokenContent(Token)));
                    return nullptr;
                }

                if (!this->consumeIfToken(Lex::TokenKind::Colon)) {
                    Diag.emitError(Token.Loc,
                                   "Expected a colon after parameter name."
                                   "Every parameter must have a type");

                    return nullptr;
                }

                const auto Type = this->parseTypeExpr();
                if (Type == nullptr) {
                    return nullptr;
                }

                auto DefaultExpr = static_cast<AST::Expr *>(nullptr);
                if (this->consumeIfToken(Lex::TokenKind::Equal)) {
                    const auto DefaultExprOpt = this->parseExpression();
                    if (!DefaultExprOpt.has_value()) {
                        return nullptr;
                    }

                    DefaultExpr = DefaultExprOpt.value();
                }

                ParamList.emplace_back(
                    new AST::ParamVarDecl(this->tokenContent(Token), Token.Loc,
                                          Type, DefaultExpr));

                const auto CommaOrCloseParenOpt = this->consume();
                if (!CommaOrCloseParenOpt.has_value()) {
                    Diag.emitError(Token.Loc,
                                   "Expected a comma or closing parenthesis");
                    return nullptr;
                }

                const auto CommaOrCloseParen = CommaOrCloseParenOpt.value();
                if (CommaOrCloseParen.Kind == Lex::TokenKind::CloseParen) {
                    break;
                }

                if (CommaOrCloseParen.Kind != Lex::TokenKind::Comma) {
                    Diag.emitError(Token.Loc,
                                   "Expected a comma or closing parenthesis");
                    return nullptr;
                }
            } while (true);
        }

        auto FuncDecl = static_cast<AST::FunctionDecl *>(nullptr);
        if (this->consumeIfToken(Lex::TokenKind::ThinArrow)) {
            const auto Type = this->parseTypeExpr();
            if (Type == nullptr) {
                return nullptr;
            }

            FuncDecl =
                new AST::FunctionDecl(NameToken.Loc,
                                      ParamList,
                                      Type,
                                      /*Body=*/nullptr,
                                      AST::Linkage::Private);
        } else {
            FuncDecl =
                new AST::FunctionDecl(NameToken.Loc,
                                      ParamList,
                                      &Sema::BuiltinType::voidType(),
                                      /*Body=*/nullptr,
                                      AST::Linkage::Private);
        }

        auto Result =
            new AST::LvalueNamedDecl(this->tokenContent(NameToken),
                                     NameToken.Loc,
                                     /*RvalueExpr=*/FuncDecl);

        const auto OpenCurlyOpt = this->peek();
        if (!OpenCurlyOpt.has_value()) {
            Diag.emitError(this->getCurrOrPrevLoc(),
                           "Unexpected eof after function prototype");
            return nullptr;
        }

        const auto OpenCurly = OpenCurlyOpt.value();
        if (OpenCurly.Kind != Lex::TokenKind::OpenCurlyBrace) {
            Diag.emitError(OpenCurly.Loc,
                           "Expected an opening curly brace, "
                           "got " SV_FMT " instead",
                           SV_FMT_ARG(
                           Lex::TokenKindGetName(OpenCurly.Kind)));
            return nullptr;
        }

        this->consume();
        if (const auto Body = this->parseCompoundStmt(OpenCurly)) {
            FuncDecl->setBody(Body);
            return Result;
        }

        return nullptr;
    }

    auto
    Parser::parseFieldList(const Lex::Token NameToken,
                           std::vector<AST::FieldDecl *> &FieldList) noexcept
        -> bool
    {
        if (!this->expect(Lex::TokenKind::OpenCurlyBrace)) {
            Diag.emitError(NameToken.Loc,
                           "Expected an opening curly brace after struct name");
            return false;
        }

        while (true) {
            if (this->consumeIfToken(Lex::TokenKind::CloseCurlyBrace)) {
                return true;
            }

            if (this->peekIs(Lex::TokenKind::Keyword)) {
                const auto Token = this->peek().value();
                Diag.emitError(this->getCurrOrPrevLoc(),
                               "Keyword \"" SV_FMT "\" cannot be used as a "
                               "field name",
                               SV_FMT_ARG(this->tokenContent(Token)));
                return false;
            }

            const auto NameTokenOpt = this->consume();
            if (!NameTokenOpt.has_value()) {
                Diag.emitError(this->getCurrOrPrevLoc(),
                               "Expected a name for field declaration");
                return false;
            }

            const auto NameToken = NameTokenOpt.value();
            if (NameToken.Kind == Lex::TokenKind::Keyword) {
                Diag.emitError(NameToken.Loc,
                               "Keyword \"" SV_FMT "\" cannot be used as a "
                               "field name",
                               SV_FMT_ARG(this->tokenContent(NameToken)));
                return false;
            }

            if (NameToken.Kind != Lex::TokenKind::Identifier) {
                Diag.emitError(NameToken.Loc,
                               "Expected a name for field declaration");
                return false;
            }

            if (!this->expect(Lex::TokenKind::Colon)) {
                Diag.emitError(NameToken.Loc,
                               "Expected a colon after field name");
                return false;
            }

            const auto Type = this->parseTypeExpr();
            if (Type == nullptr) {
                return false;
            }

            auto InitExpr = static_cast<AST::Expr *>(nullptr);
            if (this->consumeIfToken(Lex::TokenKind::Equal)) {
                const auto InitExprOpt = this->parseExpression();
                if (!InitExprOpt.has_value()) {
                    return false;
                }

                InitExpr = InitExprOpt.value();
            }

            if (!this->expect(Lex::TokenKind::Semicolon)) {
                Diag.emitError(NameToken.Loc,
                               "Expected a semicolon after field declaration");
                return false;
            }

            FieldList.emplace_back(new AST::FieldDecl(
                this->tokenContent(NameToken), NameToken.Loc, Type, InitExpr));
        }

        return true;
    }

    [[nodiscard]] auto
    Parser::parseEnumMemberList(
        std::vector<AST::EnumMemberDecl *> &FieldList) noexcept -> bool
    {
        if (!this->expect(Lex::TokenKind::OpenCurlyBrace)) {
            Diag.emitError(this->getCurrOrPrevLoc(),
                           "Expected an opening curly brace after enum name");
            return false;
        }

        if (this->consumeIfToken(Lex::TokenKind::CloseCurlyBrace)) {
            return true;
        }

        while (true) {
            const auto NameTokenOpt = this->consume();
            if (!NameTokenOpt.has_value()) {
                Diag.emitError(this->getCurrOrPrevLoc(),
                               "Expected a name for enum member declaration");
                return false;
            }

            const auto NameToken = NameTokenOpt.value();
            if (NameToken.Kind == Lex::TokenKind::Keyword) {
                Diag.emitError(NameToken.Loc,
                               "Keyword \"" SV_FMT "\" cannot be used as a "
                               "enum member name",
                               SV_FMT_ARG(this->tokenContent(NameToken)));
                return false;
            }

            if (NameToken.Kind != Lex::TokenKind::Identifier) {
                Diag.emitError(NameToken.Loc,
                               "Expected a name for enum member declaration");
                return false;
            }

            auto InitExpr = static_cast<AST::Expr *>(nullptr);
            if (this->consumeIfToken(Lex::TokenKind::Equal)) {
                if (const auto Expr = this->parseExpression()) {
                    InitExpr = Expr.value();
                } else {
                    return false;
                }
            }

            if (this->consumeIfToken(Lex::TokenKind::Semicolon)) {
                Diag.emitError(NameToken.Loc,
                               "Semicolons are not used to separate enum "
                               "members");
                return false;
            }

            FieldList.emplace_back(new AST::EnumMemberDecl(
                this->tokenContent(NameToken), NameToken.Loc, InitExpr));

            if (this->consumeIfToken(Lex::TokenKind::CloseCurlyBrace)) {
                return true;
            }

            if (!this->expect(Lex::TokenKind::Comma)) {
                Diag.emitError(NameToken.Loc,
                               "Expected a comma after enum member name");
                return false;
            }
        }
    }

    auto
    Parser::parseEnumDecl(std::optional<Lex::Token> &NameTokenOptOut) noexcept
        -> AST::EnumDecl *
    {
        if (!this->peekIs(Lex::TokenKind::OpenCurlyBrace)) {
            const auto NameTokenOpt = this->consume();
            if (!NameTokenOpt.has_value()) {
                Diag.emitError(this->getCurrOrPrevLoc(),
                               "Expected a name for enum declaration");
                return nullptr;
            }

            const auto NameToken = NameTokenOpt.value();
            if (NameToken.Kind == Lex::TokenKind::Keyword) {
                Diag.emitError(NameToken.Loc,
                               "Keyword \"" SV_FMT "\" cannot be used as a "
                               "enum name",
                               SV_FMT_ARG(this->tokenContent(NameToken)));
                return nullptr;
            }

            if (NameToken.Kind != Lex::TokenKind::Identifier) {
                Diag.emitError(NameToken.Loc,
                               "Expected a name for enum declaration");
                return nullptr;
            }

            NameTokenOptOut = NameToken;
        }

        auto FieldList = std::vector<AST::EnumMemberDecl *>();
        if (!this->parseEnumMemberList(FieldList)) {
            return nullptr;
        }

        return new AST::EnumDecl(std::move(FieldList));
    }

    auto Parser::parseEnumDeclStmt() noexcept -> AST::LvalueNamedDecl * {
        auto NameTokenOpt = std::optional<Lex::Token>();
        const auto EnumDecl = this->parseEnumDecl(NameTokenOpt);

        if (EnumDecl == nullptr) {
            return nullptr;
        }

        if (!NameTokenOpt.has_value()) {
            Diag.emitError(this->getCurrOrPrevLoc(),
                           "Expected a name for enum declaration");
            return nullptr;
        }

        if (!this->expect(Lex::TokenKind::Semicolon)) {
            Diag.emitError(this->getCurrOrPrevLoc(),
                           "Expected a semicolon after struct declaration");
            return nullptr;
        }

        const auto NameToken = NameTokenOpt.value();
        const auto Result =
            new AST::LvalueNamedDecl(this->tokenContent(NameToken),
                                     NameToken.Loc, EnumDecl);

        return Result;
    }

    auto
    Parser::parseStructDecl(std::optional<Lex::Token> &NameTokenOptOut) noexcept
        -> AST::StructDecl *
    {
        const auto StructKeywordToken = this->current();
        if (!this->peekIs(Lex::TokenKind::OpenCurlyBrace)) {
            const auto NameTokenOpt = this->consume();
            if (!NameTokenOpt.has_value()) {
                Diag.emitError(this->getCurrOrPrevLoc(),
                               "Expected a name for struct declaration");
                return nullptr;
            }

            const auto NameToken = NameTokenOpt.value();
            if (NameToken.Kind == Lex::TokenKind::Keyword) {
                Diag.emitError(NameToken.Loc,
                               "Keyword \"" SV_FMT "\" cannot be used as a "
                               "struct name",
                               SV_FMT_ARG(this->tokenContent(NameToken)));
                return nullptr;
            }

            if (NameToken.Kind != Lex::TokenKind::Identifier) {
                Diag.emitError(NameToken.Loc,
                               "Expected a name for struct declaration");
                return nullptr;
            }

            NameTokenOptOut = NameToken;
        }

        auto FieldList = std::vector<AST::FieldDecl *>();
        if (!this->parseFieldList(StructKeywordToken.value(), FieldList)) {
            return nullptr;
        }

        return new AST::StructDecl(FieldList);
    }

    auto Parser::parseStructDeclStmt() noexcept -> AST::LvalueNamedDecl * {
        auto NameTokenOpt = std::optional<Lex::Token>();
        const auto StructDecl = this->parseStructDecl(NameTokenOpt);

        if (StructDecl == nullptr) {
            return nullptr;
        }

        if (!NameTokenOpt.has_value()) {
            Diag.emitError(this->getCurrOrPrevLoc(),
                           "Expected a name for struct declaration");
            return nullptr;
        }

        if (!this->expect(Lex::TokenKind::Semicolon)) {
            Diag.emitError(this->getCurrOrPrevLoc(),
                           "Expected a semicolon after struct declaration");
            return nullptr;
        }

        const auto NameToken = NameTokenOpt.value();
        const auto Result =
            new AST::LvalueNamedDecl(this->tokenContent(NameToken),
                                     NameToken.Loc, StructDecl);

        return Result;
    }

    auto Parser::parseIfStmt(const Lex::Token IfToken) noexcept -> AST::IfStmt *
    {
        if (!this->expect(Lex::TokenKind::OpenParen)) {
            Diag.emitError(IfToken.Loc,
                           "Expected an opening parenthesis after if");
            return nullptr;
        }

        const auto ConditionOpt = this->parseExpression();
        if (!ConditionOpt.has_value()) {
            return nullptr;
        }

        if (!this->expect(Lex::TokenKind::CloseParen)) {
            Diag.emitError(IfToken.Loc,
                           "Expected a closing parenthesis after if condition");
            return nullptr;
        }

        const auto Then = this->parseStmt(/*ParseEnd=*/false);
        if (Then == nullptr) {
            return nullptr;
        }

        auto ElseStmt = static_cast<AST::Stmt *>(nullptr);
        if (const auto ElseTokenOpt =
                this->consumeIfToken(Lex::TokenKind::Keyword))
        {
            const auto Keyword = this->tokenKeyword(ElseTokenOpt.value());
            if (Keyword == Lex::Keyword::Else) {
                ElseStmt = this->parseStmt(/*ParseEnd=*/false);
                if (ElseStmt == nullptr) {
                    return nullptr;
                }
            }
        }

        const auto Condition = ConditionOpt.value();
        return new AST::IfStmt(IfToken.Loc, Condition, Then, ElseStmt);
    }

    auto
    Parser::parseCommaSeparatedStmtList() noexcept -> AST::CommaSepStmtList * {
        auto StmtList = std::vector<AST::Stmt *>();
        while (true) {
            const auto StmtOpt = this->parseStmt(/*ParseEnd=*/false);
            if (StmtOpt == nullptr) {
                return nullptr;
            }

            StmtList.emplace_back(StmtOpt);
            if (!this->consumeIfToken(Lex::TokenKind::Comma)) {
                break;
            }
        }

        return new AST::CommaSepStmtList(std::move(StmtList));
    }

    auto Parser::parseForStmt(const Lex::Token ForToken) noexcept
        -> AST::ForStmt *
    {
        if (!this->expect(Lex::TokenKind::OpenParen)) {
            Diag.emitError(ForToken.Loc,
                           "Expected an opening parenthesis after for");
            return nullptr;
        }

        const auto InitList = this->parseCommaSeparatedStmtList();
        if (!this->expect(Lex::TokenKind::Semicolon)) {
            Diag.emitError(ForToken.Loc,
                           "Expected a semicolon after for initialization");
            return nullptr;
        }

        const auto ConditionOpt = this->parseExpression();
        if (!ConditionOpt.has_value()) {
            return nullptr;
        }

        if (!this->expect(Lex::TokenKind::Semicolon)) {
            Diag.emitError(ForToken.Loc,
                           "Expected a semicolon after for condition");
            return nullptr;
        }

        const auto Step = this->parseCommaSeparatedStmtList();
        if (Step== nullptr) {
            return nullptr;
        }

        if (!this->expect(Lex::TokenKind::CloseParen)) {
            Diag.emitError(ForToken.Loc,
                           "Expected a closing parenthesis after for");
            return nullptr;
        }

        const auto Body = this->parseStmt(/*ParseEnd=*/false);
        if (Body == nullptr) {
            return nullptr;
        }

        return new AST::ForStmt(ForToken.Loc, InitList, ConditionOpt.value(),
                                Step, Body);
    }

    auto Parser::parseReturnStmt(const Lex::Token ReturnToken) noexcept
        -> AST::ReturnStmt *
    {
        const auto Expr = this->parseExpressionAndEnd(/*ExprIsOptional=*/true);
        return new AST::ReturnStmt(ReturnToken.Loc, Expr);
    }

    auto Parser::parseCompoundStmt(const Lex::Token CurlyToken) noexcept
        -> AST::CompoundStmt *
    {
        auto StmtList = std::vector<AST::Stmt *>();
        while (true) {
            const auto TokenOpt = this->peek();
            if (!TokenOpt.has_value()) {
                Diag.emitError(CurlyToken.Loc,
                               "Expected closing curly-bracket");
                return nullptr;
            }

            if (this->consumeIfToken(Lex::TokenKind::CloseCurlyBrace)) {
                break;
            }

            if (const auto Stmt = this->parseStmt(/*ParseEnd=*/true)) {
                StmtList.emplace_back(Stmt);
                continue;
            }

            return nullptr;
        }

        return new AST::CompoundStmt(CurlyToken.Loc, std::move(StmtList));
    }

    auto Parser::parseStmt(const bool ParseEnd) noexcept -> AST::Stmt * {
        const auto TokenOpt = this->consume();
        if (!TokenOpt.has_value()) {
            Diag.emitError(this->getCurrOrPrevLoc(), "Unexpected end of file");
            return nullptr;
        }

        const auto Token = TokenOpt.value();
        switch (Token.Kind) {
            case Lex::TokenKind::Keyword: {
                switch (this->tokenKeyword(Token)) {
                    case Lex::Keyword::Let:
                    case Lex::Keyword::Mut: // FIXME: 'mut' by itself is invalid
                        return this->parseVarDecl(Token, ParseEnd);
                    case Lex::Keyword::Function:
                        return this->parseFuncDecl();
                    case Lex::Keyword::If:
                        return this->parseIfStmt(Token);
                    case Lex::Keyword::Else:
                        return this->parseExpressionAndEnd();
                    case Lex::Keyword::Return:
                        return this->parseReturnStmt(Token);
                    case Lex::Keyword::Volatile:
                        Diag.emitError(Token.Loc,
                                       "Unexpected 'volatile' keyword");
                        return nullptr;
                    case Lex::Keyword::Struct:
                        return this->parseStructDeclStmt();
                    case Lex::Keyword::Enum:
                        return this->parseEnumDeclStmt();
                    case Lex::Keyword::For:
                        return this->parseForStmt(Token);
                    case Lex::Keyword::And:
                        Diag.emitError(Token.Loc, "Unexpected 'and' keyword");
                        return nullptr;
                    case Lex::Keyword::Or:
                        Diag.emitError(Token.Loc, "Unexpected 'or' keyword");
                        return nullptr;
                }

                [[fallthrough]];
            }
            case Lex::TokenKind::OpenCurlyBrace:
                return this->parseCompoundStmt(Token);
            default:
                Index--;
                if (ParseEnd) {
                    return this->parseExpressionAndEnd();
                }

                if (const auto ExprOpt = this->parseExpression()) {
                    return ExprOpt.value();
                }

                return nullptr;
        }

        return nullptr;
    }

    auto Parser::startParsing() noexcept -> bool {
        while (const auto Token = this->peek()) {
            if (const auto Stmt = this->parseStmt(/*ParseEnd=*/true)) {
                if (const auto DeclStmt =
                        llvm::dyn_cast<AST::LvalueNamedDecl>(Stmt))
                {
                    this->Context.addDecl(DeclStmt);
                    continue;
                }

                Diag.emitError(Token->Loc,
                               "Unexpected non-declaration statement");
                return false;
            }

            Diag.emitError(Token->Loc, "Unexpected non-statement");
            return false;
        }

        return true;
    }

    auto Parser::parseTopLevelExpressionOrStmt() noexcept -> AST::Stmt * {
        if (const auto Result = this->parseStmt(/*ParseEnd=*/true)) {
            if (const auto Token = this->peek()) {
                Diag.emitError(Token->Loc,
                               "Unexpected token \"" SV_FMT "\"",
                               SV_FMT_ARG(this->tokenContent(*Token)));
                return nullptr;
            }

            if (const auto Decl = llvm::dyn_cast<AST::LvalueNamedDecl>(Result))
            {
                Context.addDecl(Decl);
            }

            return Result;
        }

        return nullptr;
    }
}
