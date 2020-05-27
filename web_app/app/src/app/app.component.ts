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

  onInputChanged() {
    this.wasm.getApkName().subscribe(([apkName]) => {
      this.changeMessage = apkName;
    });
  }
}
