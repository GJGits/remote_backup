var request = require('sync-request');
const fs = require("fs");
const crypto = require('crypto');

Reset = "\x1b[0m"
Bright = "\x1b[1m"
Dim = "\x1b[2m"
Underscore = "\x1b[4m"
Blink = "\x1b[5m"
Reverse = "\x1b[7m"
Hidden = "\x1b[8m"

FgBlack = "\x1b[30m"
FgRed = "\x1b[31m"
FgGreen = "\x1b[32m"
FgYellow = "\x1b[33m"
FgBlue = "\x1b[34m"
FgMagenta = "\x1b[35m"
FgCyan = "\x1b[36m"
FgWhite = "\x1b[37m"

BgBlack = "\x1b[40m"
BgRed = "\x1b[41m"
BgGreen = "\x1b[42m"
BgYellow = "\x1b[43m"
BgBlue = "\x1b[44m"
BgMagenta = "\x1b[45m"
BgCyan = "\x1b[46m"
BgWhite = "\x1b[47m"

var test_cases = [];
const token1 = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJkYiI6MSwiZGV2aWNlX2lkIjoxLCJleHAiOjE2MDI5NDMzMTksInN1YiI6InJlbmF0byJ9.eyJzaWduIjoicFpsNklYR0Fic1Zwbnoxa1dtS3RxYi9Ed0Z0NVFiOHZWK0lKVGloRDUxUT0ifQ==";
const token2 = "";
const token3 = "";
const chunk_size = 65536;
const base_url = "http://0.0.0.0:3200/chunk/";

const test = function (test_name, callback) {
    test_cases.push({ name: test_name, call: callback });
}

const run_test = function (combination, result) {
    for (let z = 0; z < test_cases.length; z++) {
        const passed = test_cases[z].call(combination, result);
        if (passed) {
            console.log(" - " + test_cases[z].name + ": %s%s%s", FgGreen, '\u2713', Reset);
        } else {
            console.log(" - " + test_cases[z].name + ": %s%s%s      [%s]", FgRed, '\u2716', Reset, test_cases[z].error);
        }
    }
}


// 1. genero chunks da file
file_list = ["./input/thread.pdf","./input/generali.pdf"];
buff_list = []; // lista dalla quale attingere buffer da inviare
file_list.forEach(fname => {
    let stats = fs.statSync(fname);
    let fileSizeInBytes = stats["size"];
    let num_of_chunks = Math.ceil(fileSizeInBytes / chunk_size);
    let fd_in = fs.openSync(fname, 'r');
    for (let i = 0; i < num_of_chunks; i++) {
        let to_read = i < (num_of_chunks - 1) ? chunk_size : (fileSizeInBytes - (chunk_size * (num_of_chunks-1)));
        let buf = Buffer.alloc(to_read);
        fs.readSync(fd_in, buf, 0, to_read, (i * chunk_size));
        let fname_base64 = new Buffer(fname);
        fname_base64 = fname_base64.toString("base64");
        // memorizzo solo 1/5 dei buffer per non utilizzare
        // troppa memoria
        //if (i % 5 == 0) {
            const hash = crypto.createHash('sha256');
            hash.update(buf);
            buff_list.push(
                {
                    content: buf,
                    id: i,
                    hash: hash.digest("hex"),
                    fname: fname_base64,
                    size: to_read,
                    last_mod: Math.ceil(stats.ctime.getTime() / 1000)
                });
        //}

        if (!fs.existsSync("./output/" + fname_base64)) {
            fs.mkdirSync("./output/" + fname_base64);
        }
        let f_out_name = "./output/" + fname_base64 + "/" + fname_base64 + "_" + i + ".chk";
        fs.writeFileSync(f_out_name, buf);

    }
    fs.closeSync(fd_in);
});

// 2. genero test
test("Check status", function (res, exp) {
    if (res.statusCode === exp.status)
        return true;
    this.error = "got: " + res.statusCode + ", expected: " + exp.status;
    return false;
});

test("Check response message", function (res, exp) {

    // ATTENZIONE CHE VENGONO RESTITUITI 4096 BYTES VUOTI SE NON C'È NULLA
   if(res.body.length != 0) {
       const body = JSON.parse(res.body.toString());
       if (body.error === exp.msg)
           return true;
       this.error = "got: " + body.error + ", expected: " + exp.msg;
       return false;
   }
   return true;

});

// 3. eseguo test
const combinations = [
    {
        method: "POST",
        url: base_url + "0/"+buff_list[0].hash+"/1/abd/12345",
        description: "richiesta senza token",
        content: Buffer.alloc(0),
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': 0}
    },
    {
        method: "POST",
        url: base_url + "0/abcd/7/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk con hash fornito errato",
        content: buff_list[0].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[0].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + ""+buff_list[0].hash+"/7/dGhyZWFkLnBkZg==/1602506513",
        description: "Non fornisco il chunk id",
        content: buff_list[0].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[0].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "0/7/dGhyZWFkLnBkZg==/1602506513",
        description: "Non fornisco l'hash",
        content: buff_list[0].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[0].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "0/"+buff_list[0].hash+"/dGhyZWFkLnBkZg==/1602506513",
        description: "Non fornisco il num chunks",
        content: buff_list[0].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[0].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "0/7/"+buff_list[0].hash+"/1602506513",
        description: "Non fornisco il nome del file",
        content: buff_list[0].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[0].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "0/7/"+buff_list[0].hash+"/dGhyZWFkLnBkZg==",
        description: "Non fornisco il timestamp",
        content: buff_list[0].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[0].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "0/7/",
        description: "Fornisco solo chunk id, chunk size e num_chunks",
        content: buff_list[0].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[0].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "0/"+buff_list[0].hash+"/7/dGhyZWFkLnBkZg==/1234",
        description: "Chunk 0 Esatto e giusto Thread.pdf",
        content: buff_list[0].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[0].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "2/"+buff_list[2].hash+"/7/dGhyZWFkLnBkZg==/1234",
        description: "Chunk 2 Esatto e giusto Thread.pdf",
        content: buff_list[2].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[2].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "1/"+buff_list[1].hash+"/7/dGhyZWFkLnBkZg==/1234",
        description: "Chunk 1 Esatto e giusto Thread.pdf",
        content: buff_list[1].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[1].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "3/"+buff_list[3].hash+"/7/dGhyZWFkLnBkZg==/1234",
        description: "Chunk 3 Esatto e giusto Thread.pdf",
        content: buff_list[3].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[3].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "4/"+buff_list[4].hash+"/7/dGhyZWFkLnBkZg==/1234",
        description: "Chunk 4 Esatto e giusto Thread.pdf",
        content: buff_list[4].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[4].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "5/"+buff_list[5].hash+"/7/dGhyZWFkLnBkZg==/1234",
        description: "Chunk 5 Esatto e giusto Thread.pdf",
        content: buff_list[5].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[5].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "6/"+buff_list[6].hash+"/7/dGhyZWFkLnBkZg==/1234",
        description: "Chunk 6 Esatto e giusto Thread.pdf",
        content: buff_list[6].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[6].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "0/"+buff_list[7].hash+"/9/Z2VuZXJhbGkucGRm/1234",
        description: "Chunk 0 Esatto e giusto Generali.pdf",
        content: buff_list[7].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[7].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "1/"+buff_list[8].hash+"/9/Z2VuZXJhbGkucGRm/1234",
        description: "Chunk 1 Esatto e giusto Generali.pdf",
        content: buff_list[8].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[8].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "1/"+buff_list[8].hash+"/9/Z2VuZXJhbGkucGRm/9999999999999999999",
        description: "Chunk 2 con timestamp esageratamente lungo Generali.pdf",
        content: buff_list[8].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[8].size,  'Authorization' : 'Bearer ' + token1 }
    },

];
const expected_results = [
    { status: 400, msg: "Auth failed" },
    { status: 400, msg: "The chunk received is different from the one calculated"},
    { status: 400, msg: "The format of the request is wrong!"},
    { status: 400, msg: "The format of the request is wrong!"},
    { status: 400, msg: "The format of the request is wrong!"},
    { status: 400, msg: "The format of the request is wrong!"},
    { status: 400, msg: "The format of the request is wrong!"},
    { status: 400, msg: "The format of the request is wrong!"},
    { status: 200},
    { status: 200},
    { status: 200},
    { status: 200},
    { status: 200},
    { status: 200},
    { status: 200},
    { status: 200},
    { status: 200},
    { status: 400, msg: "stoi"},

];

console.log("\n");
for (let y = 0; y < combinations.length; y++) {
    console.log("%s%s%s%s", FgWhite, '\u2192', " " + combinations[y].method + " " + combinations[y].url + " [" + combinations[y].description + "]", Reset);
    const res = request(combinations[y].method, combinations[y].url, { body: combinations[y].content, headers: combinations[y].headers });
    run_test(res, expected_results[y]);
    console.log("\n");
}
