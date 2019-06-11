To run the code-:

cd Server
gcc server.c
./a.out
# Open new terminal
cd Client
gcc client.c
./a.out 

Following Error Handling has been taken care of-:

1.Socket creation Error
2.Connection Error
3.File opening Error
4.If file is avaiable on server or not

*IMPLEMENTED FOR MULTIPLE CLIENTS*

Client can use 'listall' to view all the files in the server directory.
client can enter the filename, the server will try to search your file in
Server folder. If the file is found in the path it wil be downloaded, otherwise it will display an error
message, showing that such file doesn't exist on server.
