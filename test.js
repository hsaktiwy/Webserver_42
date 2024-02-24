// Function to make a single asynchronous request
async function makeRequest(url) {
    try {
      const response = await fetch(url);
      console.log(`Request to ${url} successful. Response data:`, response);
    } catch (error) {
      console.error(`Error making request to ${url}:`, error);
      throw error;
    }
  }
  
  // Array of URLs to make requests to
  const urls = [
    'http://localhost:8080/index.html'
    // Add more URLs as needed
  ]
  
  // Function to continuously make requests in an infinite loop
  async function infiniteRequests() {
    while (true) {
      // Make requests for each URL
      const requests = urls.map(url => makeRequest(url));
  
      try {
        // Wait for all requests to complete
        await Promise.all(requests);
      } catch (error) {
        console.error('At least one request failed:', error);
      }
  
      // Add a delay between iterations (adjust the delay time as needed)
      await new Promise(resolve => setTimeout(resolve, 1000));
    }
  }
  
  // Start the infinite requests loop
  infiniteRequests();
  