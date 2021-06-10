import { Component, OnInit } from '@angular/core';
import { Observable, of } from 'rxjs';
import { AuthService } from 'src/app/auth/auth.service';
import { UserInfo } from 'src/app/auth/dtos';

@Component({
  selector: 'app-main',
  templateUrl: './main.component.html',
  styleUrls: ['./main.component.css']
})
export class MainComponent implements OnInit {

  userInfo$: Observable<UserInfo> = of<UserInfo>({user: '', device: ''});

  constructor(private authService: AuthService) { 
    this.userInfo$ = this.authService.getUserInfo();
  }

  ngOnInit(): void {
  }

}
