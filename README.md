# Unix-Linux-IPC
Unix/Linux IPC Implementation With Mailbox

This project implements Unix/Linux IPC with Mailbox in C language.

Compile codes:
gcc -pthread -o server -g server.c -lm
gcc -pthread -o client -g client.c -lm

Run codes:
Firstly you should run server:
./server

Then you can run clients as many as you want. You should give a argument to run code that specifies client_id:
./client <client_id>
Id can be either a number or a word

Details:
The server works as one process. Each time a client process runs, a new worker thread will be created in the server. This worker thread listens to the client. When a message comes, it will send the message to the given client(process).
When a client process is created, it sends a message with a special id which let the server know that a new process created. After that, it will start listening to it's id typed messages from mailbox. If you want to send a message to another client, type "w" and hit enter. After that, give a client_id that you want to send a message.
