const http = require('http');
const fs = require('fs');

let message, fileName


fs.mkdir('./client_dir', (err) => {
})

const server = http.createServer((req, res) => {
    req.on('data', (msg) => {
        message = msg.toString('utf8')
        console.log(msg)
        if(message.slice(0, 8) === 'FileName'){
            fileName = message.substr(8)
            console.log(fileName)
            fs.unlink(`./client_dir/${fileName}`, (err) => {
                console.log(err)
            });
        }
        else {
            fs.appendFile(`./client_dir/${fileName}`, message, (err) => {
                console.log(err)
            })
        }
        res.writeHead(200,{'Access-Control-Allow-Origin':'*'});
    })
});
 
server.listen(5000);
 
console.log('Node.js web server at port 5000 is running..')