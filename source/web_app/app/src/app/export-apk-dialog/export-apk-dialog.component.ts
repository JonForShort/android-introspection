import { Component } from '@angular/core';
import { MatDialogRef } from '@angular/material/dialog';

@Component({
  selector: 'app-export-apk-dialog',
  templateUrl: './export-apk-dialog.component.html',
  styleUrls: ['./export-apk-dialog.component.scss']
})
export class ExportApkDialogComponent {

  isMakeDebuggableChecked = true

  isInjectFridaChecked = false

  constructor(private dialogRef: MatDialogRef<ExportApkDialogComponent>) {
    this.dialogRef.afterClosed().subscribe(result => {
      if (result) {
        this.onExportApk()
      }
    });
  }

  private onExportApk() {
    console.log("export apk")
  }
}
