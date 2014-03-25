/*

   A simple tool to feed an HPX-84 plotter data
   Copyright (C) 2014 Gnoxter

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/parport.h>
#include <linux/ppdev.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUFSIZE 512

void usage() {

	printf("./hpx84-connector <file.hpgl>\n");
	printf("\t -d device : set different port\n");
	printf("\t reads from stdin if no file given\n");
}


int main(int argc, char *argv[]) {
	int	portFD, fd, i, readFromSTDIN = 2;
	FILE	*file;
	int 	mode = IEEE1284_MODE_COMPAT;
	char	*ppdev = "/dev/parport0";
	char	buf[BUFSIZE];
	size_t nread;


	for(i= 0; i < argc; i++) {
		if(argv[i][0] != '-')
			continue;

		//set non default ppdev port
		if (argv[i][1] == 'd' && argc > i+1) {
				ppdev = argv[i+1];
				readFromSTDIN += 2;
				continue;
		}

		usage();
		exit(EXIT_FAILURE);
	}


	/*
		Setup Parallel Port
	*/

	portFD = open(ppdev, O_RDWR);
	if(ioctl(portFD, PPCLAIM)) {
		perror("ERROR PPCLAIM");
		exit(1);
	}

	if(ioctl(portFD, PPNEGOT, &mode)) {
		perror("ERROR PPSETMODE");
		exit(1);
	}

	/*
		Setup File
	*/

	if (argc < readFromSTDIN) {
		file = stdin;
	} else {
		file = fopen(argv[argc-1], "r");
		if(file == NULL) {
			perror("ERROR opening FILE");
			exit(1);
		}
	}

	//write to plotter
	while((nread = fread(buf, 1, BUFSIZE, file)) > 0) {
		write(portFD, buf, nread);
	}
	return 0;
};
