const net = require('net');

const socket = new net.Socket();

const DESIRED_SIZE = 8192;
let requestStr = "GET /index.html HTTP/1.1\r\nHost: localhost\r\nAuthentication: Bearer "

function generateRequest() {
  let size = requestStr.length;

  while (size != DESIRED_SIZE - 4) {
    requestStr += 'x';
    size++;
  }

  requestStr += "\r\n\r\n";

  console.log(`Request size: ${requestStr.length}`);
}

socket.connect({ host: 'localhost', port: 8000 });
socket.on('connect', () => {
  generateRequest();
  socket.write(requestStr, () => {
    socket.end();
  });
});
