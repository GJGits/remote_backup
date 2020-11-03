var request = require('sync-request');
const fs = require("fs");
const crypto = require('crypto');

const token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJkYiI6MiwiZGV2aWNlX2lkIjo0LCJleHAiOjE2MDQ1Mjc5MzEsInN1YiI6InJlbmF0byJ9.eyJzaWduIjoiOEQwT1grQjdrVDd2Z0VqM0ZUVHB2UHpRRW1abzZLWjBob2k4dnBPSDZUUT0ifQ==";
const chunk_size = 65536;
const base_url = "http://0.0.0.0:3200/chunk/";

// 1. genero chunks da file
file_list = ["c#.pdf"];
buff_list = []; // lista dalla quale attingere buffer da inviare
file_list.forEach(fname => {
    let loc_fname = "./input/" + fname;
    let serv_fname = "./sync/" + fname;
    let stats = fs.statSync(loc_fname);
    let fileSizeInBytes = stats["size"];
    let num_of_chunks = Math.ceil(fileSizeInBytes / chunk_size);
    let fd_in = fs.openSync(loc_fname, 'r');
    for (let i = 0; i < num_of_chunks; i++) {
        let to_read = i < (num_of_chunks - 1) ? chunk_size : (fileSizeInBytes - (chunk_size * (num_of_chunks - 1)));
        let buf = Buffer.alloc(to_read);
        fs.readSync(fd_in, buf, 0, to_read, (i * chunk_size));
        let fname_base64 = new Buffer(serv_fname);
        fname_base64 = fname_base64.toString("base64");
        const hash = crypto.createHash('sha256');
        hash.update(buf);
        const uri = base_url + i + "/" + hash.digest("hex") + "/" + num_of_chunks + "/" + fname_base64 + "/" + Math.ceil(stats.ctime.getTime() / 1000);
        const content = {
            body: buf,
            headers: {
                'Content-Type': 'octect/stream',
                'Content-Length': to_read,
                'Authorization': 'Bearer ' + token
            }
        };
        const res = request("POST", uri, content);
        console.log(res.statusCode);
    }
    fs.closeSync(fd_in);
});