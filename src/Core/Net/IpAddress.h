#pragma once

#define IP_ANY 0
#define PORT_ANY 0

struct Ip_Address
{
	union
	{
		u8 addr_b[4];
		u16 addr_w[2];
		u32 addr;
	};
	u16 port;
};

inline bool operator==(const Ip_Address& a, const Ip_Address& b)
{
	return a.addr == b.addr && a.port == b.port;
}

inline bool operator!=(const Ip_Address& a, const Ip_Address& b)
{
	return a.addr != b.addr || a.port != b.port;
}

Ip_Address ip_make(u32 addr, u16 port);
Ip_Address ip_make(u8 net, u8 host, u8 logical_host, u8 impno, u16 port);
Ip_Address ip_parse(const char* ip_str);
const char* ip_str(const Ip_Address& addr);