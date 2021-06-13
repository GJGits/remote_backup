const dgram = require('dgram');
const server = dgram.createSocket('udp4');
const client = dgram.createSocket('udp4');
var net = require('net');
const { Subject, Observable, of } = require('rxjs');
const execSync = require('child_process').execSync;

const subjectsContainer = {
    'auth_failed': new Subject(),
    'connection_lost': new Subject(),
    'transfer': new Subject()
};

var client_ip = '0.0.0.0';
var client_port = 2800;
var udpStackManager = {};

const initialize_server = function () {
    server.bind(41234);
    server.on('error', udpStackManager.onError);
    server.on('message', udpStackManager.onMessage);
    server.on('listening', udpStackManager.onListening);
}

const set_client_udp_info = function () {
    console.log(process.cwd());
    let command = __dirname + "/find_client_ip";
    client_ip = execSync(command).toString().replace(/(\r\n|\n|\r)/gm, "");
}

const mock_network_constraints = function () {
    const id = execSync("docker exec remote_backup_client_1 cat /sys/class/net/eth0/iflink").toString().replace(/(\r\n|\n|\r)/gm, "");
    const grep_res = execSync("ip ad | grep " + id).toString().replace(/(\r\n|\n|\r)/gm, "");
    const iface_rgx = /\d{1,2}\:\s(veth\w+)@/g;
    const dock_cli_iface = iface_rgx.exec(grep_res)[1];
    if (dock_cli_iface) {
        execSync("sudo tc qdisc add dev " + dock_cli_iface + " root tbf rate 30mbit burst 1mbit latency 400ms");
    }
}

udpStackManager.onListening = function () {
    const address = server.address();
    console.log(`server listening ${address.address}:${address.port}`);
}

udpStackManager.onError = function (err) {
    console.log(`server error:\n${err.stack}`);
    server.close();
}

udpStackManager.onMessage = function (msg, info) {
    //console.log(`server got: ${msg} from ${info.address}:${info.port}`);
    msg = new TextDecoder("utf-8").decode(msg);
    let obj = JSON.parse(msg);
    subjectsContainer[obj.code].next(obj.message);
}

udpStackManager.on = function (topic) {
    if (subjectsContainer[topic])
        return subjectsContainer[topic].asObservable();
    return of({});
}

udpStackManager.send = function(message) {
    client.send(Buffer.from([message]), client_port, client_ip, (error) => {
        // todo: error handling strategy here...
        console.error(error);
    });
}

udpStackManager.run = function () {
    initialize_server();
    //set_client_udp_info();
    //mock_network_constraints();
}

udpStackManager.stop = function () {
    // todo: close is async, we need to be sure that stop ends
    //       when also close has finished
    server.close();
    subjectsContainer.auth_failed.unsubscribe();
    subjectsContainer.connection_lost.unsubscribe();
    subjectsContainer.transfer.unsubscribe();
}

module.exports = udpStackManager;