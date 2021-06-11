import { Component, OnInit } from '@angular/core';
import { Router } from '@angular/router';
import { Observable, of } from 'rxjs';
import { AuthService } from 'src/app/auth/auth.service';
import { UserInfo } from 'src/app/auth/dtos';
import { IpcRenderService } from 'src/app/services/ipc-render.service';
import { UsageInfo } from '../models';

@Component({
  selector: 'app-main',
  templateUrl: './main.component.html',
  styleUrls: ['./main.component.css']
})
export class MainComponent implements OnInit {

  userInfo$: Observable<UserInfo> = of<UserInfo>({user: '', device: ''});
  usageInfo$: Observable<UsageInfo> | undefined = of<UsageInfo>({current_byte_used: 0, total_byte_allowed: 0});

  constructor(private authService: AuthService, private ipcService: IpcRenderService, private router: Router) { 
    this.userInfo$ = this.authService.getUserInfo();
    this.usageInfo$ = this.ipcService.on('usage_info');
    this.ipcService.on('connection_lost')?.subscribe(()=>this.onConnectionLost());
  }

  ngOnInit(): void {
  }

  onConnectionLost() {
    this.router.navigate(['/app/connectionlost']);
  }

}
