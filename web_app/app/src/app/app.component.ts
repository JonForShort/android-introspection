import { Component, ViewChild } from '@angular/core';
import { MatTable } from '@angular/material/table';
import { WasmService } from './wasm.service';

export interface FilePathElement {
  path: String,
  type: String
}

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css'],
  providers: [WasmService]
})
export class AppComponent {

  title = 'app';

  isApkValid = '';

  filePaths: FilePathElement[] = [];

  displayedColumns: string[] = ['path', 'type'];

  @ViewChild(MatTable) table: MatTable<Element>;

  constructor(private wasm: WasmService) { }

  onInputChanged(event: Event) {
    const inputElement = (<HTMLInputElement>event.target)
    const file = inputElement.files[0];
    this.wasm.readFile(file).subscribe((fileContent) => {
      this.wasm.createDataFile(file.name, fileContent).subscribe((filePath) => {
        this.wasm.isApkValid(filePath).subscribe((isApkValid) => {
          this.isApkValid = isApkValid;
          if (this.isApkValid) {
            this.wasm.getFilePathsInApk(filePath).subscribe((filePaths) => {
              this.filePaths = []
              for (var i = 0; i < filePaths.size(); i++) {
                this.filePaths.push({ path: filePaths.get(i), type: "txt" })
              }
              this.table.renderRows
            })
          }
        })
      })
    })
  }
}
