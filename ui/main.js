// INCLUDES
const { menubar } = require('menubar');
const dgram = require('dgram');
const server = dgram.createSocket('udp4');
const client = dgram.createSocket('udp4');
const { ipcMain } = require('electron');
const ClientConf = require('./modules/client-conf.js');

// CONSTANTS
const mb = menubar(
  {
    icon: "remote-icon.png",
    tooltip: "show",
    browserWindow: {
      webPreferences: { nodeIntegration: true }
    }
  });

// tenere i codici in questa mappa in sync con la
// mappa dei topics in utente.
var topics = new Map();
topics.set("LOGGED_IN", 16);
topics.set("LOGGED_OUT", 17);



mb.on('ready', () => {

  console.log('app is ready');

  // MESSAGE HANDLERS

  ipcMain.on('config', (event, data) => {
    let conf = new ClientConf();
    conf.set(data);
    client.send(Buffer.from([topics.get("LOGGED_IN")]), 2800, "172.18.0.8", (err) => { console.log(err) });
  });

  ipcMain.on('reset-config', (event, data) => {
    let conf = new ClientConf();
    conf.reset();
    client.send(Buffer.from([topics.get("LOGGED_OUT")]), 2800, "172.18.0.8", (err) => { console.log(err) });
  });

  // UDP INSTANCE DEFINITION
  server.on('error', (err) => {
    console.log(`server error:\n${err.stack}`);
    server.close();
  });

  server.on('message', (msg, rinfo) => {
    console.log(`server got: ${msg} from ${rinfo.address}:${rinfo.port}`);
    msg = new TextDecoder("utf-8").decode(msg);
    if (msg === "login" || msg === "logged")
      mb.window.webContents.send('status-changed', msg);
    if (msg === "start-sync" || msg == "end-sync")
      mb.window.webContents.send('sync', msg);
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
    client.send(Buffer.from([topics.get("LOGGED_IN")]), 2800, "172.18.0.8", (err) => { console.log(err) });
  }

  else {
    mb.window.webContents.send('status-changed', "login");
    client.send(Buffer.from([topics.get("LOGGED_OUT")]), 2800, "172.18.0.8", (err) => { console.log(err) });
  }

  mb.window.webContents.send('sync', "synced");
  mb.window.openDevTools();
});