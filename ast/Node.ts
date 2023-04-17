import ASTNodeKind from "./Kind";

export default interface ASTNode {
  kind: ASTNodeKind;
  children: ASTNode[];
  hasChildren: () => boolean;
  print: (prefix: string) => void;
  run: () => Object;
}
