const ports = [8000,8001,8002];
const serverUrl = 'http://127.0.0.1'; // Replace with your server's URL

// Function to fetch data from a specific port
async function fetchDataFromPort(port) {
    try {
        const response = await fetch(`${serverUrl}:${port}`);
        const data = await response.json();
        console.log(`Data from port ${port}:`, data);
        return data;
    } catch (error) {
        console.error(`Error fetching data from port ${port}:`, error);
        return null;
    }
}

// Array to hold all the promises for fetching data
const fetchPromises = ports.map(port => fetchDataFromPort(port));

// Execute all requests concurrently
Promise.all(fetchPromises)
    .then(results => {
        console.log('All requests completed:', results);
        // Do something with the collected results here
    })
    .catch(error => console.error('Error fetching data:', error));