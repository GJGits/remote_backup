// INCLUDES
const { menubar } = require('menubar');
const dgram = require('dgram');
const server = dgram.createSocket('udp4');
const { ipcMain } = require('electron');
const textEncoding = require('text-encoding');
const storage = require('electron-json-storage');
const fs = require('fs');

// CONSTANTS
const mb = menubar(
  {
    icon: "remote-icon.png",
    tooltip: "show",
    browserWindow: {
      webPreferences: { nodeIntegration: true }
    }
  });

var token = "";


mb.on('ready', () => {

  console.log('app is ready');

  fs.readFile('../client/config/token.json', 'utf8', (err, jsonString) => {
    if (err) {
      console.log("File read failed:", err)
      return
    }
    console.log('File data:', jsonString)
    token = JSON.parse(jsonString);
  });

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
    // todo: token check format
    storage.set('token', { token: arg }, (error) => {
      mb.window.webContents.send('status-changed', "log-off");
      event.returnValue = "ko";
    });
    event.returnValue = "ok";
    mb.window.webContents.send('status-changed', "log-in");
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
  console.log("create window:", new Date().getTime());
  storage.get("token", (error, data) => {
    console.log("data:", data);
    if (error) {
      console.log("token not found");
      mb.window.webContents.send('status-changed', "log-off");
    } if (data) {
      const buff = Buffer.from(data.token.token.split(".")[1], "base64");
      const exp = JSON.parse(buff.toLocaleString("ascii")).exp;
      const now = Math.round((new Date()).getTime() / 1000);
      console.log("exp:", exp, "now:", now);
      if (exp <= now) {
        mb.window.webContents.send('status-changed', "log-off");
        storage.remove("token", (error) => { console.log(error) });
      } else {
        mb.window.webContents.send('status-changed', "log-in");
      }
    }
  });
  mb.window.openDevTools();
  // todo: inviato per test su signin/signup rimuovere
  //mb.window.webContents.send('status-changed', "log-off");
});