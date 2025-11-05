#include <stdio.h>
#include <string.h>
int main(int argc,char *argv[]){
	
	FILE *fp;
	if(argc !=2){
		perror("You must enter exactly 1 argument (0 or 1)");
	return 0;
	}
	
	
	if (strcmp(argv[1], "1") != 0 && strcmp(argv[1], "0") != 0) {
        	perror("WRONG ARGUMENT ENTER 0 OR 1");

		return 0;
         }

	 fp = fopen("/sys/class/leds/input3::capslock/brightness", "w");
          if (fp == NULL) {
         perror("Cannot open LED control file (run as root)");
         return 1;
        }

         fprintf(fp, "%s", argv[1]);

            
       fclose(fp);

	return 0;
}
