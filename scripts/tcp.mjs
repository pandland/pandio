import { createServer } from 'net';

createServer((socket) => {
  console.log('Client connected');

  socket.on('data', (data) => {
    console.log(data.toString());
    socket.end(`Echo: ${data}`);
  });
}).listen(8000);
