let server = `server 
{
    listen 127.0.0.1:PORT;
    server_names exemple;
    client_max_body_size 1;
    location /
    {
        root public/;
        index index.html;
        autoindex off;
    }
    autoindex on;
}
`

let i = 1024

let result = "";

while (i < 8830) {
    let server_modefied = server.replace("PORT", i);
    result += server_modefied;
    i++;
}

console.log(result)