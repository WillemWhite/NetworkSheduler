# NetworkSheduler
Network shedular. Receiving data from port and sending to the appropriate servers for even load. All settings must be written in the config file

## USAGE
**First** you need to fill in the configuration file:  
```
RECEIVING_PORT <port like 8888>
SENDING_ADDRESS <address like 127.0.0.1:8888>  
...  
... (then you can enter as many addresses as you like)  
...  
NUM_OF_DATAGRAMS_PER_SEC <maximum number of datagrams arriving at the receive port like 100>
```  
**Then** you need to open the scheduler itself with the command:  
```./NetworkSheduler <config filename>``` 

## TESTING
To test the program, client and server applications were also written to which datagrams are sent through the scheduler.  
Commands:  
```./client <port of localhost to which data will be sent>```
```./server <port from which the local server will receive datagrams>```

## EXAMPLES
<image src="/images/testing_commands.png" alt="Testing commands">
<image src="/images/working_client.png" alt="Working client">
<image src="/images/working_server1.png" alt="Working server1">
<image src="/images/working_server2.png" alt="Working server2">
<image src="/images/working_server3.png" alt="Working server3">
<image src="/images/working_server3.png" alt="Working Network Sheduler">
