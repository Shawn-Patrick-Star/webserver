#include "parse.h"
#include <stdbool.h>

bool strIsEqual(char *str1, const char *str2){
	int i = 0;
	while(str1[i] != '\0' && str2[i] != '\0'){
		if(str1[i] != str2[i]){
			return false;
		}
		i++;
	}
	if(str1[i] == '\0' && str2[i] == '\0'){
		return true;
	}
	return false;
}
void isValidMethod(char *method, int *status_code){
	char ch = method[0];
	int i = 0;
	char buf[10];
	while(ch != '\0'){
		buf[i++] = ch;
		ch = method[i];
	}
	buf[i] = '\0';
	if(strIsEqual(buf, "GET") || strIsEqual(buf, "POST") || strIsEqual(buf, "HEAD")){
		*status_code = NORMAL;
	}else{
		*status_code = HTTP_501;
	}

}


/**
* Given a char buffer returns the parsed request headers
*/
Request * parse(const char *buffer, int size, int socketFd, int *status_code) {
  //Differant states in the state machine
	enum {
		STATE_START = 0, STATE_CR, STATE_CRLF, STATE_CRLFCR, STATE_CRLFCRLF
	};

	int i = 0, state;
	size_t offset = 0;
	char ch;
	char buf[8192];
	memset(buf, 0, 8192);

	state = STATE_START;
	while (state != STATE_CRLFCRLF) {
		char expected = 0;

		if (i == size)
			break;

		ch = buffer[i++];
		buf[offset++] = ch;

		switch (state) {
		case STATE_START:
		case STATE_CRLF:
			expected = '\r';
			break;
		case STATE_CR:
		case STATE_CRLFCR:
			expected = '\n';
			break;
		default:
			state = STATE_START;
			continue;
		}

		if (ch == expected)
			state++;
		else
			state = STATE_START;

	}


    //Valid End State
	if (state == STATE_CRLFCRLF) {
		Request *request = (Request *) malloc(sizeof(Request));
        request->header_count = 0;
        //TODO You will need to handle resizing this in parser.y
        request->headers = (Request_header *) malloc(sizeof(Request_header)*10);
		set_parsing_options(buf, i, request);

		if (yyparse() == SUCCESS) {
			isValidMethod(request->http_method, status_code);
			return request;
		}else{
			// 向下执行
		}
	}
    //TODO Handle Malformed Requests
	*status_code = HTTP_400;
    // printf("Parsing Failed\n");
	return NULL;
}
