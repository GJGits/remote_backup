import { Component, OnInit } from '@angular/core';
import { NgForm } from '@angular/forms';
import { IpcRenderService } from 'src/app/services/ipc-render.service';
import { SigninDTO } from '../dtos';

@Component({
  selector: 'app-signin',
  templateUrl: './signin.component.html',
  styleUrls: ['./signin.component.css']
})
export class SigninComponent implements OnInit {

  user: SigninDTO = {username: '', password: ''};
  serverError = '';

  constructor(private ipcService: IpcRenderService) { }

  ngOnInit(): void {
  }

  onSubmit(f: NgForm) {
    if (f.form.valid) {
    this.ipcService.invoke("signin", this.user)?.subscribe((data) => {
      // On success: {user, device}
        // On error  : {error}
        if (data.error) {
          this.serverError = data.error
          console.log(data.error);
        }
        // todo: publish new received info
        // todo: redirect to app/main
    });
    }
    
  }

}
