const axios = require('axios');

const url = 'http://127.0.0.1:5000/';
const numRequests = 1;

async function sendRequest(url) {
    try {
        const response = await axios.get(url);
        console.log(response.data)
    } catch (error) {
        console.error(`Error sending request to ${url}:`, error);
        return null;
    }
}

async function runTests() {
    const promises = [];
    for (let i = 0; i < numRequests; i++) {
        promises.push(sendRequest(url));
    }

    try {
        const results = await Promise.all(promises);
        console.log(`Successfully sent ${results.length} requests concurrently.`);
    } catch (error) {
        console.error('Error sending requests:', error);
    }
}

runTests();