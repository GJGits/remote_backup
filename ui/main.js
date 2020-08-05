// INCLUDES
const { menubar } = require('menubar');
const dgram = require('dgram');
const server = dgram.createSocket('udp4');
const { ipcMain } = require('electron');
const textEncoding = require('text-encoding');
const storage = require('electron-json-storage');

// CONSTANTS
const mb = menubar(
  {
    icon: "remote-icon.png",
    tooltip: "tooltip-name",
    browserWindow: {
      webPreferences: { nodeIntegration: true }
    }
  });


mb.on('ready', () => {

  console.log('app is ready');

  // MESSAGE HANDLERS
  ipcMain.on('asynchronous-message', (event, arg) => {
    console.log(arg) // prints "ping"
    event.reply('asynchronous-reply', 'pong')
  });

  ipcMain.on('synchronous-message', (event, arg) => {
    console.log(arg) // prints "ping"
    event.returnValue = 'pong'
  });

  ipcMain.on('token', (event, arg) => {
    console.log(arg);
    storage.set('token', { token: arg }, (error) => {
      mb.window.webContents.send('status-changed', "logg-off");
    });
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
  storage.get("token", (error, data) => {
    if (error) {
      mb.window.webContents.send('status-changed', "log-off");
    } if(data.length) {
      //todo: check se token scaduto
      console.log(data);
      mb.window.webContents.send('status-changed', "log-in");
    }
  });
  mb.window.openDevTools();
});