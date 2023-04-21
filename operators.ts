export enum BinaryOperator {
  plus = "+",
  minus = "-",
  mult = "*",
  modulo = "%",
  div = "/",
  power = "**",
}

export enum UnaryOperator {
  BoolNot = "!",
  Invert = "~",
  // Negation = "-" handled as BinaryOperator.minus
}

export enum AssocKind {
  Left,
  Right,
}

export class BinOperatorInfo {
  assoc: AssocKind;
  precedence: number;
}

export const BinOperatorInfoMap = new Map<BinaryOperator, BinOperatorInfo>([
  [BinaryOperator.plus, { assoc: AssocKind.Left, precedence: 0 }],
  [BinaryOperator.minus, { assoc: AssocKind.Left, precedence: 0 }],
  [BinaryOperator.mult, { assoc: AssocKind.Left, precedence: 1 }],
  [BinaryOperator.modulo, { assoc: AssocKind.Left, precedence: 1 }],
  [BinaryOperator.div, { assoc: AssocKind.Left, precedence: 1 }],
  [BinaryOperator.power, { assoc: AssocKind.Right, precedence: 2 }],
]);
