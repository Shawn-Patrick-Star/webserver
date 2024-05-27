/* C declarations used in actions */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "parse.h"
#include "respend.h"
#include "log.h"

int main(int argc, char **argv)
{
	// Read from the file the sample
	if (argc < 2)
	{
		printf("Usage: %s <file_path>\n", argv[0]);
		return 0;
	}
	FILE *fp = fopen(argv[1], "r");
	if (fp == NULL) {
        fprintf(stderr, "Error opening file: %s\n", strerror(errno));
        return 1;
    }

	char buf[8192];
	int readRet = fread(buf, 1, 8192, fp);

	// Parse the buffer to the parse function. You will need to pass the socket fd and the buffer would need to
	// be read from that fd

	Request *request = parse(buf, readRet, readRet);
	if (request == NULL)
	{
		printf("Failed to parse the request\n");
		return 0;
	}
	else
	{
		// Just printing everything
		printf("%s", buf);		
		respend(request, buf);
		printf("%s", buf);
		// printf("Http Method %s\n", request->http_method);
		// printf("Http Version %s\n", request->http_version);
		// printf("Http Uri %s\n", request->http_uri);
		// for (index = 0; index < request->header_count; index++)
		// {
		//   printf("Request Header\n");
		//   printf("Header name %s Header Value %s\n", request->headers[index].header_name, request->headers[index].header_value);
		// }
	}

	free(request->headers);
	free(request);
	return 0;
}
