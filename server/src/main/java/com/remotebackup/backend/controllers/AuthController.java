package com.remotebackup.backend.controllers;

import javax.validation.Valid;

import com.remotebackup.backend.dtos.JWToken;
import com.remotebackup.backend.dtos.SigninDTO;
import com.remotebackup.backend.dtos.SignupDTO;
import com.remotebackup.backend.services.AuthService;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class AuthController {

    @Autowired
    AuthService authService;

    @PostMapping("/auth/signup")
    public ResponseEntity<JWToken> signup(@Valid @RequestBody SignupDTO signupDTO) {
        JWToken token = authService.signup(signupDTO);
        return ResponseEntity.ok(token);
    }

    @PostMapping("/auth/signin")
    public ResponseEntity<JWToken> signin(@Valid @RequestBody SigninDTO signinDTO) {
        JWToken token = authService.signin(signinDTO);
        return ResponseEntity.ok(token);
    }

}
