import { ComponentFixture, TestBed } from '@angular/core/testing';
import { ExportApkDialogComponent } from './export-apk-dialog.component';

describe('ExportApkDialogComponent', () => {
  let component: ExportApkDialogComponent;
  let fixture: ComponentFixture<ExportApkDialogComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ExportApkDialogComponent]
    })
      .compileComponents();
  });

  beforeEach(() => {
    fixture = TestBed.createComponent(ExportApkDialogComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
