import { TestBed } from '@angular/core/testing';

import { IpcRenderService } from './ipc-render.service';

describe('IpcRenderService', () => {
  let service: IpcRenderService;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(IpcRenderService);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
