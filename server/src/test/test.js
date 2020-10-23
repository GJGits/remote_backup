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
            console.log(" - " + test_cases[z].name + ": %s%s%s      [%s]", FgRed, '\u2716', Reset, test_cases[z].error);
        }
    }
}


// 1. genero chunks da file
file_list = ["./input/thread.pdf","./input/bello.txt", "./input/generali.pdf"];
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
                    last_mod: Math.floor(new Date().getTime() / 1000)
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
        url: base_url + "0/abcd/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk con hash fornito errato",
        content: buff_list[0].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[0].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "abcd/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Non fornisco il chunk id",
        content: buff_list[0].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[0].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "0/108/dGhyZWFkLnBkZg==/1602506513",
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
        url: base_url + "0/108/abcd/1602506513",
        description: "Non fornisco il nome del file",
        content: buff_list[0].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[0].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "0/108/abcd/dGhyZWFkLnBkZg==",
        description: "Non fornisco il timestamp",
        content: buff_list[0].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[0].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "0/108/",
        description: "Fornisco solo chunk id, chunk size e num_chunks",
        content: buff_list[0].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[0].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "0/efdda7eaaf1b55ac9a254225a1286eed1121ff96c3915bf706523f15bce26948/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 0 Esatto e giusto Thread.pdf",
        content: buff_list[0].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[0].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "2/ce329df635da8f8735ec85b803c71f9a396330d613fed144725f9669758b05af/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 2 Esatto e giusto Thread.pdf",
        content: buff_list[2].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[2].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "1/a3842fc6ad662e2d9447c945b482cfe73fd13076da7870a78163e269940b3fbe/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 1 Esatto e giusto Thread.pdf",
        content: buff_list[1].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[1].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "3/2eeae3d0abc5a386448763db01fe3aae13dd2846101a2e39c2b9370b82eac394/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 3 Esatto e giusto Thread.pdf",
        content: buff_list[3].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[3].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "4/98416b6c90780c29867d70c89e0647afed0d74617ebb123aff2799ccc7b075f0/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 4 Esatto e giusto Thread.pdf",
        content: buff_list[4].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[4].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "5/2ff6dc0539f97bab0720e263f415623ae04035216a677f9f2e0d6574a0dbe9c8/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 5 Esatto e giusto Thread.pdf",
        content: buff_list[5].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[5].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "6/539416989fe1f7f45df911548ddcf587b50e0f824335f40e80dc792d8cff10eb/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 6 Esatto e giusto Thread.pdf",
        content: buff_list[6].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[6].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "7/0d73cfdba1c28fdc6aafcb4c6f389452d620b13570485de4b14ab3ac724c3a94/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 7 Esatto e giusto Thread.pdf",
        content: buff_list[7].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[7].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "8/d2c486c4e0eeada34ca992a1569cbf9d81773a300fbf77096741482edc22b688/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 8 Esatto e giusto Thread.pdf",
        content: buff_list[8].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[8].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "9/c8c3ea10de74efe47a3b1a5ea580ec8f7157e3b10a6a35c096323224874687c8/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 9 Esatto e giusto Thread.pdf",
        content: buff_list[9].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[9].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "10/6104379952ba1478478dc79a3dfb05b4fd1f97c6e055c5476f6aae5c95ad2af9/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 10 Esatto e giusto Thread.pdf",
        content: buff_list[10].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[10].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "11/4907f83decdc717e7f9347cb8307c7481d1868f39887088d63f1bd230c7c8cb3/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 11 Esatto e giusto Thread.pdf",
        content: buff_list[11].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[11].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "12/f371da4ad09b2c1b1e75e8048dab809cc583fafc879c81f3c56733a5e6d9b08d/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 12 Esatto e giusto Thread.pdf",
        content: buff_list[12].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[12].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "13/b4b7c638e4c759aa1729fe4d7440b9133e358bdbeb0f7fbacb0d9ba058066dc4/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 13 Esatto e giusto Thread.pdf",
        content: buff_list[13].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[13].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "14/27fe74b83e669fbf3bb0655c18479df8e04f81e945afc13edf37804adc9a242d/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 14 Esatto e giusto Thread.pdf",
        content: buff_list[14].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[14].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "15/09d7713db39fa189cb3accf90a668c8691094810ae0c505811d6a0d966ebe76d/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 15 Esatto e giusto Thread.pdf",
        content: buff_list[15].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[15].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "16/41f17f96b403f1a1fa3b73c9cb9f58857a7317eb4f41c4506f807e201509c066/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 16 Esatto e giusto Thread.pdf",
        content: buff_list[16].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[16].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "17/7d7c69034026e4804336893730f66a7867013d14643da36c1aaad5543983904e/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 17 Esatto e giusto Thread.pdf",
        content: buff_list[17].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[17].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "18/efc96ffae26c7f6822560c2c269f379c7f0ed7db8a5c02ac2c3e0a96510483ba/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 18 Esatto e giusto Thread.pdf",
        content: buff_list[18].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[18].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "19/082b2f6a2d78880048f6684e6b214d3fddd7c16b08888cbca8064493c2c101a5/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 19 Esatto e giusto Thread.pdf",
        content: buff_list[19].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[19].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "20/bb5f0bd8f88c66ab4af5e62e501ce61028dc3003f1176957069d23f9eb8110ae/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 20 Esatto e giusto Thread.pdf",
        content: buff_list[20].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[20].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "21/4249601816ad75640bb7bf010ad99955b79d79eae2ec66ca65fb3929d306e67c/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 21 Esatto e giusto Thread.pdf",
        content: buff_list[21].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[21].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "22/084cffebb8e4b0ff0a5624993474e08233008598602bb036cc6f32e8d006bc65/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 22 Esatto e giusto Thread.pdf",
        content: buff_list[22].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[22].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "23/c3d86f7418fa95a3ac9f351fcde84a59a190dfe060320af2b9cea319ed56715b/108/dGhyZWFkLnBkZg==/1602506513",
        description: "Chunk 23 Esatto e giusto Thread.pdf",
        content: buff_list[23].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[23].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "0/20c606802c9d54f9b9e48d44c421e9c4a2b2f35748aa6e8d89c461aafff7cc2f/2/YmVsbG8udHh0/1602506513",
        description: "Chunk 0 di bello.txt",
        content: buff_list[128].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[128].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "1/4a0a08009e3687c8ef9e4d8b90fc76918a1ad35eb143eb38ac99123e8af8a486/2/YmVsbG8udHh0/1602506513",
        description: "Chunk 1 di bello.txt",
        content: buff_list[129].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': 883,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "0/ab7957a32eef08e3c9b2f7493b706b546316fd421cecb5c1da90e200c9a72c7c/143/Z2VuZXJhbGkucGRm/1602506511",
        description: "Chunk 0 Esatto e giusto Generali.pdf",
        content: buff_list[130].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[130].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "1/7d96cd8fc8a94ea2c04eac39a4ab22e7a8e0e017778846cf21ffc1c9161a7fed/143/Z2VuZXJhbGkucGRm/1602506511",
        description: "Chunk 1 Esatto e giusto Generali.pdf",
        content: buff_list[131].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[131].size,  'Authorization' : 'Bearer ' + token1 }
    },
    {
        method: "POST",
        url: base_url + "1/7d96cd8fc8a94ea2c04eac39a4ab22e7a8e0e017778846cf21ffc1c9161a7fed/143/Z2VuZXJhbGkucGRm/16025065119999",
        description: "Chunk 2 con timestamp esageratamente lungo Generali.pdf",
        content: buff_list[132].content,
        headers: { 'Content-Type': 'octect/stream', 'Content-Length': buff_list[132].size,  'Authorization' : 'Bearer ' + token1 }
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
    { status: 200},
    { status: 200},
    { status: 200},
    { status: 200},
    { status: 200},
    { status: 200},
    { status: 200},
    { status: 200},
    { status: 200},
    { status: 200},
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
