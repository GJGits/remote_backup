// INCLUDES
const { menubar } = require('menubar');
const execSync = require('child_process').execSync;
const remote = require('electron').remote
const dgram = require('dgram');
const server = dgram.createSocket('udp4');
const client = dgram.createSocket('udp4');
const { ipcMain, app, Menu, Tray } = require('electron');
const ClientConf = require('./modules/client-conf.js');
const { Notification } = require('electron');
var net = require('net');

var connected = true;

const set_network_limit = () => {
  const id = execSync("docker exec remote_backup_client_1 cat /sys/class/net/eth0/iflink").toString().replace(/(\r\n|\n|\r)/gm, "");
  const grep_res = execSync("ip ad | grep " + id).toString().replace(/(\r\n|\n|\r)/gm, "");
  const iface_rgx = /\d{1,2}\:\s(veth\w+)@/g;
  const dock_cli_iface = iface_rgx.exec(grep_res)[1];
  if (dock_cli_iface) {
    // set upload limit rate
    //execSync("sudo tc qdisc add dev " + dock_cli_iface + " handle 10: root tbf rate 0.5mbit \ burst 5kb latency 70ms peakrate 1mbit \ minburst 1540");
    //execSync("sudo tc qdisc add dev " + dock_cli_iface + " root netem delay 20ms");
    execSync("sudo tc qdisc add dev " + dock_cli_iface + " root tbf rate 30mbit burst 1mbit latency 400ms");
    // set download limit rate
    //execSync("sudo tc qdisc add dev " + dock_cli_iface + " parent 10:1 handle 100: sfq");
  }
  console.log("docker iface: ", dock_cli_iface);
}

// CONSTANTS
app.on('ready', () => {
  set_network_limit();
  var command = "./find_client_ip";
  const client_ip = execSync(command).toString().replace(/(\r\n|\n|\r)/gm, "");
  //const client_ip = "127.0.0.1";
  console.log("client ip: ", client_ip);
  // tenere i codici in questa mappa in sync con la
  // mappa dei topics in utente.
  var topics = new Map();
  topics.set("STOP", 0);
  topics.set("START", 1);
  topics.set("RESTART", 2);
  topics.set("EXIT", 3);

  function on_open(menutItem, browserWindow, event) {
    mb.showWindow();
  }

  function on_close(menutItem, browserWindow, event) {
    console.log("click close");
    client.send(Buffer.from([topics.get("EXIT")]), 2800, client_ip, (err) => { console.log(err) });
    mb.app.quit();
  }

  //const tray = new Tray("remote-icon.png");
  const contextMenu = Menu.buildFromTemplate([
    { label: 'open', type: 'normal', click: on_open },
    { type: 'separator' },
    { label: 'close', type: 'normal', click: on_close },
  ]);
  //tray.setContextMenu(contextMenu);
  const mb = menubar(
    {
      icon: "remote-icon.png",
      tooltip: "show",
      browserWindow: {
        width: 450,
        height: 400,
        webPreferences: { nodeIntegration: true }
      }
    });

  mb.on('ready', () => {

    console.log('app is ready');
    mb.tray.setContextMenu(contextMenu);

    // MESSAGE HANDLERS

    ipcMain.on('config', (event, data) => {
      let conf = new ClientConf();
      console.log("data:", data);
      conf.set(data);
      let info = conf.get_info();
      mb.window.webContents.send('info', info);
      client.send(Buffer.from([topics.get("START")]), 2800, client_ip, (err) => { console.log(err) });
    });

    ipcMain.on('reset-config', (event, data) => {
      let conf = new ClientConf();
      conf.reset();
      client.send(Buffer.from([topics.get("STOP")]), 2800, client_ip, (err) => { console.log(err) });
    });

    // UDP INSTANCE DEFINITION
    server.on('error', (err) => {
      console.log(`server error:\n${err.stack}`);
      server.close();
    });

    server.on('message', (msg, rinfo) => {
      //console.log(`server got: ${msg} from ${rinfo.address}:${rinfo.port}`);
      msg = new TextDecoder("utf-8").decode(msg);
      let obj = JSON.parse(msg);
      if (obj.code === "transfer") {
        mb.window.webContents.send('transfer', obj.message);
      }
      if (obj.code === "connection-lost") {

        if (connected) {
          connected = false;
          mb.window.webContents.send('background-message', msg);
          var refreshId = setInterval(function testPort(port, host, cb) {
            net.createConnection(3200, "0.0.0.0").on("connect", function (e) {
              let conf = new ClientConf();
              if (conf.isValid()) {
                mb.window.webContents.send('status-changed', "logged");
                let info = conf.get_info();
                mb.window.webContents.send('info', info);
                client.send(Buffer.from([topics.get("START")]), 2800, client_ip, (err) => { console.log(err) });
                connected = true;
              }
              else {
                mb.window.webContents.send('status-changed', "login");
                client.send(Buffer.from([topics.get("STOP")]), 2800, client_ip, (err) => { console.log(err) });
              }
              // recuperata la connessione esco a prescindere da essere ancora loggati o meno
              clearInterval(refreshId);
            }).on("error", function (e) {
              console.log("unreach");
            });
          }, 30000);
        }




      }
      if (obj.code === "auth-failed") {
        const notification = {
          title: 'Authentication failed',
          body: 'Please log in.'
        }
        let conf = new ClientConf();
        conf.reset();
        new Notification(notification).show();
        mb.window.webContents.send('status-changed', "login");
      }

    });

    server.on('listening', () => {
      const address = server.address();
      console.log(`server listening ${address.address}:${address.port}`);
    });

    server.bind(41234);

  });

  mb.on('after-create-window', () => {
    let conf = new ClientConf();
    if (conf.isValid()) {
      mb.window.webContents.send('status-changed', "logged");
      let info = conf.get_info();
      mb.window.webContents.send('info', info);
      client.send(Buffer.from([topics.get("START")]), 2800, client_ip, (err) => { console.log(err) });
    }

    else {
      mb.window.webContents.send('status-changed', "login");
      client.send(Buffer.from([topics.get("STOP")]), 2800, client_ip, (err) => { console.log(err) });
    }

    mb.window.webContents.send('sync', "synced");
    //mb.window.openDevTools();
  });


}); // end app