import { ComponentFixture, TestBed } from '@angular/core/testing';

import { SyncingStatusComponent } from './syncing-status.component';

describe('SyncingStatusComponent', () => {
  let component: SyncingStatusComponent;
  let fixture: ComponentFixture<SyncingStatusComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ SyncingStatusComponent ]
    })
    .compileComponents();
  });

  beforeEach(() => {
    fixture = TestBed.createComponent(SyncingStatusComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
