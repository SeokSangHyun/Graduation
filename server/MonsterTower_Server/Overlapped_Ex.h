#pragma once

struct EXOver {
	WSAOVERLAPPED wsaover;
	int command;
	UINT target;
	WSABUF wsabuf;
	char io_buf[MAX_BUFF_SIZE];
};