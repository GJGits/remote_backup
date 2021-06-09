import { Injectable } from '@angular/core';
import { from, Observable, of } from 'rxjs';
import { IpcRenderService } from './ipc-render.service';

@Injectable({
  providedIn: 'root'
})
export class MockIpcService implements IpcRenderService {

  constructor() { }
  
  on(channel: string): Observable<any> | undefined {
    throw new Error('Method not implemented.');
  }
  
  send(channel: string, data: any): void {
    throw new Error('Method not implemented.');
  }
  
  invoke(channel: string, data?: any): Observable<any> | undefined {
    if (channel == 'signup') {
      //return of({user: 'Pinco Pallo', device: 1});
      return of({error: 'Errore dal server!'});
    }
    if (channel == 'signin') {
      //return of({user: 'Pinco Pallo', device: 1});
      return of({error: 'Errore dal server!'});
    }
    return of({});
  }
}
