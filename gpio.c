
/*
 * Based on code from Linux kernel:
 * linux/arch/arm/mach-at91/gpio.c
 * Copyright (C) 2005 HP Labs
 *
 */



#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdint.h>
#include <stdbool.h>

#include "at91_pio.h"

#include "common.h"


struct at91_gpio {
	bool available;		/* Can the user modify this gpio? */
	void *regbase; 		/* Base of register bank (initialized at runtime) */
	uint32_t mask; 		/* Mask of gpio in register (initialized at runtime)*/
};

static struct at91_gpio gpio_table[] = {
		{false, NULL, 0},    /*PA00*/
		{false, NULL, 0},    /*PA01*/
		{false, NULL, 0},    /*PA02*/
		{false, NULL, 0},    /*PA03*/
		{false, NULL, 0},    /*PA04*/
		{false, NULL, 0},    /*PA05*/
		{false, NULL, 0},    /*PA06*/
		{false, NULL, 0},    /*PA07*/
		{false, NULL, 0},    /*PA08*/
		{false, NULL, 0},    /*PA09*/
		{false, NULL, 0},    /*PA10*/
		{false, NULL, 0},    /*PA11*/
		{false, NULL, 0},    /*PA12*/
		{false, NULL, 0},    /*PA13*/
		{false, NULL, 0},    /*PA14*/
		{false, NULL, 0},    /*PA15*/
		{false, NULL, 0},    /*PA16*/
		{false, NULL, 0},    /*PA17*/
		{false, NULL, 0},    /*PA18*/
		{false, NULL, 0},    /*PA19*/
		{false, NULL, 0},    /*PA20*/
		{false, NULL, 0},    /*PA21*/
		{false, NULL, 0},    /*PA22*/
		{false, NULL, 0},    /*PA23*/
		{false, NULL, 0},    /*PA24*/
		{false, NULL, 0},    /*PA25*/
		{false, NULL, 0},    /*PA26*/
		{false, NULL, 0},    /*PA27*/
		{false, NULL, 0},    /*PA28*/
		{false, NULL, 0},    /*PA29*/
		{false, NULL, 0},    /*PA30*/
		{false, NULL, 0},    /*PA31*/

		{false, NULL, 0},    /*PB00*/
		{false, NULL, 0},    /*PB01*/
		{false, NULL, 0},    /*PB02*/
		{false, NULL, 0},    /*PB03*/
		{false, NULL, 0},    /*PB04*/
		{false, NULL, 0},    /*PB05*/
		{false, NULL, 0},    /*PB06*/
		{false, NULL, 0},    /*PB07*/
		{false, NULL, 0},    /*PB08*/
		{false, NULL, 0},    /*PB09*/
		{false, NULL, 0},    /*PB10*/
		{false, NULL, 0},    /*PB11*/
		{false, NULL, 0},    /*PB12*/
		{false, NULL, 0},    /*PB13*/
		{false, NULL, 0},    /*PB14*/
		{false, NULL, 0},    /*PB15*/
		{false, NULL, 0},    /*PB16*/
		{false, NULL, 0},    /*PB17*/
		{false, NULL, 0},    /*PB18*/
		{false, NULL, 0},    /*PB19*/
		{false, NULL, 0},    /*PB20*/
		{false, NULL, 0},    /*PB21*/
		{false, NULL, 0},    /*PB22*/
		{false, NULL, 0},    /*PB23*/
		{false, NULL, 0},    /*PB24*/
		{false, NULL, 0},    /*PB25*/
		{false, NULL, 0},    /*PB26*/
		{false, NULL, 0},    /*PB27*/
		{false, NULL, 0},    /*PB28*/
		{false, NULL, 0},    /*PB29*/
		{false, NULL, 0},    /*PB30*/
		{false, NULL, 0},    /*PB31*/

		{false, NULL, 0},    /*PC00*/
		{false, NULL, 0},    /*PC01*/
		{false, NULL, 0},    /*PC02*/
		{false, NULL, 0},    /*PC03*/
		{false, NULL, 0},    /*PC04*/
		{false, NULL, 0},    /*PC05*/
		{false, NULL, 0},    /*PC06*/
		{false, NULL, 0},    /*PC07*/
		{false, NULL, 0},    /*PC08*/
		{false, NULL, 0},    /*PC09*/
		{false, NULL, 0},    /*PC10*/
		{false, NULL, 0},    /*PC11*/
		{false, NULL, 0},    /*PC12*/
		{false, NULL, 0},    /*PC13*/
		{false, NULL, 0},    /*PC14*/
		{false, NULL, 0},    /*PC15*/
		{false, NULL, 0},    /*PC16*/
		{false, NULL, 0},    /*PC17*/
		{false, NULL, 0},    /*PC18*/
		{false, NULL, 0},    /*PC19*/
		{false, NULL, 0},    /*PC20*/
		{false, NULL, 0},    /*PC21*/
		{false, NULL, 0},    /*PC22*/
		{false, NULL, 0},    /*PC23*/
		{false, NULL, 0},    /*PC24*/
		{false, NULL, 0},    /*PC25*/
		{false, NULL, 0},    /*PC26*/
		{false, NULL, 0},    /*PC27*/
		{false, NULL, 0},    /*PC28*/
		{false, NULL, 0},    /*PC29*/
		{false, NULL, 0},    /*PC30*/
		{false, NULL, 0},    /*PC31*/
};


static inline uint32_t pin_to_mask(uint32_t pin)
{
	return 1 << (pin % 32);
}

static inline struct at91_gpio *get_gpio(uint32_t pin)
{
	if(pin < ARRAY_SIZE(gpio_table)) {
		return &gpio_table[pin];
	}

	return NULL;
}


static int at91_get_gpio_reg(uint32_t pin, uint32_t reg_offset)
{
	struct at91_gpio *p = get_gpio(pin);
	uint32_t reg;

	if(p == NULL) {
		return -EINVAL;
	}

	reg = __raw_readl(p->regbase);
	return (reg & p->mask) != 0;
}

/*
 * assuming the pin is muxed as a gpio output, set its value.
 */
int at91_set_gpio_value(uint32_t pin, int value)
{
	struct at91_gpio *p = get_gpio(pin);
	if(p == NULL) {
		return -EINVAL;
	}

	__raw_writel(p->mask, p->regbase + (value ? PIO_SODR : PIO_CODR));
	return 0;
}


/*
 * read the pin's value (works even if it's not muxed as a gpio).
 */
int at91_get_gpio_value(uint32_t pin)
{
	return at91_get_gpio_reg(pin, PIO_PDSR);
}

/*
 * mux the pin to the "GPIO" peripheral role.
 */
int at91_set_GPIO_periph(uint32_t pin, int use_pullup)
{
	struct at91_gpio *p = get_gpio(pin);
	if(p == NULL) {
		return -EINVAL;
	}

	__raw_writel(p->mask, p->regbase + PIO_IDR);
	__raw_writel(p->mask, p->regbase + (use_pullup ? PIO_PUER : PIO_PUDR));
	__raw_writel(p->mask, p->regbase + PIO_PER);
	return 0;
}

/*
 * mux the pin to the "A" internal peripheral role.
 */
int at91_set_A_periph(uint32_t pin, int use_pullup)
{
	struct at91_gpio *p = get_gpio(pin);
	if(p == NULL) {
		return -EINVAL;
	}

	__raw_writel(p->mask, p->regbase + PIO_IDR);
	__raw_writel(p->mask, p->regbase + (use_pullup ? PIO_PUER : PIO_PUDR));
	__raw_writel(p->mask, p->regbase + PIO_ASR);
	__raw_writel(p->mask, p->regbase + PIO_PDR);
	return 0;
}

/*
 * mux the pin to the "B" internal peripheral role.
 */
int at91_set_B_periph(uint32_t pin, int use_pullup)
{
	struct at91_gpio *p = get_gpio(pin);
	if(p == NULL) {
		return -EINVAL;
	}

	__raw_writel(p->mask, p->regbase + PIO_IDR);
	__raw_writel(p->mask, p->regbase + (use_pullup ? PIO_PUER : PIO_PUDR));
	__raw_writel(p->mask, p->regbase + PIO_BSR);
	__raw_writel(p->mask, p->regbase + PIO_PDR);
	return 0;
}


/*
 * mux the pin to the gpio controller (instead of "A" or "B" peripheral), and
 * configure it for an input.
 */
int  at91_set_gpio_input(uint32_t pin, int use_pullup)
{
	struct at91_gpio *p = get_gpio(pin);
	if(p == NULL) {
		return -EINVAL;
	}

	__raw_writel(p->mask, p->regbase + PIO_IDR);
	__raw_writel(p->mask, p->regbase + (use_pullup ? PIO_PUER : PIO_PUDR));
	__raw_writel(p->mask, p->regbase + PIO_ODR);
	__raw_writel(p->mask, p->regbase + PIO_PER);
	return 0;
}


/*
 * mux the pin to the gpio controller (instead of "A" or "B" peripheral),
 * and configure it for an output.
 */
int  at91_set_gpio_output(uint32_t pin, int value)
{
	struct at91_gpio *p = get_gpio(pin);
	if(p == NULL) {
		return -EINVAL;
	}

	__raw_writel(p->mask, p->regbase + PIO_IDR);
	__raw_writel(p->mask, p->regbase + PIO_PUDR);
	__raw_writel(p->mask, p->regbase + (value ? PIO_SODR : PIO_CODR));
	__raw_writel(p->mask, p->regbase + PIO_OER);
	__raw_writel(p->mask, p->regbase + PIO_PER);
	return 0;
}



int at91_is_gpio_direction_output(uint32_t pin)
{
	return at91_get_gpio_reg(pin, PIO_OSR);
}


int at91_is_peripheral_A(uint32_t pin)
{
	return at91_get_gpio_reg(pin, PIO_ABSR);
}

/*
 * enable/disable the glitch filter; mostly used with IRQ handling.
 */
int  at91_set_deglitch(uint32_t pin, int is_on)
{
	struct at91_gpio *p = get_gpio(pin);
	if(p == NULL) {
		return -EINVAL;
	}

	__raw_writel(p->mask, p->regbase + (is_on ? PIO_IFER : PIO_IFDR));
	return 0;
}

/*
 * enable/disable the multi-driver; This is only valid for output and
 * allows the output pin to run as an open collector output.
 */
int  at91_set_multi_drive(uint32_t pin, int is_on)
{
	struct at91_gpio *p = get_gpio(pin);
	if(p == NULL) {
		return -EINVAL;
	}

	__raw_writel(p->mask, p->regbase + (is_on ? PIO_MDER : PIO_MDDR));
	return 0;
}



static void at91_dbg_show()
{
	int i;

	printf("Debug GPIO states:\n");

	for(i=0; i<ARRAY_SIZE(gpio_table); i++) {
		printf("GPIO %03d: [%s], %s, %s\n", i,
				at91_is_gpio_direction_output(i) ? "out": " in",
				at91_is_peripheral_A(i) ? "A": "B",
				at91_get_gpio_value(i) ? "  set": "clear");
	}
}


int init_gpio(void *gpio_regs)
{
	int i;
	for(i=0; i<ARRAY_SIZE(gpio_table); i++)	{
		gpio_table[i].regbase = (char*)gpio_regs + (i / 32)*0x20;
		gpio_table[i].mask = pin_to_mask(i);
	}

	at91_dbg_show();

	return 0;
}

int main(int argc, char ** argv)
{
	int fd = open("/dev/uio0", O_RDWR);
	if(fd < 0)
	{
		ERROR("Cannot open /dev/uio0");
		return -1;
	}

	void *gpio_regs = mmap(NULL, 0x600, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 2*getpagesize());
	init_gpio(gpio_regs);


	return 0;
}

