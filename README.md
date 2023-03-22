# NetworkSheduler
Network shedular. Receiving data from port and sending to the appropriate servers for even load.  
All settings must be written in the config file.  
For the sake of experiment, I decided to distribute datagrams according to a relatively **uniform distribution law**  
through the rand() function.  
(По-русски: Сетевой планировщик. Данные настроек сохраняются в конфигурационный файл,  
имя и расширение которого может быть любым. Единственное, его имя указывается при вызове самой программы.  
Ради эксперимента решил распределять датаграммы по равномерному закону распределения через соответствующую функцию rand())

## USAGE
**First** you need to fill the configuration file:  
(По-русски: здесь представлена структура конфигурационного файла)
```
RECEIVING_PORT <port like 8888>
SENDING_ADDRESS <address like 127.0.0.1:8888>  
...  
... (then you can enter as many addresses as you like)  
...  
NUM_OF_DATAGRAMS_PER_SEC <maximum number of datagrams arriving at the receive port like 100>
```  
**Then** you need to open the scheduler itself with the command:  
(По-русски: вызов программы)
```./NetworkSheduler <config filename>``` 

## TESTING
To test the program, client and server applications were also written to which datagrams are sent through the scheduler.  
(По-русски: для теста сделал 2 приложения: клиент и сервер. Клиент каждую секунду шлет датаграммы на заданный порт,  
а сервер, тем временем, с определенного порта их и принимает. Все происходит на локальном сервере.  
В итоге, для тестирования планировщика, нужно просто указать соответствующие порты)  
Commands:  
```./client <port of localhost to which data will be sent>```  
```./server <port from which the local server will receive datagrams>```

## EXAMPLES
<image src="/images/testing_commands.png" alt="Testing commands">
<image src="/images/working_client.png" alt="Working client">
<image src="/images/working_server1.png" alt="Working server1">
<image src="/images/working_server2.png" alt="Working server2">
<image src="/images/working_server3.png" alt="Working server3">
<image src="/images/working_sheduler.png" alt="Working Network Sheduler">
