#include <iostream>
#include <vector>
#include <string>

std::string findMatchingLocation(const std::string& uri, const std::vector<std::string>& locations) {
    std::string match;
    size_t longestMatchLength = 0;

    for (std::vector<std::string>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
        const std::string& location = *it;
        // Check if the URI starts with the location prefix
        printf("%s\n",it->c_str());
        // if (uri.find(location) == 0 && location.length() > longestMatchLength) {
        //     match = location;
        //     longestMatchLength = location.length();
        // }
    }

    return match;
}

int main() {
    // Sample URI
    std::string uri = "/test_test";

    // Sample list of location blocks
    std::vector<std::string> locations;
    locations.push_back("/");
    locations.push_back("/test_test");
    locations.push_back("/test_test2");

    // Find the matching location
    std::string matchedLocation = findMatchingLocation(uri, locations);

    if (!matchedLocation.empty()) {
        std::cout << "Matched location: " << matchedLocation << std::endl;
    } else {
        std::cout << "No matching location found." << std::endl;
    }

    return 0;
}