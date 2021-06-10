export interface SignupDTO {
    username: string;
    password: string;
    password_confirm: string;
}

export interface SigninDTO {
    username: string;
    password: string;
}

export interface UserInfo {
    user: string;
    device: string;
}