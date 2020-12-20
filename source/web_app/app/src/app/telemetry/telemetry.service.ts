import { Injectable } from '@angular/core';

@Injectable({
  providedIn: 'root'
})
export class TelemetryService {

  constructor() { }

  logEvent(event: string, eventParams?: { [key: string]: any }): void {
    console.log(`sending event : ${event} : ${JSON.stringify(eventParams)}`)
  }
}
