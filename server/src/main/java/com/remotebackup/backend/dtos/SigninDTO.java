package com.remotebackup.backend.dtos;

import javax.validation.constraints.NotNull;
import javax.validation.constraints.Pattern;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.EqualsAndHashCode;

@EqualsAndHashCode
@Data
@AllArgsConstructor
public class SigninDTO {

    @NotNull(message = "Username cannot be empty")
    private String username;

    @NotNull(message = "Password cannot be empty")
    @Pattern(regexp = "^\\w+$", flags = Pattern.Flag.UNICODE_CASE)
    private String password;

    @NotNull(message = "Mac address cannot be empty")
    @Pattern(regexp = "^[0-9a-f:]{17}$", flags = Pattern.Flag.UNICODE_CASE)
    private String mac_address;

}
