import { NgModule } from '@angular/core';
import { MatDialogModule } from "@angular/material/dialog";
import { MatTableModule } from '@angular/material/table';
import { MatTabsModule } from '@angular/material/tabs';
import { CommonModule } from '@angular/common';

@NgModule({
  declarations: [],
  imports: [
    CommonModule,
    MatDialogModule,
    MatTabsModule,
    MatTableModule
  ],
  exports: [
    MatTabsModule,
    MatTableModule
  ]
})

export class AppMaterialModule { }
