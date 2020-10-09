var request = require('sync-request');
const fs = require("fs");

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
const token1 = "";
const token2 = "";
const token3 = "";
const chunk_size = 4096;
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
            console.log(" - " + test_cases[z].name + ": %s%s%s", FgRed, '\u2716', Reset);
        }
    }
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
test("Check status", (res, exp) => {
    return res.statusCode === exp;
});

test("Check response message", (res, exp) => {
    return false;
});

// 3. eseguo test
const combinations = [
    {   
        description: "no auth req:",
        url: base_url + "/0/abc/1/abd/12345",
        content: Buffer.alloc(0),
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': 0 }
    }
];
const expected_results = [
    { status: 500, msg: "some error message here..." }
];

console.log("\n");
for (let y = 0; y < combinations.length; y++) {
    console.log("%s%s%s", FgWhite, combinations[y].description, Reset);
    const res = request("POST", combinations[y].url, { body: combinations[y].content, headers: combinations[y].headers });
    run_test(res, expected_results[y]);
    console.log("\n");
}
