package com.remotebackup.backend;

import static org.springframework.restdocs.mockmvc.RestDocumentationRequestBuilders.*;
import static org.springframework.restdocs.mockmvc.RestDocumentationResultHandler.*;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.*;
import static org.springframework.restdocs.payload.PayloadDocumentation.*;
import static org.springframework.restdocs.mockmvc.MockMvcRestDocumentation.*;
import static org.springframework.restdocs.mockmvc.MockMvcRestDocumentation.*;
import static org.springframework.restdocs.operation.preprocess.Preprocessors.*;
import static org.springframework.test.web.servlet.result.MockMvcResultHandlers.*;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.autoconfigure.web.servlet.WebMvcTest;
import org.junit.Before;
import org.junit.Rule;
import org.junit.jupiter.api.Test;
import org.springframework.restdocs.JUnitRestDocumentation;
import org.springframework.test.web.servlet.MockMvc;
import org.springframework.test.web.servlet.setup.MockMvcBuilders;
import org.springframework.web.context.WebApplicationContext;

@WebMvcTest
public class AuthTests {

    @Rule
    public final JUnitRestDocumentation restDocumentation = new JUnitRestDocumentation();

    // @Autowired
    // private ObjectMapper objectMapper;

    @Autowired
    private WebApplicationContext context;

    @Autowired
    private MockMvc mockMvc;


    @Before
    public void setUp() {
        this.mockMvc = MockMvcBuilders.webAppContextSetup(this.context)
                .apply(documentationConfiguration(this.restDocumentation)).alwaysDo(document("{method-name}", 
                preprocessRequest(prettyPrint()), preprocessResponse(prettyPrint())))
              .build();
    }

    @Test
    public void testSignup() throws Exception {
        this.mockMvc.perform(post("/auth/signup")).andExpect(status().isOk());
    }

    @Test
    public void testSignin() throws Exception {
        this.mockMvc.perform(post("/auth/signin")).andExpect(status().isOk());
    }

}
