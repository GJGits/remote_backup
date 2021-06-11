import { ComponentFixture, TestBed } from '@angular/core/testing';

import { ConnectionLostComponent } from './connection-lost.component';

describe('ConnectionLostComponent', () => {
  let component: ConnectionLostComponent;
  let fixture: ComponentFixture<ConnectionLostComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ ConnectionLostComponent ]
    })
    .compileComponents();
  });

  beforeEach(() => {
    fixture = TestBed.createComponent(ConnectionLostComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
