var request = require('sync-request');
const fs = require("fs");
const crypto = require('crypto');

const token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJkYiI6MSwiZGV2aWNlX2lkIjoxLCJleHAiOjE2MDQ4MjU4MDAsInN1YiI6InJlbmF0byJ9.eyJzaWduIjoiV3pyTnp1YS8rc3RZNVovSjFLK0pDbDA2a2laRWN5MWJBNkl2V0xEbXl1TT0ifQ==";
const chunk_size = 65536;
const base_url = "http://0.0.0.0:3200/";

// 1. genero chunks da file
file_list = ["administrator_view_selected_class.php"];
buff_list = []; // lista dalla quale attingere buffer da inviare
action = "post";

if (action === "post") {
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
            const uri = base_url + "chunk/" + i + "/" + hash.digest("hex") + "/" + num_of_chunks + "/" + fname_base64 + "/" + Math.ceil(stats.ctime.getTime() / 1000);
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
}

if (action === "delete") {
    file_list.forEach(fname => {
        let serv_fname = "./sync/" + fname;
        let fname_base64 = new Buffer(serv_fname);
        fname_base64 = fname_base64.toString("base64");
        const uri = base_url + "file/" + fname_base64 + "/" + Math.ceil(new Date() / 1000);
        const content = { headers: { 'Authorization': 'Bearer ' + token } };
        const res = request("DELETE", uri, content);
        console.log(res.statusCode);
        console.log(res);
    });
}

