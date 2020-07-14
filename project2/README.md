# OS 2020 Project2
This is the source code of operating system project2.

``./master_device`` : the device moudule for master server  
``./slave_device``  : the device moudule for slave client  
``./ksocket``: the device moudule including the funtions used for kernel socket  
``./user_program`` : the user program "master" and "slave"  
  
To use it, please:
```
    $ make && sudo make install
    $ cd user_program
    $ ./master <N> <file_in1> <file_in2> ... <file_inN> <method>
    $ ./slave <n> <file_out1> <file_out2> ... <file_outn> <method> <ip>
    
    # Note that: 
    #    1. method = mmap or fcntl
    #    2. slave <n> must be less or equal to master <N>        
```
