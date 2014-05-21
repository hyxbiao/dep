
#ifndef	__COMMON_H
#define	__COMMON_H

#include <stdio.h>

#define	LOG_FORMAT(level, format)	"["level"]["__FILE__":%d] "format"\n", __LINE__
#define	LOG_BASE(level, format, ...)	fprintf(stderr, LOG_FORMAT(level, format), ##__VA_ARGS__)

#define	LOG_D(format, ...)	LOG_BASE("DEBUG", format, ##__VA_ARGS__)
#define	LOG_W(format, ...)	LOG_BASE(" WARN", format, ##__VA_ARGS__)
#define	LOG_E(format, ...)	LOG_BASE("FATAL", format, ##__VA_ARGS__)


#endif	//__COMMON_H
