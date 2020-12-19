import { AndroidManifestViewComponent } from './android-manifest-view/android-manifest-view.component';
import { AppComponent } from './app.component';
import { AppMaterialModule } from './app-material/app-material.module'
import { AppRoutingModule } from './app-routing.module';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { BrowserModule } from '@angular/platform-browser';
import { DragAndDropFileDirective } from "./directives/drag-and-drop-file-directive"
import { HeaderButtonHoverDirective } from "./directives/header-button-hover-directive"
import { NgModule } from '@angular/core';
import { PermissionsViewComponent } from './permissions-view/permissions-view.component'
import { TelemetryModule } from './telemetry/telemetry.module';

@NgModule({
  declarations: [
    AppComponent,
    AndroidManifestViewComponent,
    DragAndDropFileDirective,
    HeaderButtonHoverDirective,
    PermissionsViewComponent
  ],
  imports: [
    AppRoutingModule,
    AppMaterialModule,
    BrowserAnimationsModule,
    BrowserModule,
    TelemetryModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
