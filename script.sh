#!/bin/bash

BASH="$HOME/.bashrc"
if [ -f "$BASH" ];then
	echo "Adding new enviroment variable to $BASH"
        echo "export HELLO=\$HOSTNAME" >> "$BASH"
   	 echo "LOCAL=\$(whoami)" >> "$BASH"
	  gnome-terminal &
else
    echo ".bashrc not found in home directory."
fi


	
