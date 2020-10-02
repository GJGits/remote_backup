const check_creds = (username, password, ripPassword) => {

    const user_rgx = /^\w+$/
    const pass_rgx = /^\w+$/

    if (!username) {
        $(".alert.alert-danger.error").show();
        $(".alert.alert-danger.error").text("inserire username");
        return false;
    }

    if (!user_rgx.test(username)) {
        $(".alert.alert-danger.error").show();
        $(".alert.alert-danger.error").text("username non valido. Caretteri consentiti: a-zA-Z0-9_");
        return false;
    }

    if (!password) {
        $(".alert.alert-danger.error").show();
        $(".alert.alert-danger.error").text("inserire password");
        return false;
    }

    if (!pass_rgx.test(password)) {
        $(".alert.alert-danger.error").show();
        $(".alert.alert-danger.error").text("password non valida. Caretteri consentiti: a-zA-Z0-9_");
        return false;
    }

    if (!ripPassword) {
        $(".alert.alert-danger.error").show();
        $(".alert.alert-danger.error").text("ripetere password");
        return false;
    }

    if (password !== ripPassword) {
        $(".alert.alert-danger.error").show();
        $(".alert.alert-danger.error").text("le due password non coincidono");
        return false;
    }

    return true;
};

var signin = (username, password) => {

};

var signup = (username, password, ripPassword) => {
    
}