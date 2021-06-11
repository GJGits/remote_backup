import { Injectable } from '@angular/core';
import { BehaviorSubject, Observable } from 'rxjs';
import { IpcRenderService } from '../services/ipc-render.service';
import { UserInfo } from './dtos';
import { map } from 'rxjs/operators';
import { Router } from '@angular/router';

@Injectable({
  providedIn: 'root'
})
export class AuthService {

  private userInfo: BehaviorSubject<UserInfo> = new BehaviorSubject({user: '', device: ''});

  constructor(private ipcService: IpcRenderService, private router: Router) {
    this.ipcService?.on('credentials_expired')?.subscribe(() => this.credentialsExpired());
    this.ipcService?.on('user_info')?.subscribe((userInfo) => this.setCredentials(userInfo));
   }

  private credentialsExpired(): void {
    this.clearCredentials();
    this.router.navigate(['/auth/signin']);
  }
  
  private clearCredentials(): void {
    this.userInfo.next({user: '', device: ''});
  }

  public setCredentials(userInfo: UserInfo): void {
    this.userInfo.next(userInfo);
  }

  public getUserInfo(): Observable<UserInfo> {
    return this.userInfo.asObservable();
  }

  public isLoggedIn(): Observable<boolean> {
    return this.userInfo.asObservable().pipe(map(u => u.user !== ''));
  }

  public logOff(): void {
    this.clearCredentials();
    this.ipcService.send('logoff');
  }

}
