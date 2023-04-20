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

export function binOpPrecedence(operator: BinaryOperator): number {
  switch (operator) {
    case BinaryOperator.plus:
    case BinaryOperator.minus:
      return 0;
    case BinaryOperator.mult:
    case BinaryOperator.modulo:
    case BinaryOperator.div:
      return 1;
    case BinaryOperator.power:
      return 2;
    default:
      throw "Illegal operator type (How did this happen??)";
  }
}
