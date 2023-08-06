#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define HdrSz 20
#define MaxIPLength 16

int IsLittleEndian();
unsigned int GetLength(char *hdr);
unsigned int GetIP(char *hdr, int option);
char *FormatIP(unsigned int ipInt);
int IsChecksumValid(char* hdr);
int GetProtocol(char *hdr);
int GetVersion(char *hdr);
char GetFlags(char *hdr, int pos);

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	char *hdr;
	unsigned int n;

	hdr = malloc( sizeof(char) * HdrSz);
	FILE *fp = fopen(argv[1], "rb");
	if (!fp)
	{
		fprintf(stderr, "ERROR: Unable to read file.\n");
		exit(1);
	}
	n = (int) fread(hdr, sizeof(char), HdrSz, fp);
	fclose(fp);
	
	if (n != 20)
	{
		fprintf(stderr, "ERROR: Header not 20 bytes.\n");
		exit(1);
	}

	/* cleanup */
	printf("read %u bytes\n", n);
	if (IsLittleEndian())
	{
		printf("Machine is little endian\n");
	}
	else
	{
		printf("Machine is big endian\n");
		exit(1);
	}
	unsigned int len = GetLength(hdr);
	printf("total length: %u bytes (0x%x)\n", len, len);

	if (IsChecksumValid(hdr))
	{
		printf("Checksum is valid\n");
	}
	else
	{
		printf("Checksum is not valid\n");
	}

	printf("protocol: 0x%x\n", GetProtocol(hdr));
	printf("version: %u\n", GetVersion(hdr));
	printf("flag: 0x%x%x%x\n", GetFlags(hdr, 0), 
		GetFlags(hdr, 1), GetFlags(hdr,2));

	// get source IP
	unsigned int sip = GetIP(hdr, 0);
	printf("source ip: %u (0x%x)\n", sip, sip);
	char *sipFormatted = FormatIP(sip);
	printf("formatted: %s\n", sipFormatted);
	free(sipFormatted);

	// get dest IP
	unsigned int dip = GetIP(hdr, 1);
	printf("destination ip: %u (0x%x)\n", dip, dip);
	char *dipFormatted = FormatIP(dip);
	printf("formatted: %s\n", dipFormatted);
	free(dipFormatted);

	free(hdr);
	return 0;
}

int IsLittleEndian()
{
	int n = 0x41;
	return 'A' == *(char *)&n;
}

unsigned int GetLength(char *hdr)
{
	return (*(hdr+2) << 8) | *(hdr+3);
}

unsigned int GetIP(char *hdr, int option)
{
	if (option != 0 && option != 1)
	{
		return -1;
	}

	unsigned char *pc = (unsigned char *) (hdr + (3 + option) * sizeof(int));
	unsigned int output = 0;
	for (int i=0; i < sizeof(int); i++)
	{
		output <<= 8;
		output |= pc[i];
	}
	return output;
}

char *FormatIP(unsigned int ipInt)
{
	char *ps = malloc(sizeof(char) * MaxIPLength);
	int ipSegments = 4;
	unsigned int nums[ipSegments];
	for (int i=0; i < ipSegments; i++)
	{
		nums[i] = ((ipInt<<(8*i))&(~0<<24))>>24;
	}
	sprintf(ps, "%d.%d.%d.%d", nums[0], nums[1], nums[2], nums[3]);
	return 	ps;
}

int IsChecksumValid(char *hdr)
{
	unsigned int chksum = 0;
	unsigned char c1, c2;
	for(int i=0; i<HdrSz; i += 2)
	{
		c1 = (unsigned char) *(hdr+i);
		c2 = (unsigned char) *(hdr+i+1);
		chksum += (c1 << 8 | c2);
	}
	while (chksum & ~0xFFFF)
	{
		chksum = (chksum & 0xFFFF) + (chksum >> 16);
	}

	return chksum == 0 || chksum == 0xFFFF;
}

int GetProtocol(char *hdr)
{
	unsigned int *pn = (unsigned int *) hdr;
	pn += 2;
	return (int) (((*pn)&(~0<<8))>>8) & 0xFF;
}

int GetVersion(char *hdr)
{
	return (int) (((*hdr) & 0xF0) >> 4);
}

char GetFlags(char *hdr, int pos)
{
	if (pos > 3 || pos < 0)
	{
		fprintf(stderr, "ERROR: Invalid input\n");
		exit(1);
	}
	
	unsigned int *pn = (unsigned int *) hdr;
	pn++;
	printf("line 2: %u (0x%x)\n", *pn, *pn);
	return (char) (*pn>>(24-pos-1)) & 0x01; 
}
