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

  public getApkName(): Observable<string> {
    return this.wasmReady.pipe(filter(value => value === true)).pipe(
      map(() => {
        return this.module.getName();
      })
    )
  }
}
