interface stackInterface<dataType> {
  push(dataItem: dataType): void;
  pop(): dataType | null | undefined;
  peek(): dataType | null;
  isEmpty(): boolean;
  size(): number;
  printStack(): void;
}

export class Stack<dataType> implements stackInterface<dataType> {
  private data: Array<dataType> = [];
  private stackSize: number = 0;

  push(dataItem: dataType): void {
    this.data.push(dataItem);
  }

  pop(): dataType | null | undefined {
    let element = this.data.pop();
    return element;
  }

  peek(): dataType | null {
    let element = this.data[this.size() - 1];
    return element;
  }
  isEmpty(): boolean {
    let result = this.data.length <= 0;
    return result;
  }
  size(): number {
    let len = this.data.length;
    return len;
  }
  printStack(): void {
    console.log("All stack values are printed below!");
    this.data.forEach((dataItem) => {
      console.log(dataItem);
    });
  }
}
