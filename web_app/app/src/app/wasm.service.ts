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
    const moduleArgs = { wasmBinary: binary }
    Module(moduleArgs).then((result: Module) =>
      this.module = result,
      this.wasmReady.next(true)
    )
  }

  public isApkValid(filePath: String): Observable<boolean> {
    return this.wasmReady
      .pipe(filter(value => value === true))
      .pipe(map(() => {
        return this.module.isValid(filePath)
      }))
  }

  public getFilePathsInApk(filePath: String): Observable<any> {
    return this.wasmReady
      .pipe(filter(value => value === true))
      .pipe(map(() => {
        return this.module.getFiles(filePath)
      }))
  }

  public getApkProperties(filePath: String): Observable<any> {
    return this.wasmReady
      .pipe(filter(value => value === true))
      .pipe(map(() => {
        return this.module.getProperties(filePath)
      }))
  }

  public readFile = (blob: Blob): Observable<Uint8Array> => {
    const reader = new FileReader();
    reader.readAsArrayBuffer(blob);
    return Observable.create(observer => {
      reader.onerror = err => observer.error(err)
      reader.onabort = err => observer.error(err)
      reader.onloadend = () => observer.complete()
      reader.onload = ((_: ProgressEvent): void => {
        let data = new Uint8Array((<ArrayBuffer>reader.result))
        observer.next(data);
      });
    })
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

  public deleteDataFile(fileName: String): Observable<Boolean> {
    return this.wasmReady
      .pipe(filter(value => value === true))
      .pipe(map(() => {
        const filePath = "/" + fileName
        const fileExists = this.module.FS.analyzePath(filePath, false).exists;
        if (fileExists) {
          this.module.FS.unlink(filePath);
          return true;
        } else {
          return false;
        }
      }))
  }
}
