
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */

//Para explicações mais detalhadas sobre as portas usadas para leitura (inb) e escrita (outb) de registradores ler o site www.brokenthorn.com/Resources/OSDev19.html
int init_module(void)
{
	int leitura;
	printk(KERN_INFO "Iniciando...\n");

	
	leitura = inb(0x64); //Para ler o registrador de status (registrador integrante do keyboard controller) do teclado é necessário ler da porta de E/S 0x64
	
	while (1){
	  if ( (leitura & 0x02) == 0) //O valor retornado da leitura é um valor de 8 bits que segue um formato específico
			break;	      //Quando o bit 1 do byte retornado é 1 (00000010), significa que o Input buffer está cheio e que não deve-se fazer a escrita ainda no keyboard controller. 
	  leitura = inb(0x64);	      //A operação de AND com 0x02 é igual a zero quando o bit 1 da leitura é zero, o que significa que o Input Buffer está vazio e que uma escrita pode ser feita.
	}
	//Vale ressaltar que os comandos enviados para o keyboard encoder (via porta 0x60) são enviados para o  keyboard controller primeiramente. Por isso deve-se verificar se o bit 1 do registrador de status. 
	// Ao escrever um byte de comando na porta 0x60, o keyboard controller transmite o valor diretamente para o keyboard encoder.
	outb(0xED, 0x60); //O comando 0xED enviado ao keyboard encoder via porta E/S 0x60 significaria que o valor dos LEDs serão definidos. O próximo byte gravado na porta 0x60 atualizaria os LEDs no teclado.
  leitura = inb(0x64); //Novamente o registrador de status é lido.
  while (1){
	  if ( (leitura & 0x02) == 0) // Ele é lido até que o bit 1 do byte retornado da leitura seja 0.
			break;
	  leitura = inb(0x64);
	}
  //Então é feita a escrita do byte gravado na porta 0x60 que atualizaria os LEDs no teclado segundo esta definição:
  //  Bit 0: Scroll lock LED (0: off 1:on)
  //Bit 1: Num lock LED (0: off 1:on)
  //  Bit 2: Caps lock LED (0: off 1:on)
  outb(0x02, 0x60); // Se essa linha for comentada e o módulo compilado e carregado, o teclado irá parar de funcionar e a tecla Enter ficará sendo precionada.
  
  leitura = inb(0x64);//Essa linha pode ser comentada, não alteraria nada no código de acender os LEDs.
	printk(KERN_INFO "Led do Num Lock aceso!\n");


	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Fim.\n");
}
