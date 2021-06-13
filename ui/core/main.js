const { menubar } = require('../renderer/node_modules/menubar/lib');
const { ipcMain, app, Menu, Tray } = require('electron');
const udpStackManager = require('./udp_stack');
const auth_module = require('./modules/auth');

function on_open(menutItem, browserWindow, event) {
    mb.showWindow();
    mb.window.openDevTools();
}

function on_close(menutItem, browserWindow, event) {
    // client.send(Buffer.from([topics.get("EXIT")]), 2800, client_ip, (err) => { console.log(err) });
    udpStackManager.stop();
    mb.app.quit();
}

//const tray = new Tray("remote-icon.png");
const contextMenu = Menu.buildFromTemplate([
    { label: 'open', type: 'normal', click: on_open },
    { type: 'separator' },
    { label: 'close', type: 'normal', click: on_close },
]);

/**
 * contextIsolation: false -> otherwise electron main 
 * and renderer (Angular app would not be linked)
 */
const mb = menubar(
    {
        index: "file://" + __dirname + "/../renderer/dist/renderer/index.html",
        icon: __dirname + "/../renderer/dist/renderer/assets/remote-icon.png",
        tooltip: "show",
        browserWindow: {
            width: 450,
            height: 420,
            webPreferences: {
                nodeIntegration: true,
                contextIsolation: false,
            }
        }
    });

mb.on('ready', () => {

    console.log('app is ready');
    mb.tray.setContextMenu(contextMenu);

    udpStackManager.run();

});

mb.on('after-create-window', () => {
    auth_module.run(mb.window.webContents);
});
