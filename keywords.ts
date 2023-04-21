export enum KeywordKind {
  Let = "let",
}

export let KeywordToLexemeMap = new Map<KeywordKind, string>([
  [KeywordKind.Let, "let"],
]);
