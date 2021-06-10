import { Component, OnInit } from '@angular/core';
import { NgForm } from '@angular/forms';
import { Router } from '@angular/router';
import { IpcRenderService } from 'src/app/services/ipc-render.service';
import { AuthService } from '../auth.service';
import { SignupDTO } from '../dtos';

@Component({
  selector: 'app-signup',
  templateUrl: './signup.component.html',
  styleUrls: ['./signup.component.css']
})
export class SignupComponent implements OnInit {

  newUser: SignupDTO = { username: '', password: '', password_confirm: '' };
  serverError = '';

  constructor(private ipcService: IpcRenderService, private authService: AuthService, private router: Router) { }

  ngOnInit(): void {
  }

  onSubmit(f: NgForm) {
    if (f.form.valid) {
      this.ipcService.invoke("signup", this.newUser)?.subscribe((data) => {
        // On success: {user, device}
        // On error  : {error}
        if (data.error) {
          this.serverError = data.error
        } else {
          this.authService.setCredentials(data);
          this.router.navigate(['/app']);
        }
      });
    }

  }

}
