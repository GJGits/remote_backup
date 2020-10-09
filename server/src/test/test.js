const { Table } = require('console-table-printer');
var http = require('http');
const fs = require("fs");

const p = new Table({
    columns: [
        { name: "test", alignment: "center", color: "white_bold" },
        { name: "status", alignment: "center" }
    ]
});
var test_cases = [];
const token1 = "";
const token2 = "";
const token3 = "";
const chunk_size = 4096;

const post_chunk = function (chunk, content_length, expected_status, token = undefined) {
    let headers = { 'Content-Type': 'octect/stream', 'Content-Length': content_length };
    if (token)
        headers.Authorization = 'Bearer ' + token;
    const options = {
        hostname: '0.0.0.0',
        port: 3200,
        path: '/chunk/' + chunk.id + '/' + content_length + '/' + chunk.hash + '/' + chunk.nchunks + '/' + chunk.path_base64 + '/' + chunk.last_mod,
        method: 'POST',
        headers: headers
    };
    const req = http.request(options, (res) => {
        return res.statusCode;
    });
    return req;
}

const test = function (test_name, callback) {
    test_cases.push({ name: test_name, call: callback });
}

// 1. genero chunks da file
file_list = ["./input/thread.pdf", "./input/generali.pdf"];
file_list.forEach(fname => {
    let stats = fs.statSync(fname);
    let fileSizeInBytes = stats["size"];
    let num_of_chunks = Math.ceil(fileSizeInBytes / chunk_size);
    let fd_in = fs.openSync(fname, 'r');
    for (let i = 0; i < num_of_chunks; i++) {
        let to_read = i < (num_of_chunks - 1) ? chunk_size : ((chunk_size * num_of_chunks) - fileSizeInBytes);
        let buf = Buffer.alloc(to_read);
        fs.readSync(fd_in, buf, 0, to_read, (i * chunk_size));
        let fname_base64 = new Buffer(fname);
        fname_base64 = fname_base64.toString("base64");
        if (!fs.existsSync("./output/" + fname_base64)) {
            fs.mkdirSync("./output/" + fname_base64);
        }
        let f_out_name = "./output/" + fname_base64 + "/" + fname_base64 + "_" + i + ".chk";
        fs.writeFileSync(f_out_name, buf);

    }
    fs.closeSync(fd_in);
});

// 2. genero test
test("No auth post", () => {
    return true;
});

test("Wrong format post", () => {
    return false;
});

// 3. eseguo test
for (let z = 0; z < test_cases.length; z++) {
    const passed = test_cases[z].call();
    if (passed) {
        p.addRow({ test: test_cases[z].name, status: '\u2713' }, { color: 'green' });
    } else {
        p.addRow({ test: test_cases[z].name, status: '\u2716' }, { color: 'red' });
    }
}
p.printTable();
