const fs = require('fs');

module.exports = class Token {

    constructor() {
        this.format_rgx = /^\{\"token\"\:\s?\"[a-zA-Z0-9=]+\.[a-zA-Z0-9=]+\.[a-zA-Z0-9=]+\"\}$/;
        this.value_rgx = /^[a-zA-Z0-9=]+\.[a-zA-Z0-9=]+\.[a-zA-Z0-9=]+$/;
        this.value;
        this.read();
    }

    read() {
        fs.readFile('../client/config/token.json', 'utf8', (err, jsonString) => {
            if (err) {
                console.log("File read failed:", err)
                return
            }
            if (this.rightFormat(jsonString)) {

                this.value = JSON.parse(jsonString).token;
                return;
            }
            this.value = undefined;
        });
    }

    write() {
        fs.writeFile('../client/config/token.json', JSON.stringify({ token: this.value }), (err) => {
            if (err) {
                console.log("File read failed:", err)
                return
            }
        });
    }

    set(str_token_value) {
        if (this.isValid(str_token_value)) {
            this.value = str_token_value;
            this.write();
            return true;
        }
        return false;
    }

    reset() {
        this.value = "";
        this.write();
    }

    isExpired() {
        const buff = Buffer.from(this.value.split(".")[1], "base64");
        const exp = JSON.parse(buff.toLocaleString("ascii")).exp;
        const now = Math.round((new Date()).getTime() / 1000);
        console.log("exp:", exp, ", now:", now);
        return exp <= now;
    }

    isValid() {
        return this.value && this.value_rgx.test(this.value) && !this.isExpired();
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