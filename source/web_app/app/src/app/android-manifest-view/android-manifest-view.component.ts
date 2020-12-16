import { OnChanges, Component, Input, SimpleChanges } from '@angular/core';

import * as Prism from 'prismjs'
import * as vkbeautify from 'vkbeautify';

@Component({
  selector: 'app-android-manifest-view',
  templateUrl: './android-manifest-view.component.html',
  styleUrls: ['./android-manifest-view.component.scss']
})
export class AndroidManifestViewComponent implements OnChanges {

  @Input() androidManifest: string = ""

  prettifiedAndroidManifest: string = ""

  ngOnChanges(changes: SimpleChanges): void {
    this.prettifiedAndroidManifest = Prism.highlight(
      vkbeautify.xml(this.androidManifest),
      Prism.languages.xml,
      "xml")
  }
}