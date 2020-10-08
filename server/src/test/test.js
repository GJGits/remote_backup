const { Table } = require('console-table-printer');
const fs = require("fs");
const p = new Table();
p.addRow({ text: 'red wine', value: '\u2716' }, { color: 'red' });
p.addRow({ text: 'red wine', value: '\u2713' }, { color: 'green' });
p.printTable();

const token1 = "";
const token2 = "";
const token3 = "";
const chunk_size = 4096;

// 1. genero chunks da file
file_list = ["./input/thread.pdf"];
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

// 2. effettuo test

