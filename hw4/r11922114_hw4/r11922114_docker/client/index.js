const { io } = require('socket.io-client');
const fs = require('fs');

let isConnect = false;
var socket = io.connect(process.env.URL || 'http://localhost:8080');
console.log('try to link:', process.env.URL || 'http://localhost:8080');

socket.on('connect', function () {
	isConnect = true;
	socket.on('message', function (data) {
		fs.appendFileSync(
			process.env.DEBUG
				? './received.txt'
				: '/r11922114_clientvol/received.txt',
			data + '\n',
			(err) => {
				if (err) {
					console.error(err);
				}
				// file written successfully
			}
		);
		socket.disconnect(true);
		console.log('res:', data);
	});
});

setTimeout(() => {
	if (!isConnect) {
		console.log('連線逾時(30s)');
		process.exit(0);
	}
}, 30000);
