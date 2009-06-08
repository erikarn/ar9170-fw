#ifndef __COMPILER
#define __COMPILER

#define __noinline	__attribute__((noinline))
#define __section(s)	__attribute__((section("." # s)))
#define __packed	__attribute__((packed))

#endif /* __COMPILER */
