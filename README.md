# Ping scan
This repository contains code for scanning all the online hosts on the subnet by sending them ICMP echo request. Hosts which respond with ICMP echo reply are considered online.

##### Note
This program assumes netmask to be **255.255.255.0** or **Class C**. If netmask of your interface is different from it this code would require changes for that.  
Also **eth0** interface is used. Interface name could be changed in *utilities.c*

#### Compiling
* Type following command to generate executable __hosts__  

	``` sh
	$ make
	```
	
	
	
#### Running
* To run __hosts__ you need to give root permissions to it. To run on *Ubuntu* type the following command 
	
	``` sh
	$ sudo ./hosts
	```  
	

#### Basic Explanation
* First the ip address of host on eth0 interface is found out
* From the above calculated ip address, ip address of hosts are calculated.
* For every host a new child process is created which sends ping 10 times to the host, 1 ping per second and stores the result in shared memory array.
* Parent waits for all the child processes to exit and then print the host ip addresses which responded with ICMP reply.

#### TODOS
* Add compatibility for generic IPV4 address.
* IPV6 compatibility.
