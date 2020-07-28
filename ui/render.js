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
    });

    $("#accedi").click((event) => {
        event.preventDefault();
        if (!logged) {
            $("#login").show();
            $("#signup").hide();
            $("#logged").hide();
        }
    });

});



