import { Injectable } from '@angular/core'
import { Observable, BehaviorSubject } from 'rxjs'
import { filter, map } from 'rxjs/operators'

import * as Module from './../assets/js/wasm/wasm.wasm.js'

@Injectable()
export class WasmService {
  module: any

  wasmReady = new BehaviorSubject<boolean>(false)

  constructor() {
    this.instantiateWasm('/assets/js/wasm/wasm.wasm.wasm')
  }

  private async instantiateWasm(url: string) {
    const wasmFile = await fetch(url)
    const buffer = await wasmFile.arrayBuffer()
    const binary = new Uint8Array(buffer)
    const moduleArgs = {
      wasmBinary: binary,
      onRuntimeInitialized: () => {
        this.wasmReady.next(true)
      },
    }
    this.module = Module(moduleArgs)
  }

  public isApkValid(filePath: String): Observable<string> {
    return this.wasmReady
      .pipe(filter(value => value === true))
      .pipe(map(() => {
        return this.module.isValid(filePath)
      }))
  }

  public readFile = (blob: Blob): Observable<Uint8Array> => {
    return Observable.create(obs => {
      const reader = new FileReader();
      reader.onerror = err => obs.error(err);
      reader.onabort = err => obs.error(err);
      reader.onload = () => obs.next(reader.result);
      reader.onloadend = () => obs.complete();
      reader.readAsArrayBuffer(blob);
    });
  }

  public createDataFile(fileName: String, fileContent: Uint8Array): Observable<String> {
    return this.wasmReady
      .pipe(filter(value => value === true))
      .pipe(map(() => {
        const filePath = "/" + fileName
        this.module.FS.createDataFile('/', fileName, fileContent, true, true, true);
        return filePath
      }))
  }
}
