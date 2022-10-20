#ifndef EXTCLIB_SysSocketFramework_H_
#define EXTCLIB_SysSocketFramework_H_

#include <stddef.h>

extern int ListenNet(char *address);
extern int AcceptNet(int listener);

extern int ConnectNet(char *address);
extern int CloseNet(int conn);

extern int SendNet(int conn, char *buffer, size_t size);
extern int RecvNet(int conn, char *buffer, size_t size);

#endif /* EXTCLIB_SysSocketFramework_H_ */
