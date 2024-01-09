const http = require('http');

const totalRequests = 2; // Number of requests to send
const port = 8000; // Replace with your server's port

// Function to send a single request
function sendRequest() {
    return new Promise((resolve, reject) => {
        const options = {
            hostname: '127.0.0.1', // Replace with your server's hostname
            port: port,
            path: '/', // Replace with your server's endpoint
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

// Array to hold all the request promises
const requests = [];

// Create multiple requests and store their promises in the array
for (let i = 0; i < totalRequests; i++) {
    requests.push(sendRequest());
}

// Execute all the requests simultaneously
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