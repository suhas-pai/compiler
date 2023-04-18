interface stackInterface<DataType> {
  push(dataItem: DataType): DataType;
  pop(): DataType | null | undefined;
  peek(): DataType | null;
  empty(): boolean;
  size(): number;
  printStack(): void;
}

export class Stack<DataType> implements stackInterface<DataType> {
  private data: Array<DataType> = [];
  private stackSize: number = 0;

  push(dataItem: DataType): DataType {
    this.data.push(dataItem);
    return dataItem;
  }

  pop(): DataType | null | undefined {
    let element = this.data.pop();
    return element;
  }

  top(): DataType | null | undefined {
    if (this.data.length != 0) {
      return this.data.at(-1);
    }

    return undefined;
  }

  peek(): DataType | null {
    let element = this.data[this.size() - 1];
    return element;
  }
  empty(): boolean {
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
