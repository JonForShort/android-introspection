import { Injectable } from '@angular/core';
import { TelemetrySender } from './telemetry-sender'

@Injectable({
  providedIn: 'root'
})
export class TelemetryService {

  telemetrySender: TelemetrySender

  constructor() {
    this.telemetrySender = new TelemetrySender()
  }

  sendEvent(event: string, eventParams?: { [key: string]: any }): void {
    this.telemetrySender.sendEvent(event, eventParams)
  }
}
