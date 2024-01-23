docker build -t nginx00 .
docker run -d -p 80:80 --name nginx nginx00