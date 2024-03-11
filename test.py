import requests

url = 'http://127.0.0.1:8080'
headers = {'User-Agent': 'Your User Agent'}

# Make the request without the Host header
response = requests.get(url, headers=headers)

print(response.text)