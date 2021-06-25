package com.remotebackup.backend;

import static org.springframework.restdocs.mockmvc.RestDocumentationRequestBuilders.*;
import static org.springframework.restdocs.mockmvc.RestDocumentationResultHandler.*;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.*;

import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.remotebackup.backend.controllers.AuthController;
import com.remotebackup.backend.dtos.SigninDTO;
import com.remotebackup.backend.dtos.SignupDTO;

import static org.springframework.restdocs.payload.PayloadDocumentation.*;
import static org.springframework.restdocs.mockmvc.MockMvcRestDocumentation.*;
import static org.springframework.restdocs.mockmvc.MockMvcRestDocumentation.*;
import static org.springframework.restdocs.operation.preprocess.Preprocessors.*;
import static org.springframework.test.web.servlet.result.MockMvcResultHandlers.*;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.autoconfigure.restdocs.AutoConfigureRestDocs;
import org.springframework.boot.test.autoconfigure.web.servlet.WebMvcTest;
import org.springframework.boot.test.mock.mockito.MockBean;
import org.junit.Before;
import org.junit.Rule;
import org.junit.jupiter.api.Test;
import org.springframework.restdocs.JUnitRestDocumentation;
import org.springframework.test.web.servlet.MockMvc;
import org.springframework.test.web.servlet.ResultMatcher;
import org.springframework.test.web.servlet.setup.MockMvcBuilders;
import org.springframework.web.context.WebApplicationContext;

@WebMvcTest(AuthController.class)
@AutoConfigureRestDocs
public class AuthTests {

    @Autowired
    private MockMvc mockMvc;

    @Autowired
    private ObjectMapper objectMapper;

    @MockBean
    private SignupDTO mockSignupDTO;

    @MockBean
    private SigninDTO mockSigninDTO;

    private final String SIGNUP_URL = "/auth/signup";
    private final String SIGNIN_URL = "/auth/signin";

    /**
     * Check that the controller listen for signup requests
     * 
     * @throws Exception
     */
    @Test
    public void checkListenSignupRequest() throws Exception {
        this.mockMvc.perform(post(SIGNUP_URL).contentType("application/json")).andExpect(status().isBadRequest());
    }

    /**
     * Check that the controller listen for sigin requests
     * 
     * @throws Exception
     */
    @Test
    public void checkListenSigninRequest() throws Exception {
        this.mockMvc.perform(post(SIGNIN_URL).contentType("application/json")).andExpect(status().isBadRequest());
    }

    /**
     * Check that signup input is successfully serialized into a Java Object
     * @throws Exception
     */

    @Test
    public void checkSerializationSignupRequest() throws Exception {
        SignupDTO user = new SignupDTO("pinco", "123456", "123456", "aa:bb:cc:dd:ee:aa");
        this.mockMvc.perform(post(SIGNUP_URL).contentType("application/json").content(objectMapper.writeValueAsString(user))).andExpect(status().isOk());
    }

    /**
     * Check that signin input is successfully serialized into a Java Object
     * @throws Exception
     */

    @Test
    public void checkSerializationSigninRequest() throws Exception {
        SigninDTO user = new SigninDTO("pinco", "123456", "aa:bb:cc:dd:ee:aa");
        this.mockMvc.perform(post(SIGNIN_URL).contentType("application/json").content(objectMapper.writeValueAsString(user))).andExpect(status().isOk());
    }

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
        // good request
        dtos.put(new SignupDTO("pinco", "123456", "123456", "aa:bb:cc:dd:ee:aa"), status().isOk());

        for (Entry<SignupDTO, ResultMatcher> testCase : dtos.entrySet()) {
            this.mockMvc.perform(post(SIGNUP_URL).contentType("application/json").content(objectMapper.writeValueAsString(testCase.getKey()))).andExpect(testCase.getValue());
        }
    }


}
