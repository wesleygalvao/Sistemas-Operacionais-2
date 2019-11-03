
//TIMER - exemplo funcional


#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/init.h> /* Needed for the macros */
#include <linux/timer.h>
 
int g_time_interval = 10000;
struct timer_list g_timer;
 
void TimerHandler(struct timer_list *t)
{
    /*Restarting the timer...*/
    mod_timer( &g_timer, jiffies + msecs_to_jiffies(g_time_interval));
 
    printk(KERN_INFO "Timer Handler called.\n");
}
 
int init_module(void)
{
    printk(KERN_INFO "My module inserted into kernel!!!.\n");
 
    /*Starting the timer.*/
    timer_setup(&g_timer, TimerHandler, 0);
    mod_timer( &g_timer, jiffies + msecs_to_jiffies(g_time_interval));
 
    return 0;
}
 
void cleanup_module(void)
{
    del_timer(&g_timer);
    printk(KERN_INFO "My module exited from kernel!!!\n");
}