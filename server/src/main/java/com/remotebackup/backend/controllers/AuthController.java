package com.remotebackup.backend.controllers;

import javax.validation.Valid;

import com.remotebackup.backend.dtos.SigninDTO;
import com.remotebackup.backend.dtos.SignupDTO;

import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class AuthController {

    // TODO: on signup return token
    @PostMapping("/auth/signup")
    public ResponseEntity<String> signup(@Valid @RequestBody SignupDTO signupDTO) {
        return ResponseEntity.ok("signup");
    }

    // TODO: on sigin return token
    @PostMapping("/auth/signin")
    public ResponseEntity<String> signin(@Valid @RequestBody SigninDTO signinDTO) {
        return ResponseEntity.ok("signin");
    }

}
