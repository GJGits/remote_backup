package com.remotebackup.backend;

import static org.springframework.restdocs.mockmvc.RestDocumentationRequestBuilders.*;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.*;
import static org.springframework.restdocs.payload.PayloadDocumentation.*;
import static org.springframework.restdocs.mockmvc.MockMvcRestDocumentation.document;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.autoconfigure.web.servlet.WebMvcTest;
import org.junit.Rule;
import org.junit.jupiter.api.Test;
import org.springframework.restdocs.JUnitRestDocumentation;
import org.springframework.test.web.servlet.MockMvc;
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

    // @Before
    // public void setUp() {
    //     // this.mockMvc = MockMvcBuilders.webAppContextSetup(this.context)
    //     //         .apply(documentationConfiguration(this.restDocumentation)).build();
    //     this.mockMvc = MockMvcBuilders.webAppContextSetup(this.context).build();
    //     this.mockMvc.
    // }

    @Test
    public void testSignup() throws Exception {
        this.mockMvc.perform(post("/auth/signup")).andExpect(status().isOk()).andDo(document("error-example",
        responseFields(
                fieldWithPath("error").description("The HTTP error that occurred, e.g. `Bad Request`"),
                fieldWithPath("message").description("A description of the cause of the error"),
                fieldWithPath("path").description("The path to which the request was made"),
                fieldWithPath("status").description("The HTTP status code, e.g. `400`"),
                fieldWithPath("timestamp").description("The time, in milliseconds, at which the error occurred"))));
    }

    @Test
    public void testSignin() throws Exception {
        this.mockMvc.perform(post("/auth/signin")).andExpect(status().isOk());
    }

}
