import { Component } from '@angular/core';
import { WasmService } from './wasm.service';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css'],
  providers: [WasmService]
})

export class AppComponent {

  title = 'app';

  isApkValid = '';

  constructor(private wasm: WasmService) { }

  onInputChanged(event: Event) {
    const inputElement = (<HTMLInputElement>event.target)
    const file = inputElement.files[0];
    this.wasm.readFile(file).subscribe((fileContent) => {
      this.wasm.createDataFile(file.name, fileContent).subscribe((filePath) => {
        this.wasm.isApkValid(filePath).subscribe((isApkValid) => {
          this.isApkValid = isApkValid;
        })
      })
    })
  }
}