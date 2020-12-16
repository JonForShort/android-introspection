import { ComponentFixture, TestBed } from '@angular/core/testing';

import { AndroidManifestViewComponent } from './android-manifest-view.component';

describe('AndroidManifestViewComponent', () => {
  let component: AndroidManifestViewComponent;
  let fixture: ComponentFixture<AndroidManifestViewComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ AndroidManifestViewComponent ]
    })
    .compileComponents();
  });

  beforeEach(() => {
    fixture = TestBed.createComponent(AndroidManifestViewComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
