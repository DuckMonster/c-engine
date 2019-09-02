#include "IpAddress.h"
#include <stdio.h>

Ip_Address ip_make(u32 addr, u16 port)
{
	Ip_Address out_addr;
	out_addr.addr = addr;
	out_addr.port = port;

	return out_addr;
}

Ip_Address ip_make(u8 net, u8 host, u8 logical_host, u8 impno, u16 port)
{
	Ip_Address out_addr;
	out_addr.addr_b[0] = net;
	out_addr.addr_b[1] = host;
	out_addr.addr_b[2] = logical_host;
	out_addr.addr_b[3] = impno;
	out_addr.port = port;

	return out_addr;
}

Ip_Address ip_parse(const char* ip_str)
{
	Ip_Address out_addr;
	out_addr.port = PORT_ANY;

	sscanf(ip_str, "%hhu.%hhu.%hhu.%hhu:%hu",
		&out_addr.addr_b[0], &out_addr.addr_b[1], &out_addr.addr_b[2], &out_addr.addr_b[3],
		&out_addr.port);

	return out_addr;
}

const char* ip_str(const Ip_Address& addr)
{
	static char str_buf[50];
	sprintf(str_buf, "%hhu.%hhu.%hhu.%hhu:%hu", 
		addr.addr_b[0], addr.addr_b[1], addr.addr_b[2], addr.addr_b[3],
		addr.port);

	return str_buf;
}