#ifndef PRACTICAL_H
#define PRACTICAL_H

#define BUFSIZE 256

typedef unsigned long in_addr_t;
typedef unsigned short in_port_t;

void DieWithUserMessage(const char *msg, const char *detail);
void DieWithSystemMessage(const char *msg);

#endif
