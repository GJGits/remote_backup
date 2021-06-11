import { Injectable } from '@angular/core';
import { BehaviorSubject, from, Observable, of, Subject } from 'rxjs';
import { Transfer, UsageInfo } from '../logged/models';
import { IpcRenderService } from './ipc-render.service';

@Injectable({
  providedIn: 'root'
})
export class MockIpcService implements IpcRenderService {

  constructor() { }

  on(channel: string): Observable<any> | undefined {
    switch (channel) {
      case 'credentials_expired': { return; }
      case 'user_info': { return of({ user: '', device: '' }); }
      case 'usage_info': {
        let usageSubject = new BehaviorSubject<UsageInfo>({ current_byte_used: (1.2 * 1000), total_byte_allowed: (2 * 1024 * 1024 * 1024) });
        setTimeout(() => {
          usageSubject.next({ current_byte_used: (16 * 1000 * 1000), total_byte_allowed: (2 * 1024 * 1024 * 1024) });
        }, 5000);
        return usageSubject.asObservable();
      }
      case 'transfer': {
        let transferSubject = new BehaviorSubject<Transfer>({path: 'somePath', last_change: 123456, status: 1, producer: 0, nchunks: 3});
        setTimeout(() => {
          transferSubject.next({path: 'somePath', last_change: 123456, status: 0, producer: 0, nchunks: 3});
        }, 5000);
        return transferSubject.asObservable();
      }
      case 'connection_lost': {
        let connectionSubject = new Subject<any>();
        setTimeout(()=>{connectionSubject.next({})}, 20000);
        return connectionSubject;
      }

      default: return of();
    }
  }
  send(channel: string, data?: any): void {
    throw new Error('Method not implemented.');
  }

  invoke(channel: string, data?: any): Observable<any> | undefined {
    if (channel == 'signup') {
      return of({ user: 'Pinco Pallo', device: 1 });
      //return of({error: 'Errore dal server!'});
    }
    if (channel == 'signin') {
      return of({ user: 'Pinco Pallo', device: 1 });
      //return of({error: 'Errore dal server!'});
    }
    return of({});
  }
}
