package com.remotebackup.backend.services;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.autoconfigure.web.servlet.WebMvcTest;
import org.springframework.boot.test.mock.mockito.MockBean;
import org.springframework.security.authentication.BadCredentialsException;

import static org.junit.Assert.assertThrows;
import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.remotebackup.backend.dtos.JWToken;
import com.remotebackup.backend.dtos.SigninDTO;
import com.remotebackup.backend.dtos.SignupDTO;
import com.remotebackup.backend.entities.UserEntity;
import com.remotebackup.backend.exceptions.UserAlreadyExsistException;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.mockito.Mockito;

@WebMvcTest(AuthService.class)
public class AuthServiceTests {

    @Autowired
    private AuthService authService;

    @MockBean
    private UserService userService;

    @MockBean
    private JwtTokenService jwtTokenService;

    @BeforeEach
    public void setUp() {
        List<String> userRoles = new ArrayList<>();
        userRoles.add("USER");
        Mockito.when(userService.loadUserByUsername("tizio")).thenReturn(new UserEntity("tizio", "$2a$10$v/nDeRWFmmhFBwhUBIbmYuwbxhegAuiRWmsW/vq0kioYoCoy3IfgS"));
        Mockito.when(userService.loadUserByUsername("caio")).thenReturn(null);
        Mockito.when(userService.getUserRoles()).thenReturn(userRoles);
        Mockito.when(jwtTokenService.createToken("caio", userRoles)).thenReturn("arg0");
        Mockito.when(jwtTokenService.createToken("tizio", userRoles)).thenReturn("arg1");
    }

    @Test
    public void signupCheck() throws Exception {
        assertThrows(UserAlreadyExsistException.class, () -> {
            this.authService.signup(new SignupDTO("tizio", "123456", "123456", "aa:bb:cc:dd:ee:aa"));
        });
        JWToken realToken = new JWToken("arg0");
        JWToken obtainedToken = this.authService.signup(new SignupDTO("caio", "123456", "123456", "aa:bb:cc:dd:ee:aa"));
        assertEquals(realToken.getToken(), obtainedToken.getToken());
    }

    @Test
    public void signinCheck() throws Exception {
        assertThrows(BadCredentialsException.class, () -> {
            this.authService.signin(new SigninDTO("tizio", "1234567", "aa:bb:cc:dd:ee:aa"));
        });
        JWToken realToken = new JWToken("arg1");
        JWToken obtainedToken = this.authService.signin(new SigninDTO("tizio", "123456", "aa:bb:cc:dd:ee:aa"));
        assertEquals(realToken.getToken(), obtainedToken.getToken());
    }

}
