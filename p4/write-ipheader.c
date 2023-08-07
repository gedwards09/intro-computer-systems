#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define HdrLen 20

int main(int argc, char *argv[])
{
	if (argc !=2)
	{
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		exit(1);
	}
	FILE *fp = fopen(argv[1], "wb");
	if (!fp)
	{
		fprintf(stderr, "ERROR: unable to open output file.\n");
		exit(1);
	}
	char buff[HdrLen] = {0x45, 0x00, 0x00, 0x73,
					0x00, 0x00, 0x40, 0x00,
					0x40, 0x11, 0xb8, 0x61,
					0xc0, 0xa8, 0x00, 0x01,
					0xc0, 0xa8, 0x00, 0xc7};
	fwrite(buff, 1, HdrLen, fp);
	return 0;
} 
