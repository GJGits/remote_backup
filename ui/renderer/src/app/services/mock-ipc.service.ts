import { Injectable } from '@angular/core';
import { userInfo } from 'os';
import { from, Observable, of } from 'rxjs';
import { IpcRenderService } from './ipc-render.service';

@Injectable({
  providedIn: 'root'
})
export class MockIpcService implements IpcRenderService {

  constructor() { }
  
  on(channel: string): Observable<any> | undefined {
    switch (channel) {
      case 'credentials_expired': {return;}
      case 'user_info': {return of({user: '', device: ''});}
      default: return of();
    }
  }
  
  send(channel: string, data?: any): void {
    throw new Error('Method not implemented.');
  }
  
  invoke(channel: string, data?: any): Observable<any> | undefined {
    if (channel == 'signup') {
      return of({user: 'Pinco Pallo', device: 1});
      //return of({error: 'Errore dal server!'});
    }
    if (channel == 'signin') {
      return of({user: 'Pinco Pallo', device: 1});
      //return of({error: 'Errore dal server!'});
    }
    return of({});
  }
}
