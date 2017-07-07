#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>

int main()
{
	int fd, i;
	ssize_t ret;
	char my_buf[60];

	for (i = 0; i < 60; i++) my_buf[i] = '*';

	printf("PID: %d\n", getpid());
	fd = open("/dev/chrdev", O_RDWR);

	ret = write(fd, my_buf, 60);

	if (ret == -1)
	{
	perror("ERROR");
	exit(1);
	}

	lseek(fd, 20, SEEK_SET);
	ret = read(fd, my_buf, 10);

	if (ret == -1)
	{
	perror("ERROR");
	exit(2);
	}

	printf("Buf: %s\n", my_buf);

	memset(my_buf, 0, sizeof(my_buf));

	write(fd, "This is just testing", 15);

	lseek(fd, 0, SEEK_SET);

	read(fd, my_buf, 60);
	printf("Buf: %s\n", my_buf);

	close(fd);
 }
