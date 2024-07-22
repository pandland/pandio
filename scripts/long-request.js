const http = require('http');

function generateLongString(size) {
    return 'a'.repeat(size);
}

const querySize = 9000 - 'GET /test?param= HTTP/1.1\r\nHost: localhost:8000\r\n\r\n'.length;
const longString = generateLongString(querySize);

// Opcje zapytania HTTP
const options = {
    hostname: 'localhost',
    port: 8000,
    path: `/test?param=${longString}`,
    method: 'GET',
    headers: {
        'Content-Type': 'application/x-www-form-urlencoded',
    },
};

const req = http.request(options, (res) => {
    let data = '';

    res.on('data', (chunk) => {
        data += chunk;
    });

    res.on('end', () => {
        console.log('Response:', data);
    });
});

req.on('error', (e) => {
    console.error(`Problem with request: ${e.message}`);
});

req.end();
