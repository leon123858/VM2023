const Koa = require('koa');
const Router = require('koa-router');
const fs = require('fs');
const staticServer = require('koa-static');
const views = require('koa-views');
const path = require('path');
const cors = require('koa2-cors');
const app = new Koa();
const router = new Router();
const server = require('http').createServer(app.callback());
const io = require('socket.io')(server, {
	cors: {
		origin: '*',
		methods: ['GET', 'POST'],
		allowedHeaders: ['token'],
	},
});
app.use(cors());
app.use(
	views(path.join(__dirname, './view'), {
		extension: 'ejs',
	})
);
app.use(staticServer(__dirname, 'public'));

// socketIO
io.on('connection', (socket) => {
	fs.appendFileSync(
		'./sent.txt',
		'r11922114 sending from the server\n',
		(err) => {
			if (err) {
				console.error(err);
			}
			// file written successfully
		}
	);
	socket.emit('message', 'r11922114 sending from the server');
	socket.on('disconnect', function () {
		console.log('disconnect');
	});
});

// general API
router
	.get('/', (ctx) => {
		ctx.body = '這是 node socket io 的實作, 進入 test 頁面試用';
	})
	.get('/about', (ctx) => {
		ctx.body = '由 Leon Lin 於練習製作';
	})
	.get('/test', async (ctx) => {
		let title = 'socket IO demo';
		await ctx.render('socketIO', {
			title,
		});
	});
app.use(router.routes());
app.listen(3000);
server.listen(8080);
console.log('[demo] start-quick is starting at port 3000');
