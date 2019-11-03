/*
 *  hello-1.c - The simplest kernel module.
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */


unsigned bcd2bin(unsigned char val)
{
	return (val & 0x0f) + (val >> 4) * 10;
}
//EXPORT_SYMBOL(bcd2bin);


int init_module(void)
{
	int hora, minuto, sec;
	printk(KERN_INFO "Iniciando device driver do RTC\n");

	//inicio de regiao critica
	outb(4, 0x70);
        hora = inb(0x71);

	outb(2, 0x70);
        minuto = inb(0x71);

	outb(0, 0x70);
        sec = inb(0x71);
	//fim de regiao critica


	printk(KERN_INFO "Leitura do RTC Hora = %d\n", bcd2bin(hora));
	printk(KERN_INFO "Leitura do RTC Min = %d\n", bcd2bin(minuto));
	printk(KERN_INFO "Leitura do RTC Sec = %d\n", bcd2bin(sec));


	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Nosso device driver terminou.\n");
}
