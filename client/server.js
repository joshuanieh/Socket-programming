const http = require('http');
const fs = require('fs');
 
const server = http.createServer((req, res) => {
    req.on('data', (msg) => {
        let message = msg.toString('utf8')
        // console.log(message)
        let fileName = message.substr(0, message.indexOf(' '))
        let content = message.substr(message.indexOf(' ') + 1)

        fs.mkdir('./client_dir', (err) => {
            console.log(err)
        })
        
        fs.writeFile(`./client_dir/${fileName}`, content, (err) => {
            console.log(err)
        })
    })
});
 
server.listen(5000);
 
console.log('Node.js web server at port 5000 is running..')