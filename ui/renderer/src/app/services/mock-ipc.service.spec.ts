import { TestBed } from '@angular/core/testing';

import { MockIpcService } from './mock-ipc.service';

describe('MockIpcService', () => {
  let service: MockIpcService;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(MockIpcService);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
