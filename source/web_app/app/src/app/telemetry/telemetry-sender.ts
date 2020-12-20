export class TelemetrySender {

    constructor() { }

    sendEvent(event: string, eventParams?: { [key: string]: any }): void {
        console.log(`sending event [${event}] params [${JSON.stringify(eventParams)}]`)
    }
}