#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

#define PAGE_SIZE 4096
#define BUF_SIZE 512
#define MAP_SIZE PAGE_SIZE * 100
int main (int argc, char* argv[])
{
	char buf[BUF_SIZE];
	int N_master, N;  // number of files
	int i, dev_fd, file_fd;// the fd for the device and the fd for the input file
	size_t ret, file_size = 0, data_size = -1, offset = 0;
	char file_name[50];
	char method[20];
	char ip[20];
	struct timeval start;
	struct timeval end;
	double trans_time; //calulate the time between the device is opened and it is closed
	char *kernel_address, *file_address;

	if (argc < 5) {
		fprintf(stderr, "usage: %s <N> <filenames ...> <method> <ip>\n", argv[0]);
		return -1;
	}
	N = atoi(argv[1]);
	if (N == 0) {
		fprintf(stderr, "The following arguments are required: int N\n");
		return -1;
	}
	if (argc != N + 4) {
		fprintf(stderr, "usage: %s <N> <filenames ...> <method> <ip>\n", argv[0]);
		return -1;
	}
	strcpy(method, argv[N+2]);
	strcpy(ip, argv[N+3]);

	if( (dev_fd = open("/dev/slave_device", O_RDWR)) < 0)//should be O_RDWR for PROT_WRITE when mmap()
	{
		perror("failed to open /dev/slave_device\n");
		return 1;
	}

	ioctl(dev_fd, 0x12345677, ip);
	read(dev_fd, &N_master, 4);
	if (N > N_master)
	{
		fprintf(stderr, "number of files remains in master is less than %d\n", N);
		N = N_master;
	}
	write(dev_fd, &N, 4);
	ioctl(dev_fd, 0x12345679);
	fprintf(stdout, "get numbers of files from master = %d\n", N);


	for(int i = 0; i < N; i++)
	{
		offset = 0;
		file_size = 0;
		strcpy(file_name, argv[i+2]);

		gettimeofday(&start ,NULL);
		if( (file_fd = open (file_name, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) < 0)
		{
			perror("failed to open input file\n");
			return 1;
		}

		if(ioctl(dev_fd, 0x12345677, ip) == -1)	//0x12345677 : connect to master in the device
		{
			perror("ioclt create slave socket error\n");
			return 1;
		}

		switch(method[0])
		{
			case 'f'://fcntl : read()/write()
				do
				{
					ret = read(dev_fd, buf, sizeof(buf)); // read from the the device
					write(file_fd, buf, ret); //write to the input file
					file_size += ret;
				}while(ret > 0);
				break;
			case 'm':
				kernel_address = mmap(NULL, MAP_SIZE, PROT_READ, MAP_SHARED, dev_fd, 0);
				while (1) {
					ret = ioctl(dev_fd, 0x12345678);
					if (ret == 0) {
						file_size = offset;
						break;
					}
					posix_fallocate(file_fd, offset, ret);
					file_address = mmap(NULL, ret, PROT_WRITE, MAP_SHARED, file_fd, offset);
					memcpy(file_address, kernel_address, ret);
					offset += ret;
				}
				break;
		}

		if(ioctl(dev_fd, 0x12345679) == -1)// end receiving data, close the connection
		{
			perror("ioclt client exits error\n");
			return 1;
		}
		gettimeofday(&end, NULL);
		trans_time = (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)*0.0001;
		printf("Transmission time: %lf ms, File size: %zd bytes\n", trans_time, file_size);

		close(file_fd);
	}
	ioctl(dev_fd, 0x11111111);
	close(dev_fd);
	return 0;
}


