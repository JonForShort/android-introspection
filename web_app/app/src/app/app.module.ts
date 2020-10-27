import { AndroidManifestViewComponent } from './android-manifest-view/android-manifest-view.component';
import { AppComponent } from './app.component';
import { AppMaterialModule } from './app-material/app-material.module'
import { AppRoutingModule } from './app-routing.module';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { BrowserModule } from '@angular/platform-browser';
import { DragAndDropFileDirective } from "./directives/drag-and-drop-file-directive"
import { HeaderButtonHoverDirective } from "./directives/header-button-hover-directive"
import { NgModule } from '@angular/core';

@NgModule({
  declarations: [
    AppComponent,
    AndroidManifestViewComponent,
    DragAndDropFileDirective,
    HeaderButtonHoverDirective
  ],
  imports: [
    AppRoutingModule,
    AppMaterialModule,
    BrowserAnimationsModule,
    BrowserModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
