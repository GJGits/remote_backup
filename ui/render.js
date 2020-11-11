const { ipcRenderer } = require('electron');
const du = require('du');
const getMAC = require('getmac').default

const CONFIG_SYNC = "../client/sync/"
const mac = getMAC();
//const mac = "aa:bb:cc:dd:ee:ff"

var change_status = (status) => {
    let stats = ["login", "signup", "logged"];
    let index = stats.findIndex((el) => { return el === status; });
    index = index === -1 ? 0 : index;
    for (let i = 0; i < stats.length; i++) {
        if (i === index)
            $("#" + stats[i]).show();
        else
            $("#" + stats[i]).hide();
    }
    $(".alert.alert-danger.error").hide();
}

var update_usage = () => {
    let size_prom = du(CONFIG_SYNC);
    size_prom.then((size) => {
        let percentage = ((size / (2 * 1024 * 1024 * 1024)) * 100) > 100 ? 100 : ((size / (2 * 1024 * 1024 * 1024)) * 100).toFixed(2);
        $("#usage").text("Total space usage: " + percentage + "%");
        $("#us_prog").removeClass();
        $("#us_prog").addClass("progress-bar");
        $("#us_prog").css("width", "" + parseInt(percentage) + "%");
        $("#us_prog").attr("aria-valuenow", parseInt(percentage));
    });
}

/* MESSAGES HANDLERS */

ipcRenderer.on('transfer', (event, arg) => {
    if (arg.status === 0) {
        $("#loading").hide();
        $("#synced").show();
        $("#noconn").hide();
        update_usage();
    } else {
        $("#loading").show();
        $("#synced").hide();
        $("#noconn").hide();
    }
});

ipcRenderer.on('background-message', (event, arg) => {
    console.log("render receiced:", arg);
    $("#loading").hide();
    $("#synced").hide();
    $("#noconn").show();
});

ipcRenderer.on('status-changed', (event, arg) => {
    arg = arg.replace("\n", "");
    console.log("status changed:", arg);
    change_status(arg);
});

ipcRenderer.on('sync', (event, arg) => {
    if (arg === 'start-sync') {
        $("#loading").show();
        $("#synced").hide();
        $("#noconn").hide();
    } else {
        $("#loading").hide();
        $("#synced").show();
        $("#noconn").hide();
        update_usage();
    }
});

$(document).ready(function () {

    /* EVENTS HANDLERS */

    $("#registrati").click((event) => {
        event.preventDefault();
        change_status("signup");
    });

    $("#accedi").click((event) => {
        event.preventDefault();
        change_status("login");
    });

    $("#signin-btn").click((event) => {
        event.preventDefault();
        console.log("login clicked");
        username = $("#username").val();
        password = $("#password").val();
        console.log("mac:", mac);
        $.ajax({
            url: "http://0.0.0.0:3200/auth/signin",
            type: "POST",
            data: JSON.stringify({ username: username, password: password.toString(), mac_address: mac }),
            contentType: "application/json",
            dataType: "json",
        }).done(function (data) {
            console.log("dati successo", data);
            ipcRenderer.send('config', { username: username, token: data.token });
            change_status("logged");
        }).fail(function (error) {
            $(".alert.alert-danger.error").show();
            if (error.status === 404) {
                $(".alert.alert-danger.error").text("server non raggiungibile");
            } else {
                $(".alert.alert-danger.error").text(error.responseJSON.error);
            }

        });
    });

    $("#signup-btn").click((event) => {
        console.log("signup clicked");
        $("#rip-password").removeAttr("title");
        event.preventDefault();
        username = $("#username2").val();
        password = $("#password2").val();
        ripPassword = $("#rip-password").val();
        const supf = $("#supf");
        supf.validate();
        if (password !== ripPassword) {
            $("#rip-password")[0].setCustomValidity('Le due password non coincidono');
            $("#rip-password").attr('title', 'Le due password non coincidono');
            $("#rip-password")[0].reportValidity();
        } else {
            if (supf.valid()) {
                $.ajax({
                    url: "http://0.0.0.0:3200/auth/signup",
                    type: "POST",
                    data: JSON.stringify({ username: username, password: password.toString(), password_confirm: ripPassword.toString(), mac_address: mac }),
                    contentType: "application/json",
                    dataType: "json"
                }).done(function (data) {
                    console.log("dati successo", data);
                    ipcRenderer.send('config', { username: username, token: data.token });
                    change_status("logged");
                }).fail(function (error) {
                    $(".alert.alert-danger.error").text(error);
                    $(".alert.alert-danger.error").show();
                    if (error.status === 404) {
                        $(".alert.alert-danger.error").text("server non raggiungibile");
                    } else {
                        $(".alert.alert-danger.error").text(error.responseJSON.error);
                    }
                });
            }
        }





    });

    $("#loggoff").click((event) => {
        ipcRenderer.send('reset-config', {});
        change_status("login");
    });

});



