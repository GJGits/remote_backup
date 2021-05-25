import { ComponentFixture, TestBed } from '@angular/core/testing';

import { FooterInfosComponent } from './footer-infos.component';

describe('FooterInfosComponent', () => {
  let component: FooterInfosComponent;
  let fixture: ComponentFixture<FooterInfosComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ FooterInfosComponent ]
    })
    .compileComponents();
  });

  beforeEach(() => {
    fixture = TestBed.createComponent(FooterInfosComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
