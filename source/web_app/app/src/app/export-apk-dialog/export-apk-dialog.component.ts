import { Component } from '@angular/core';
import { MatDialogRef } from '@angular/material/dialog';

@Component({
  selector: 'app-export-apk-dialog',
  templateUrl: './export-apk-dialog.component.html',
  styleUrls: ['./export-apk-dialog.component.css']
})
export class ExportApkDialogComponent {

  constructor(private dialogRef: MatDialogRef<ExportApkDialogComponent>) { }
}
