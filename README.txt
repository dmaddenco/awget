CS457 - awget
Andy Thai 830276315, Collin Kinchen 830257569, David Madden 830338902

The following awget and ss application was written, compiled, and tested by Andy Thai, Collin Kinchen, and David Madden.
The purpose of this application is to send a file request through multiple hosts to retrieve the file
making it difficult to trace the request.

Awget:
To compile Awget: ./awget <URL> -c chaingang.txt
The [-c] and [chaingang.txt] are optional arguments. Once compiled the application will
attempt to connect to the given IP address and port numbers specified in the chaingang.txt file
On return, the file retrieved will be saved in the current directory.

SS:
To compile SS: ./ss -p portNumber
Once compiled, the ss application will listen for requests for chaingang file and check
if the list is empty. If the list is not empty it will forward the request to a random 
IP address. If the list is empty, the final ss will call -wget to retrive the file from
the <URL> and forward it back to the requestor.
