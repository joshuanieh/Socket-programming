var http = require('http');
var fs = require('fs');
 
var server = http.createServer(function (req, res) {
    req.on('data', (msg) => {
        let message = msg.toString('utf8')
        let fileName = message.substr(0, message.indexOf(' '))
        let content = message.substr(message.indexOf(' ') + 1)
        console.log(msg.toString('utf8'))
        fs.writeFile(`./client_dir/${fileName}`, content, (err) => {
            console.log(err)
        })
    })
});
 
server.listen(5000);
 
console.log('Node.js web server at port 5000 is running..')