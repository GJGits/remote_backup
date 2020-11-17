const fs = require('fs');

const CONFIG_ROOT = "../client/config2/client-conf.json"

module.exports = class ClientConf {

    constructor() {
        this.format_rgx = /^\{\"username\":\s?\"\w*\",\s?\"token\"\:\s?\"[a-zA-Z0-9=]+\.[a-zA-Z0-9=]+\.[a-zA-Z0-9=]+\"\}$/;
        this.value_rgx = /^[a-zA-Z0-9=]+\.[a-zA-Z0-9=]+\.[a-zA-Z0-9=]+$/;
        this.content;
        this.read();
    }

    read() {
        this.content = JSON.parse(fs.readFileSync(CONFIG_ROOT, 'utf8'));
    }

    write() {
        fs.writeFileSync(CONFIG_ROOT, JSON.stringify(this.content), (err) => {
            if (err) {
                console.log("File read failed:", err)
            }
        });
    }

    set(config) {
        this.content = config;
        this.write();
    }

    reset() {
        this.content.username = "";
        this.content.token = "";
        this.write();
    }

    isExpired() {
        const buff = Buffer.from(this.content.token.split(".")[1], "base64");
        const exp = JSON.parse(buff.toLocaleString("ascii")).exp;
        const now = Math.round((new Date()).getTime() / 1000);
        console.log("exp:", exp, ", now:", now);
        return exp <= now;
    }

    isValid() {
        console.log('content:', this.content);
        return this.content && this.value_rgx.test(this.content.token) && !this.isExpired();
    }

    rightFormat(json_string) {
        if (!json_string || json_string === "")
            return false;
        json_string = json_string.replace("\n", "");
        json_string = json_string.replace("\r", "");
        if (!this.format_rgx.test(json_string)) {
            console.log("regex not passed:", json_string);
            return false;
        }
        return true;
    }


};
