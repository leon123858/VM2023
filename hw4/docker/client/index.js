const { io } = require('socket.io-client');
const fs = require('fs');

let isConnect = false;
var socket = io.connect('http://localhost:8080');

socket.on('connect', function () {
	isConnect = true;
	socket.on('message', function (data) {
		fs.appendFileSync('./received.txt', data + '\n', (err) => {
			if (err) {
				console.error(err);
			}
			// file written successfully
		});
		socket.disconnect(true);
		process.exit(0);
	});
});

setTimeout(() => {
	if (!isConnect) {
		console.log('連線逾時(3s)');
		process.exit(0);
	}
}, 3000);
