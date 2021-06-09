import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { SigninDTO, SignupDTO } from './dtos';

@Injectable({
  providedIn: 'root'
})
export class AuthService {

  baseURL = "0.0.0.0:3200";

  constructor(private httpClient: HttpClient) { }

  public isLoggedIn() {
    return false;
  }

  public signup(singupDTO: SignupDTO): Observable<any> {
    return this.httpClient.post(this.baseURL + "/signup", singupDTO);
  }

  public signin(signinDTO: SigninDTO): Observable<any> {
    return this.httpClient.post(this.baseURL + "/signin", signinDTO);
  }

}
