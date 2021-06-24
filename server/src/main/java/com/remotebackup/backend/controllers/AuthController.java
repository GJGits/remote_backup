package com.remotebackup.backend.controllers;

import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class AuthController {
    
    @PostMapping("/auth/signup")
    public void signup() {
    }

    @PostMapping("/auth/signin")
    public void signin() {
    }

}
