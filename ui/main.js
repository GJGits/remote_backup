// INCLUDES
const { menubar } = require('menubar');
const dgram = require('dgram');
const server = dgram.createSocket('udp4');
const { ipcMain } = require('electron');
const Token = require('./modules/token.js');

// CONSTANTS
const mb = menubar(
  {
    icon: "remote-icon.png",
    tooltip: "show",
    browserWindow: {
      webPreferences: { nodeIntegration: true }
    }
  });

var token = new Token();


mb.on('ready', () => {

  console.log('app is ready');

  // MESSAGE HANDLERS

  ipcMain.on('token', (event, data) => {
    if (data && data.token && token.set(data.token)) {
      mb.window.webContents.send('status-changed', "log-in");
      event.returnValue = "ok";
    } else {
      event.returnValue = "ko";
      mb.window.webContents.send('status-changed', "log-off");
    }
  });

  // UDP INSTANCE DEFINITION
  server.on('error', (err) => {
    console.log(`server error:\n${err.stack}`);
    server.close();
  });

  server.on('message', (msg, rinfo) => {
    console.log(`server got: ${msg} from ${rinfo.address}:${rinfo.port}`);
    msg = new TextDecoder("utf-8").decode(msg);
    mb.window.webContents.send('asynchronous-message', msg);
    if (msg === "log-in" || msg === "log-off")
      mb.window.webContents.send('status-changed', msg);
  });

  server.on('listening', () => {
    const address = server.address();
    console.log(`server listening ${address.address}:${address.port}`);
  });

  server.bind(41234);

});

mb.on('after-create-window', () => {
  if (token && token.isValid()) {
    mb.window.webContents.send('status-changed', "log-in");
  } else {
    token.reset();
    mb.window.webContents.send('status-changed', "log-off");
  }
  mb.window.openDevTools();
});