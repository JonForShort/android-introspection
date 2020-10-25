import { Component, ViewChild } from '@angular/core'
import { MatTable } from '@angular/material/table'
import { WasmService } from './wasm.service'

export interface ContentElement {
  path: String,
  type: String
}

export interface PropertyElement {
  key: String,
  value: String
}

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css'],
  providers: [WasmService]
})
export class AppComponent {

  isApkValid: boolean = false;

  contents: ContentElement[] = [];

  contentsDisplayedColumns: string[] = ['path', 'type'];

  properties: PropertyElement[] = [];

  propertiesDisplayedColumns: string[] = ['key', 'value'];

  androidManifest: string = "";

  @ViewChild(MatTable) table: MatTable<Element>;

  constructor(private wasm: WasmService) { }

  onFileDropped(files: Array<File>) {
    const file = files[0]
    this.handleFileInput(file)
  }

  onInputChanged(event: Event) {
    const inputElement = (<HTMLInputElement>event.target)
    const file = inputElement.files[0]
    this.handleFileInput(file)
  }

  private handleFileInput(file: File) {
    this.wasm.deleteDataFile(file.name).subscribe((isDeleted) => {
      this.wasm.readFile(file).subscribe((fileContent) => {
        this.wasm.createDataFile(file.name, fileContent).subscribe((filePath) => {
          this.wasm.isApkValid(filePath).subscribe((isApkValid) => {
            this.isApkValid = isApkValid
            if (this.isApkValid) {

              this.wasm.getFilePathsInApk(filePath).subscribe((filePaths) => {
                this.contents = []
                for (var i = 0; i < filePaths.size(); i++) {
                  this.contents.push({ path: filePaths.get(i), type: "txt" })
                }
                this.table.renderRows
              })

              this.wasm.getApkProperties(filePath).subscribe((properties) => {
                this.properties = []
                const propertiesKeys = properties.keys()
                for (var i = 0; i < propertiesKeys.size(); i++) {
                  const key = propertiesKeys.get(i)
                  const value = properties.get(key)
                  this.properties.push({ key: key, value: value })

                  if (key === "manifest") {
                    this.androidManifest = value
                  }
                }
                this.table.renderRows
              })
            }
          })
        })
      })
    })
  }
}
