import { ComponentFixture, TestBed } from '@angular/core/testing';

import { PermissionsViewComponent } from './permissions-view.component';

describe('PermissionsViewComponentComponent', () => {
  let component: PermissionsViewComponent;
  let fixture: ComponentFixture<PermissionsViewComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [PermissionsViewComponent]
    }).compileComponents();
  });

  beforeEach(() => {
    fixture = TestBed.createComponent(PermissionsViewComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
