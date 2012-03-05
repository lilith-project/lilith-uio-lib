/*
 * common.h
 *
 *  Created on: Mar 5, 2012
 *      Author: julien
 */

#ifndef COMMON_H_
#define COMMON_H_

#define ERROR printf
#define EINVAL 1

#define __raw_writeb(v,a)       (*(volatile unsigned char *)(a) = (v))
#define __raw_writew(v,a)       (*(volatile unsigned short *)(a) = (v))
#define __raw_writel(v,a)       (*(volatile unsigned int *)(a) = (v))

#define __raw_readb(a)          (*(volatile unsigned char *)(a))
#define __raw_readw(a)          (*(volatile unsigned short *)(a))
#define __raw_readl(a)          (*(volatile unsigned int *)(a))

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#endif /* COMMON_H_ */
