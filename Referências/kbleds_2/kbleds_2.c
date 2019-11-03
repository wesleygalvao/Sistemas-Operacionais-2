//-------------------------------------------------------------------
//	led.c
//
//	This module defines a character-mode device-driver for the
//	standard PC keyboard's three Light Emitting Diodes (LEDs).
//	By writing a value to the device-node '/dev/led', the LEDs
//	can be turned on or turned off (only the lowest three bits
//	of whatever value is written will have an effect):  
//
//		bit #0:	controls LED for 'Scroll-Lock' 
//		bit #1: controls LED for 'Num-Lock' 
//		bit #2: controls LED for 'Caps-Lock' 
//	
//	(This device-driver does NOT work with those keyboards built 
//	using a Universal Serial Bus (USB) interface.)
//		
//	NOTE: Developed and tested with Linux kernel version 2.4.18.
//
//	programmer: ALLAN CRUSE
//	written on: 30 JAN 2003
//-------------------------------------------------------------------

#define	__KERNEL__
#define	  MODULE  

#include <linux/module.h>	// for init_module() 
#include <asm/uaccess.h>	// for get_ds(), set_fs()
#include <asm/io.h>		// for inb(), outb()
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */

#define	SUCCESS	0
#define LED_CMD	0xED
#define KB_STAT 0x64
#define KB_DATA 0x60
#define INBUF_FULL 2
#define TIMEOUT 1000000


// global variables
static char modname[] = "led";
static char devname[] = "/dev/led";
static unsigned int led_major = 0;


// service-function prototypes
static ssize_t led_write( struct file *, const char *, size_t, loff_t * );
static ssize_t led_read( struct file *, char *, size_t, loff_t * );
	

static struct file_operations
led_fops =	{
		owner:		THIS_MODULE,
		read:		NULL, 	// not yet implemented 
		write:		led_write,
		};


// module initialization function
int init_module( void )
{
	mode_t	mode;
	dev_t	dev_id;
	
	printk( "<1>\nInstalling \'%s\' module\n", modname );

	// register this character driver with the kernel, and
	// let the kernel assign an unused major device-number
	led_major = register_chrdev( 0, modname, &led_fops );	
	if ( led_major < 0 ) return led_major;

	// delete previous device-node (in case it still exists)
	set_fs( get_ds() );	// let kernel make system-calls
	sys_unlink( devname );	// remove the named device-node

	// create new device-node (i.e., device "special" file) 
	mode = S_IFCHR;		// character-driver interface
	dev_id = MKDEV( led_major, 0 );	// major and minor IDs 
	sys_mknod( devname, mode, dev_id );	// create file

	// change the default file-permissions
	mode = 0666;		// everybody can read and write
	sys_chmod( devname, mode );	// modifies permissions
	
	return	SUCCESS;
}


// module cleanup function
void cleanup_module( void )
{
	printk( "<1>Removing \'%s\' module\n", modname );

	// unregister this character-driver, and cleanup '/dev'
	unregister_chrdev( led_major, modname );
	set_fs( get_ds() );	// let kernel make system-calls
	sys_unlink( devname );	// delete driver's device-node
}

MODULE_LICENSE("GPL");


// helper function - returns TRUE when controller is ready
static int kb_ready_for_input( void )
{	
	int	reps, status;
	
	reps = TIMEOUT;
	do	{	
		status = inb( KB_STAT );	
		}
	while (( status & INBUF_FULL )&&( --reps ));
	return	( reps != 0 );
}
	

// module 'write' function
static ssize_t
led_write( struct file *file, const char *buf, size_t count, loff_t *pos )
{
	int	led = buf[0] & 0x07;	// mask all but lowest three bits
	
	printk( "<1>  %s: write 0x%02X to LEDs ", modname, led );
	printk( "  count=%d  *pos=%08X \n", count, *pos );
	
	cli();				// enter critical section
	
	// wait until keyboard-controller's input buffer is empty
	if ( !kb_ready_for_input() ) return -EBUSY;

	// give 'LED_WRITE' command to the keyboard-controller 
	outb( LED_CMD, KB_DATA );	// send 'LED_write' command

	// wait until the controller has accepted this command
	if ( !kb_ready_for_input() ) return -EBUSY;
	
	// give LED indicator-byte to the keyboard-controller 
	outb( led, KB_DATA );		// give byte to KB controller

	// wait until the controller has accepted this data-byte
	if ( !kb_ready_for_input() ) return -EBUSY;

	sti();				// leave critical section

	return 	count;	// tell kernel that all the data was written
}
