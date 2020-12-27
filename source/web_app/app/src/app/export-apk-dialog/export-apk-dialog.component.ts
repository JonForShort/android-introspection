import { Component, Inject } from '@angular/core';
import { MatDialogRef, MAT_DIALOG_DATA } from '@angular/material/dialog';
import { LogService } from '../log/log.service';
import { WasmService } from '../wasm.service';

@Component({
  selector: 'app-export-apk-dialog',
  templateUrl: './export-apk-dialog.component.html',
  styleUrls: ['./export-apk-dialog.component.scss'],
  providers: [WasmService]
})
export class ExportApkDialogComponent {

  isMakeDebuggableChecked = true

  isInjectFridaChecked = false

  constructor(
    private dialogRef: MatDialogRef<ExportApkDialogComponent>,
    private wasm: WasmService,
    private logger: LogService,
    @Inject(MAT_DIALOG_DATA) private data: any
  ) {
    this.dialogRef.afterClosed().subscribe(result => {
      if (result) {
        this.onExportApk(data.file)
      }
    });
  }

  private onExportApk(file: File) {
    this.logger.log("onExportApk: file [${file.name}]")

    this.wasm.readFile(file).subscribe((fileContent) => {
      const blob = new window.Blob([fileContent], { type: file.type });
      this.promptUserToSaveFile(file.name, blob)
    })
  }

  private promptUserToSaveFile(fileName: string, blob: Blob) {
    const downloadAncher = document.createElement("a");
    downloadAncher.style.display = "none";
    downloadAncher.href = URL.createObjectURL(blob);
    downloadAncher.download = fileName;
    downloadAncher.click();
  }
}
