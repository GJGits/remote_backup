import { TestBed } from '@angular/core/testing';

import { ProdIpcService } from './prod-ipc.service';

describe('ProdIpcService', () => {
  let service: ProdIpcService;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(ProdIpcService);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
