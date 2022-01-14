var http = require('http');
var fs = require('fs');
 
var server = http.createServer(function (req, res) {
    req.on('data', (msg) => {
        let data = msg.toString('utf8').split(' ')
        console.log(msg.toString('utf8'))
        fs.writeFile(`./client_dir/${data[0]}`, data[1], (err) => {
            console.log(err)
        })
    })
});
 
server.listen(5000);
 
console.log('Node.js web server at port 5000 is running..')