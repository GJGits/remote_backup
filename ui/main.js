// INCLUDES
const { menubar } = require('menubar');
const dgram = require('dgram');
const server = dgram.createSocket('udp4');
const { ipcMain } = require('electron');
const textEncoding = require('text-encoding');

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

  // UDP INSTANCE DEFINITION
  server.on('error', (err) => {
    console.log(`server error:\n${err.stack}`);
    server.close();
  });

  server.on('message', (msg, rinfo) => {
    console.log(`server got: ${msg} from ${rinfo.address}:${rinfo.port}`);
    msg = new TextDecoder("utf-8").decode(msg);
    mb.window.webContents.send('asynchronous-message', msg);
    //if (msg === "log-in" || msg === "log-off")
      mb.window.webContents.send('status-changed', msg);
  });

  server.on('listening', () => {
    const address = server.address();
    console.log(`server listening ${address.address}:${address.port}`);
  });

  server.bind(41234);

  // Appena app ok leggo token per verificare se utente logged-in e poi invio status
  // al processo render
  // todo: leggere da file...

});

mb.on('after-create-window', () => {
  mb.window.webContents.send('status-changed', "log-off");
  mb.window.openDevTools();
});