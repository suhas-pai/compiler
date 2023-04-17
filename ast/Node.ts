import ASTNodeKind from "./kind";

export default interface ASTNode {
  kind: ASTNodeKind;
  children: ASTNode[];
  hasChildren: () => boolean;
  print: () => void;
  run: () => Object;
}
