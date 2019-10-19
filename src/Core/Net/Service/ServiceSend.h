#pragma once

struct Connection;

void net_service_flush_out(Connection* connection);
void net_service_send(void*);