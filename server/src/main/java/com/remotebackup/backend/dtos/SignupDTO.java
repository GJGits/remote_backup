package com.remotebackup.backend.dtos;

import javax.validation.constraints.Min;
import javax.validation.constraints.Pattern;
import javax.validation.constraints.Size;

import com.remotebackup.backend.validators.FieldsValueMatch;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.EqualsAndHashCode;
import javax.validation.constraints.NotNull;

@EqualsAndHashCode
@Data
@AllArgsConstructor
@FieldsValueMatch(field = "password", fieldMatch = "password_confirm", message = "Passwords do not match!")
public class SignupDTO {

    @NotNull(message = "Username cannot be empty")
    @Size(min = 4, max = 10, message = "Username length must be at least 4")
    private String username;

    @NotNull(message = "Password cannot be empty")
    @Pattern(regexp = "^\\w+$", flags = Pattern.Flag.UNICODE_CASE)
    private String password;

    @NotNull(message = "Password confirm cannot be empty")
    private String password_confirm;

    @NotNull(message = "Mac address cannot be empty")
    @Pattern(regexp = "^[0-9a-f:]{17}$", flags = Pattern.Flag.UNICODE_CASE)
    private String mac_address;

}
