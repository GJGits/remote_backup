const { ipcRenderer } = require('electron');

$(document).ready(function () {

    $("#test").html("<p>content added by JQuery!</p>");

    ipcRenderer.on('asynchronous-message', (event, arg) => {
        console.log("render receiced:", arg); // prints "ping"
        //event.reply('asynchronous-reply', 'pong')
        $("#test").html("Ricevuto messaggio: " + arg);
    });

    ipcRenderer.on('synchronous-message', (event, arg) => {
        console.log("render receiced:", arg) // prints "ping"
        //event.returnValue = 'pong'
    });

});



