const http = require('http');

const totalRequests = 1000;
const port = 8000;

function sendRequest() {
    return new Promise((resolve, reject) => {
        const options = {
            hostname: '127.0.0.1',
            port: port,
            path: '/',
            method: 'GET'
        };

        const req = http.request(options, (res) => {
            let data = '';
            res.on('data', (chunk) => {
                data += chunk;
            });
            res.on('end', () => {
                resolve(data);
            });
        });

        req.on('error', (error) => {
            reject(error);
        });

        req.end();
    });
}

const requests = [];

for (let i = 0; i < totalRequests; i++) {
    requests.push(sendRequest());
}

Promise.all(requests)
    .then((results) => {
        console.log('All requests completed successfully!');
        console.log('Response data from each request:');
        results.forEach((data, index) => {
            console.log(`Request ${index + 1}: ${data}`);
        });
    })
    .catch((error) => {
        console.error('An error occurred during requests:', error);
    });