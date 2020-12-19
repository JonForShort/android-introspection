import { Component, OnChanges, Input, SimpleChanges } from '@angular/core';

@Component({
  selector: 'app-permissions-view',
  templateUrl: './permissions-view.component.html',
  styleUrls: ['./permissions-view.component.scss']
})
export class PermissionsViewComponent implements OnChanges {

  @Input() androidManifest: string = ""

  ngOnChanges(changes: SimpleChanges): void {
  }
}
