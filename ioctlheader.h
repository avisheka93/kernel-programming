#ifndef IOCTLHEADER.H
#define IOCTLHEADER.H
#include <linux/ioctl.h>

typedef struct
{
	int status, dignity, ego;
} query_arg_t;

#define GETVAR _IOR('q', 1, query_arg_t *)
#define CLEARVAR _IO('q', 2, query_arg_t *)
#define SETVAR _IOW('q', 3, query_arg_t *)

#endif
