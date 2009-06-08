#ifndef __COMPILER
#define __COMPILER

#define __noinline	__attribute__((noinline))
#define __section(s)	__attribute__((section("." # s)))

#endif /* __COMPILER */
