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

  changeMessage = '';

  constructor(private wasm: WasmService) { }

  onInputChanged(event: Event) {
    const file = event.target.files[0];
    this.wasm.writeFile(file.name, file).subscribe(([filePath]) => {
      this.wasm.isApkValid(filePath).subscribe(([isApkValid]) => {
        this.changeMessage = isApkValid;
      })
    })
  }
}
