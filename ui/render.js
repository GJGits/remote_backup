const { ipcRenderer } = require('electron');
const textEncoding = require('text-encoding');
var logged = false;

$(document).ready(function () {

    ipcRenderer.on('asynchronous-message', (event, arg) => {
        console.log("render receiced:", arg); // prints "ping"
        //event.reply('asynchronous-reply', 'pong')
    });

    ipcRenderer.on('synchronous-message', (event, arg) => {
        console.log("render receiced:", arg) // prints "ping"
        //event.returnValue = 'pong'
    });

    ipcRenderer.on('status-changed', (event, arg) => {
        console.log(JSON.stringify(arg));
        arg = arg.replace("\n", "");
        console.log(JSON.stringify(arg));
        if (arg === "log-in") {
            logged = true;
            $("#login").hide();
            $("#signup").hide();
            $("#logged").show();
        } else {
            logged = false;
            $("#login").show();
            $("#signup").hide();
            $("#logged").hide();
        }
    });

    $("#registrati").click((event) => {
        event.preventDefault();
        if (!logged) {
            $("#login").hide();
            $("#signup").show();
            $("#logged").hide();
        }
        username = $("#username").val();
        password = $("#password").val();
        confirm_password = $("#rip-password").val();
        $.post("0.0.0.0:3200/signup", { username: username, password: password, confirm_password: confirm_password })
            .done((data) => {
                ipcRenderer.sendSync('token', data.token);
                console.log(data);
            })
            .fail((error) => {
                // todo: show error message
                console.log(error);
            });
    });

    $("#accedi").click((event) => {
        event.preventDefault();
        if (!logged) {
            $("#login").show();
            $("#signup").hide();
            $("#logged").hide();
        }
        username = $("#username").val();
        password = $("#password").val();
        $.post("0.0.0.0:3200/sigin", { username: username, password: password})
            .done((data) => {
                // todo: store jwt
                console.log(data);
            })
            .fail((error) => {
                // todo: show error message
                console.log(error);
            });
    });

});



