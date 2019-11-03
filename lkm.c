
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */


int init_module(void)
{
	int leitura;
	printk(KERN_INFO "Iniciando...\n");

	
	leitura = inb(0x64);
	
	while (1){
	  if ( (leitura & 0x02) == 0)
			break;
	  leitura = inb(0x64);
	}
	
	
	outb(0xED, 0x60);
  leitura = inb(0x64);
  while (1){
	  if ( (leitura & 0x02) == 0)
			break;
	  leitura = inb(0x64);
	}

  outb(0x02, 0x60); // Se essa linha for comentada e o módulo compilado e carregado, o teclado irá parar de funcionar e a tecla Enter ficará sendo precionada.
  leitura = inb(0x64);
	printk(KERN_INFO "Led do Num Lock aceso!\n");


	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Fim.\n");
}
