#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/timer.h>
#include <linux/tty.h>
#include <linux/kd.h>
#include <linux/vt.h>
#include <linux/console_struct.h>

#define MAX_DEV 1

static int mychardev_open(struct inode *inode, struct file *file);
static int mychardev_release(struct inode *inode, struct file *file);
static long mychardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static ssize_t mychardev_read(struct file *file, char __user *buf, size_t count, loff_t *offset);
static ssize_t mychardev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset);

static const struct file_operations mychardev_fops = {
    .owner      = THIS_MODULE,
    .open       = mychardev_open,
    .release    = mychardev_release,
    .unlocked_ioctl = mychardev_ioctl,
    .read       = mychardev_read,
    .write       = mychardev_write
};

struct mychar_device_data {
    struct cdev cdev;
};

static int dev_major = 0;
static struct class *mychardev_class = NULL;
static struct mychar_device_data mychardev_data[MAX_DEV];
int g_time_interval = 10000;
struct timer_list g_timer;
int countador = 0;
int leitura;
extern int fg_console;
struct tty_driver *driver;

static int mychardev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

void TimerHandler(struct timer_list *t)
{
  if ((countador % 2) == 0){
    driver = vc_cons[fg_console].d->port.tty->driver;
    ((driver->ops)->ioctl) (vc_cons[fg_console].d->port.tty, KDSETLED, 0x02);
	  printk(KERN_INFO "Led do Num Lock aceso!\n");
	}
	else{
    driver = vc_cons[fg_console].d->port.tty->driver;
    ((driver->ops)->ioctl) (vc_cons[fg_console].d->port.tty, KDSETLED, 0x00);
	  printk(KERN_INFO "Led do Num Lock apagado!\n");
	}
	countador++;
	
  mod_timer( &g_timer, jiffies + msecs_to_jiffies(g_time_interval));

  printk(KERN_INFO "Timer Handler chamado.\n");
}

static int __init mychardev_init(void)
{
    int err, i;
    dev_t dev;

    err = alloc_chrdev_region(&dev, 0, MAX_DEV, "grupo2");

    dev_major = 245;

    mychardev_class = class_create(THIS_MODULE, "grupo2");
    mychardev_class->dev_uevent = mychardev_uevent;

    for (i = 0; i < MAX_DEV; i++) {
        cdev_init(&mychardev_data[i].cdev, &mychardev_fops);
        mychardev_data[i].cdev.owner = THIS_MODULE;

        cdev_add(&mychardev_data[i].cdev, MKDEV(dev_major, i), 1);

        device_create(mychardev_class, NULL, MKDEV(dev_major, i), NULL, "grupo2-%d", i);
    }
    
    return 0;
}

static void __exit mychardev_exit(void)
{
    int i;

    for (i = 0; i < MAX_DEV; i++) {
        device_destroy(mychardev_class, MKDEV(dev_major, i));
    }

    class_unregister(mychardev_class);
    class_destroy(mychardev_class);

    unregister_chrdev_region(MKDEV(dev_major, 0), MINORMASK);
    del_timer(&g_timer);
    ((driver->ops)->ioctl) (vc_cons[fg_console].d->port.tty, KDSETLED, 0xFF);
}

static int mychardev_open(struct inode *inode, struct file *file)
{
    printk("MYCHARDEV: Device open\n");
    return 0;
}

static int mychardev_release(struct inode *inode, struct file *file)
{
    printk("MYCHARDEV: Device close\n");
    return 0;
}

static long mychardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    printk("MYCHARDEV: Device ioctl\n");
    return 0;
}

static ssize_t mychardev_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
    uint8_t *data = "Hello from the kernel world!\n";
    size_t datalen = strlen(data);

    printk("Reading device: %d\n", MINOR(file->f_path.dentry->d_inode->i_rdev));

    if (count > datalen) {
        count = datalen;
    }

    if (copy_to_user(buf, data, count)) {
        return -EFAULT;
    }

    return count;
}

void LigaDesligaLEDNumLock(int valor)
{
  driver = vc_cons[fg_console].d->port.tty->driver;

  ((driver->ops)->ioctl) (vc_cons[fg_console].d->port.tty, KDSETLED, valor);
  /*
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
	outb(valor, 0x60);*/
}

int potenciadedez (int expoente)
{
  int i, resultado = 1;
  for (i = 1; i <= expoente; i++)
    resultado = resultado * 10;
  return resultado;
}

static ssize_t mychardev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
    size_t maxdatalen = 30, ncopied;
    uint8_t databuf[maxdatalen];
    int freq = 0, i;
    printk("Writing device: %d\n", MINOR(file->f_path.dentry->d_inode->i_rdev));

    if (count < maxdatalen) {
        maxdatalen = count;
    }

    ncopied = copy_from_user(databuf, buf, maxdatalen);

    if (ncopied == 0) {
        printk("Copied %zd bytes from the user\n", maxdatalen);
    } else {
        printk("Could't copy %zd bytes from the user\n", ncopied);
    }

    databuf[maxdatalen] = 0;

    printk("Data from the user: %s\n", databuf);

    if (databuf[0] == '0' && count == 2)
    {
      del_timer(&g_timer);
      printk("Desligar LED Num Lock:\n");
      LigaDesligaLEDNumLock(0);
    }
    else{
      if (databuf[0] == '1' && count == 2){
        del_timer(&g_timer);
        printk("Ligar LED Num Lock:\n");
        LigaDesligaLEDNumLock(2); 
      }
      else
        if (count == 5){
          for (i = 0; i < count - 2; i++)
            freq += (databuf[i] - 48) * potenciadedez(i) ;
          if (freq < 1000){
            timer_setup(&g_timer, TimerHandler, 0);
            mod_timer(&g_timer, jiffies + msecs_to_jiffies(g_time_interval));  
          }
          else{
            g_time_interval = freq;
            timer_setup(&g_timer, TimerHandler, 0);
            mod_timer(&g_timer, jiffies + msecs_to_jiffies(g_time_interval));
          }
          ;
          
        }
    }

    return count;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rennes Freitas Souza");

module_init(mychardev_init);
module_exit(mychardev_exit);
