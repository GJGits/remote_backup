const { ipcMain } = require('electron');
const udpStackManager = require('../udp_stack');
const ClientConf = require('./client-conf.js');

var user_info = {};
var auth_module = {};
var ipcRenderer = {};

ipcMain.handle('signin', async (event, data) => {
    //todo: generate real response
    return { user: 'Pinco', device: 'device 1' };
});

ipcMain.handle('signup', async (event, data) => {
    //todo: generate real response
    return { user: 'Pinco', device: 'device 1' };
});

function loadUserInfo() {
    let conf = new ClientConf();
    if (conf.isValid()) {
        let info = conf.get_info();
        ipcRenderer.send('user_info', info);
        udpStackManager.send(1); // 1 = START
    }

    else {
        ipcRenderer.send('cresentials_expired');
        udpStackManager.send(0); // 0 = STOP
    }
}

udpStackManager.on('auth_failed').subscribe(() => {
    ipcRenderer.send('credentials_exipred');
});

auth_module.run = function (renderer) {
    ipcRenderer = renderer;
    loadUserInfo();
}

module.exports = auth_module;

