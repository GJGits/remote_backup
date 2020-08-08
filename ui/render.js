const { ipcRenderer } = require('electron');
const textEncoding = require('text-encoding');
var logged = false;

$(document).ready(function () {

    console.log("page loaded");

    ipcRenderer.on('asynchronous-message', (event, arg) => {
        console.log("render receiced:", arg);
        //event.reply('asynchronous-reply', 'pong')
    });

    ipcRenderer.on('synchronous-message', (event, arg) => {
        console.log("render receiced:", arg);
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
                //ipcRenderer.sendSync('token', result.token);
                console.log("result:", result);
            },
            statusCode: {
                500: function (message) {
                    $(".error").html(message);
                    console.log(message);
                }
            }
        });
    });

});



