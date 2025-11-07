# Embedded-Linux Day3


Capslock video

https://drive.google.com/file/d/1FpDMJazzOokUoSh6IxZoQCpS_RxDY0xg/view?usp=drivesdk

Q1- compgen -c > /tmp/all_user_commands.list 
Q2- vim .bashrc
    add date 
    source ~/.bashrc
Q3- compgen -c | wc -l
Q4- a) This command is incorrect in logic
    b) rm does not work with a pipe, rm: missing operand
    c)Counts how many outputs
Q5- sudo find / -name ".profile" 
Q6- ls -i / 
    ls -i /etc
    ls -i /etc/hosts
    if you want to the dir not fils make -id

Q7- sudo ln -s /etc/passwd /boot/passwd
Q8-sudo ln /etc/passwd /boot/passwd  
   cant Because different partions
   Because they have the same inode so cant be in defferent partions
Q9-echo \
   "\"this meansto continue the command on the next line
    This ">" is called the secondary prompt
    to change is Temporary  PS2=": "
    to make it permenat put it bashrc



