package com.remotebackup.backend;

import static org.springframework.restdocs.mockmvc.RestDocumentationRequestBuilders.*;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.*;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.remotebackup.backend.configurations.SecurityConfiguration;
import com.remotebackup.backend.controllers.AuthController;
import com.remotebackup.backend.dtos.ErrorDTO;
import com.remotebackup.backend.dtos.JWToken;
import com.remotebackup.backend.dtos.SigninDTO;
import com.remotebackup.backend.dtos.SignupDTO;
import com.remotebackup.backend.entities.UserEntity;
import com.remotebackup.backend.exceptions.UserAlreadyExsistException;
import com.remotebackup.backend.services.AuthService;
import com.remotebackup.backend.services.UserService;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.autoconfigure.restdocs.AutoConfigureRestDocs;
import org.springframework.boot.test.autoconfigure.web.servlet.WebMvcTest;
import org.springframework.boot.test.mock.mockito.MockBean;
import org.springframework.security.authentication.BadCredentialsException;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.mockito.Mockito;
import org.springframework.test.web.servlet.MockMvc;
import org.springframework.test.web.servlet.ResultMatcher;

@WebMvcTest(AuthController.class)
@AutoConfigureRestDocs
public class AuthControllerTests {

        @Autowired
        private MockMvc mockMvc;

        @Autowired
        private ObjectMapper objectMapper;

        @MockBean
        private AuthService authService;

        @MockBean
        private UserService userService;

        private final String SIGNUP_URL = "/auth/signup";
        private final String SIGNIN_URL = "/auth/signin";

        /**
         * Set mock services default behavior. If a different behavior is needed for a
         * particular test it must be overwritten with a Mockit.when(...) call inside
         * the test.
         */
        @BeforeEach
        private void setUp() {
                SignupDTO validDTO = new SignupDTO("tizio", "123456", "123456", "aa:bb:cc:dd:ee:aa");
                List<String> userRoles = new ArrayList<>();
                userRoles.add("USER");
                Mockito.when(authService.signup(validDTO)).thenReturn(new JWToken("arg0"));
                Mockito.when(userService.loadUserByUsername("tizio")).thenReturn(new UserEntity("tizio", "123456"));
                Mockito.when(userService.getUserRoles()).thenReturn(userRoles);
        }

        /**
         * Check that the controller listen for signup requests
         * 
         * @throws Exception
         */
        @Test
        public void checkListenSignupRequest() throws Exception {
                this.mockMvc.perform(post(SIGNUP_URL).contentType("application/json"))
                                .andExpect(status().isBadRequest());
        }

        /**
         * Check that the controller listen for sigin requests
         * 
         * @throws Exception
         */
        @Test
        public void checkListenSigninRequest() throws Exception {
                this.mockMvc.perform(post(SIGNIN_URL).contentType("application/json"))
                                .andExpect(status().isBadRequest());
        }

        /**
         * Check that signup input is successfully serialized into a Java Object
         * 
         * @throws Exception
         */

        @Test
        public void checkSerializationSignupRequest() throws Exception {
                SignupDTO user = new SignupDTO("pinco", "123456", "123456", "aa:bb:cc:dd:ee:aa");
                this.mockMvc.perform(post(SIGNUP_URL).contentType("application/json")
                                .content(objectMapper.writeValueAsString(user))).andExpect(status().isOk());
        }

        /**
         * Check that signin input is successfully serialized into a Java Object
         * 
         * @throws Exception
         */

        @Test
        public void checkSerializationSigninRequest() throws Exception {
                SigninDTO user = new SigninDTO("pinco", "123456", "aa:bb:cc:dd:ee:aa");
                this.mockMvc.perform(post(SIGNIN_URL).contentType("application/json")
                                .content(objectMapper.writeValueAsString(user))).andExpect(status().isOk());
        }

        /**
         * Check constraints on SignupDTO object
         * 
         * @throws Exception
         */

        @Test
        public void checkValiditySignupRequest() throws Exception {
                Map<SignupDTO, ResultMatcher> dtos = new HashMap<>();
                // test null fields
                dtos.put(new SignupDTO(null, "123456", "123456", "aa:bb:cc:dd:ee:aa"), status().isBadRequest());
                dtos.put(new SignupDTO("pinco", null, "123456", "aa:bb:cc:dd:ee:aa"), status().isBadRequest());
                dtos.put(new SignupDTO("pinco", "123456", null, "aa:bb:cc:dd:ee:aa"), status().isBadRequest());
                dtos.put(new SignupDTO("pinco", "123456", "123456", null), status().isBadRequest());
                // test empty fields
                dtos.put(new SignupDTO("", "123456", "123456", "aa:bb:cc:dd:ee:aa"), status().isBadRequest());
                dtos.put(new SignupDTO("pinco", "", "123456", "aa:bb:cc:dd:ee:aa"), status().isBadRequest());
                dtos.put(new SignupDTO("pinco", "123456", "", "aa:bb:cc:dd:ee:aa"), status().isBadRequest());
                dtos.put(new SignupDTO("pinco", "123456", "123456", ""), status().isBadRequest());
                // test wrong formats
                dtos.put(new SignupDTO("1", "123456", "123456", "aa:bb:cc:dd:ee:aa"), status().isBadRequest());
                dtos.put(new SignupDTO("pinco", "!!", "123456", "aa:bb:cc:dd:ee:aa"), status().isBadRequest());
                dtos.put(new SignupDTO("pinco", "123456", "12", "aa:bb:cc:dd:ee:aa"), status().isBadRequest());
                dtos.put(new SignupDTO("pinco", "123456", "123456", "a"), status().isBadRequest());

                for (Entry<SignupDTO, ResultMatcher> testCase : dtos.entrySet()) {
                        this.mockMvc.perform(post(SIGNUP_URL).contentType("application/json")
                                        .content(objectMapper.writeValueAsString(testCase.getKey())))
                                        .andExpect(testCase.getValue());
                }
        }

        @Test
        public void signupCheck() throws Exception {
                SignupDTO validDTO = new SignupDTO("tizio", "123456", "123456", "aa:bb:cc:dd:ee:aa");
                SignupDTO duplicateDTO = new SignupDTO("tizio", "123456", "123456", "aa:bb:cc:dd:ee:aa");
                this.mockMvc.perform(post(SIGNUP_URL).contentType("application/json")
                                .content(objectMapper.writeValueAsString(validDTO))).andExpect(status().isOk());
                Mockito.when(authService.signup(duplicateDTO)).thenThrow(new UserAlreadyExsistException());
                this.mockMvc.perform(post(SIGNUP_URL).contentType("application/json")
                                .content(objectMapper.writeValueAsString(duplicateDTO)))
                                .andExpect(status().isBadRequest()).andExpect(content().contentType("application/json"))
                                .andExpect(content().json(objectMapper.writeValueAsString(
                                                new ErrorDTO("An account with this username already exsists"))));
        }

        @Test
        public void signinCheck() throws Exception {
                SigninDTO validCredentials = new SigninDTO("tizio", "123456", "aa:bb:cc:dd:ee:aa");
                SigninDTO invalidPassword = new SigninDTO("tizio", "23456", "aa:bb:cc:dd:ee:aa");
                Mockito.when(authService.signin(validCredentials)).thenReturn(new JWToken("arg0"));
                this.mockMvc.perform(post(SIGNIN_URL).contentType("application/json")
                                .content(objectMapper.writeValueAsString(validCredentials))).andExpect(status().isOk());
                Mockito.when(authService.signin(invalidPassword)).thenThrow(new BadCredentialsException("msg"));
                this.mockMvc.perform(post(SIGNIN_URL).contentType("application/json")
                                .content(objectMapper.writeValueAsString(invalidPassword)))
                                .andExpect(status().isBadRequest());
                
        }

}
