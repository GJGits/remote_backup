const { ipcRenderer } = require('electron');
const textEncoding = require('text-encoding');
var logged = false;
var transfers = [];

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

var change_status = (status) => {
    if ($("#login")) {
        if (status) {
            $("#login").hide();
            $("#signup").hide();
            $("#logged").show();
            $(".alert.alert-danger.error").hide();
        } else {
            $("#login").show();
            $("#signup").hide();
            $("#logged").hide();
            $(".alert.alert-danger.error").hide();
        }
    }
}

/* MESSAGES HANDLERS */

ipcRenderer.on('asynchronous-message', (event, arg) => {
    console.log("render receiced:", arg);
    //event.reply('asynchronous-reply', 'pong')
});

ipcRenderer.on('synchronous-message', (event, arg) => {
    console.log("render receiced:", arg);
    //event.returnValue = 'pong'
});

ipcRenderer.on('status-changed', (event, arg) => {
    arg = arg.replace("\n", "");
    console.log(JSON.stringify(arg));
    logged = arg === "log-in";
    change_status(logged);
});

ipcRenderer.on('transfer', (event, arg) => {
    const tokens = arg.split(";");
    const transfer = {
        direction: tokens[0],
        file_name: tokens[1],
        done: tokens[2],
        total: tokens[3],
    };
    let dir_img = "";
    if (direction === "up") {
        dir_img = "imgs/icons8-upload-48.png";
    } else if (direction === "down") {
        dir_img = "imgs/icons8-download-48.png";
    } else {
        dir_img = "imgs/icons8-cancel-48.png";
    }
    const dom_element = $("#" + transfer.file_name);
    if (!dom_element.length) {
        // nuovo elemento
        $("#transfers").append("");
    } else {
        if (done === total) {
            // caricamento completato
        }
        if (direction === "abort") {
            // caricamento cancellato
        }
    }
});


$(document).ready(function () {

    change_status(logged);

    /* EVENTS HANDLERS */

    $("#registrati").click((event) => {
        event.preventDefault();
        $("#login").hide();
        $("#signup").show();
        $("#logged").hide();
        $(".alert.alert-danger.error").hide();
    });

    $("#accedi").click((event) => {
        event.preventDefault();
        $("#login").show();
        $("#signup").hide();
        $("#logged").hide();
        $(".alert.alert-danger.error").hide();
    });

    $("#signin-btn").click((event) => {
        event.preventDefault();
        console.log("login clicked");
        username = $("#username").val();
        password = $("#password").val();
        $.ajax({
            url: "http://0.0.0.0:3200/auth/signin",
            type: "POST",
            data: JSON.stringify({ username: username, password: password.toString() }),
            contentType: "application/json",
            dataType: "json",
            success: function (result) {
                if (result && result.token) {
                    ipcRenderer.sendSync('token', {user:username, token:result.token});
                }

            },
            statusCode: {
                500: function (message) {
                    $(".alert.alert-danger.error").show();
                    $(".alert.alert-danger.error").text(message.responseJSON.error);
                }
            }
        });
    });

    $("#signup-btn").click((event) => {
        event.preventDefault();
        console.log("login clicked");
        username = $("#username2").val();
        password = $("#password2").val();
        ripPassword = $("#rip-password").val();
        if (check_creds(username, password.toString(), ripPassword.toString())) {
            $.ajax({
                url: "http://0.0.0.0:3200/auth/signup",
                type: "POST",
                data: JSON.stringify({ username: username, password: password.toString(), password_confirm: ripPassword.toString() }),
                contentType: "application/json",
                dataType: "json",
                success: function (result) {
                    ipcRenderer.sendSync('token', result);
                },
                statusCode: {
                    500: function (message) {
                        $(".alert.alert-danger.error").show();
                        $(".alert.alert-danger.error").text(message.responseJSON.error);
                    }
                }
            });
        }

    });

});



