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
        let to_read = i < (num_of_chunks - 1) ? chunk_size : ((chunk_size * num_of_chunks) - fileSizeInBytes);
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
                    last_mod: stats.ctimeMs
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
        url: base_url + "0/abc/1/abd/12345",
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
        url: base_url + "abcd/7/dGhyZWFkLnBkZg==/1602506513",
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
        url: base_url + "0/abcd/dGhyZWFkLnBkZg==/1602506513",
        description: "Non fornisco il num chunks",
        content: buff_list[0].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[0].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "0/7/abcd/1602506513",
        description: "Non fornisco il nome del file",
        content: buff_list[0].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[0].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "0/7/abcd/dGhyZWFkLnBkZg==",
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
        url: base_url + "0/e38a6ac491749f0c9de66384e732d3f1cb17cd045827525cf04d3e6dd17d8224/7/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 0 Esatto e giusto Thread.pdf",
        content: buff_list[0].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[0].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "2/80c3f7c017045bfe2a77aa7486a12631b0ae7f60750a6c3c0f6abd7b2e00c22e/7/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 2 Esatto e giusto Thread.pdf",
        content: buff_list[2].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[2].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "1/fb29b54398627437ca82ece684ea2feb1b771882e1d45a5acb338bfa158a2327/7/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 1 Esatto e giusto Thread.pdf",
        content: buff_list[1].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[1].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "3/25832f8a8120e0f4b6d57d9ef0372a8fb08fed55460604fefb9d289acf97b8d9/7/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 3 Esatto e giusto Thread.pdf",
        content: buff_list[3].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[3].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "4/c921ed2e4f80a4fbe91bceba64c9b84c3bace5f202386f8ba7e55b3468b1037a/7/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 4 Esatto e giusto Thread.pdf",
        content: buff_list[4].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[4].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "5/2676d091260928595d7d97fb3fbb8677ed6410a4826da8b3b31325341bca550f/7/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 5 Esatto e giusto Thread.pdf",
        content: buff_list[5].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[5].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "6/09c347df11c9518f8408f56992acf090bc48485497f0dc649c7b22bafc301cd8/7/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 6 Esatto e giusto Thread.pdf",
        content: buff_list[6].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[6].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "0/2e03b87fae9a2104bdc40bc1f5a4db4007566f8666c196e113abfb5d4e63226f/9/Z2VuZXJhbGkucGRm/1602506511",
        description: "Chunk 0 Esatto e giusto Generali.pdf",
        content: buff_list[7].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[7].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "1/0ba53c61e57398843c6fafff1ab46495254d4e13a6341c5cb4531a73866fa514/9/Z2VuZXJhbGkucGRm/1602506511",
        description: "Chunk 1 Esatto e giusto Generali.pdf",
        content: buff_list[8].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[8].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "1/7d96cd8fc8a94ea2c04eac39a4ab22e7a8e0e017778846cf21ffc1c9161a7fed/9/Z2VuZXJhbGkucGRm/16025065119999",
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
