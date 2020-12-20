import { Component, ViewChild } from '@angular/core'
import { MatTable } from '@angular/material/table'
import { MatTabGroup, MatTab } from '@angular/material/tabs';
import { combineLatest } from 'rxjs';
import { WasmService } from './wasm.service'
import { LogService } from './log/log.service'
import { TelemetryService } from './telemetry/telemetry.service'

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
  styleUrls: ['./app.component.scss'],
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

  @ViewChild("ApkInformationTabGroup", { static: false }) tabGroup: MatTabGroup;

  constructor(private wasm: WasmService, private logger: LogService, private telemetry: TelemetryService) { }

  ngAfterViewInit() {
    this.updateTableState()
  }

  private updateTableState() {
    this.tabGroup._tabs.forEach((item: MatTab, index: number, array: MatTab[]) => {
      item.disabled = !this.isApkValid
    })
    this.table.renderRows
  }

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
    this.logger.log(`handleFileInput: loading apk file [${file.name}]`)

    this.wasm.deleteDataFile(file.name).subscribe((isDeleted) => {
      this.wasm.readFile(file).subscribe((fileContent) => {
        this.wasm.createDataFile(file.name, fileContent).subscribe((filePath) => {
          this.wasm.isApkValid(filePath).subscribe((isApkValid) => {

            this.logger.log(`handleFileInput: loaded apk file is valid [${isApkValid}]`)

            this.isApkValid = isApkValid
            if (this.isApkValid) {
              combineLatest([
                this.wasm.getFilePathsInApk(filePath),
                this.wasm.getApkProperties(filePath)
              ]).subscribe(([filePaths, properties]) => {
                this.updateApkContents(filePaths);
                this.updateApkProperties(properties);
                this.updateTableState()
                this.sendApkLoadedTelemetry(properties)

                this.logger.log(`handleFileInput: completed loading apk file [${file.name}]`)
              })
            }
          })
        })
      })
    })
  }

  private updateApkContents(filePaths) {
    this.contents = []
    for (var i = 0; i < filePaths.size(); i++) {
      this.contents.push({ path: filePaths.get(i), type: "txt" })
    }
  }

  private updateApkProperties(properties) {
    this.properties = []
    const propertiesKeys = properties.keys()
    for (var i = 0; i < propertiesKeys.size(); i++) {
      const key = propertiesKeys.get(i)
      const value = properties.get(key)
      if (key === "manifest") {
        this.androidManifest = value
      } else {
        this.properties.push({ key: key, value: value })
      }
    }
  }

  private sendApkLoadedTelemetry(properties) {
    const eventParams = {}
    const propertiesKeys = properties.keys()
    for (var i = 0; i < propertiesKeys.size(); i++) {
      const key = propertiesKeys.get(i)
      const value = properties.get(key)
      if (key === "manifest") {
        continue
      }
      eventParams[key] = value
    }
    this.telemetry.logEvent("apk_loaded", eventParams)
  }
}
