#include <linux/init.h>     
#include <linux/module.h>     
#include <linux/kernel.h>   
#include <linux/kthread.h>
#include <linux/semaphore.h>

MODULE_LICENSE("GPL");     
MODULE_AUTHOR("Reem Helal");  

struct task_struct *ts1[100]; //It can support up to 100 producers
struct task_struct *ts2[100]; //It can support up to 100 consumers
struct semaphore name;
struct semaphore empty;
struct semaphore full;
char thread_name[TASK_COMM_LEN] = { };

//input parameters for the kernel module >> producer, consumer and size
static int prod = 0;
module_param(prod, int, 0);

static int cons = 0;
module_param(cons, int, 0);

static int size = 0;
module_param(size, int, 0);

static int prod_thread(void *arg) {
    //Thread code
    int id = 0;
    char name[TASK_COMM_LEN];
    get_task_comm(name, current); //stores the name of the producer kthread
    sscanf(name, "Producer-%d", &id); //it extracts the id number of the producer

    //while the kthread is running
    while (!kthread_should_stop()){
        if (down_interruptible(&empty)) break; 
            printk("An item has been produced by Producer-%d\n", id); 
            up(&full); 
    }    
    return 0;
}

static int cons_thread(void *arg) {
    
    int id = 0;
    char name[TASK_COMM_LEN];
    get_task_comm(name, current); //stores the name of the consumer kthread
    sscanf(name, "Consumer-%d", &id); //it extracts the id number of the consumer

    while (!kthread_should_stop()){
        if (down_interruptible(&full)) break; 
            printk("An item has been consumed by Consumer-%d\n", id);
            up(&empty);  
    }
    return 0;
}

static int __init producer_consumer_init(void) {  

    int id;
    struct task_struct *t = current;
    get_task_comm(thread_name, t); // t is a task_struct pointer 
    sema_init(&empty, size); //initialize empty's address to size 
    sema_init(&full, 0); //initialize full's address to 0

    for (id = 0; id < prod; id++){
        ts1[id] = kthread_run(prod_thread, NULL, "Producer-%d", id + 1);
    }

    for (id = 0; id < cons; id++){
        ts2[id] = kthread_run(cons_thread, NULL, "Consumer-%d", id + 1); 
    }

    //printk(KERN_INFO "Hello, Reem!\n");     
    return 0;     
}

static void __exit producer_consumer_exit(void) { //there's something wrong in this function 

    int id;
    //loops through every producer thread, which is the number of producers the test asks for, till it exits and stops the thread
    for (id = 0; id < prod; id++){
        if (ts1[id]) {
            kthread_stop(ts1[id]); 
        }
    }    
    
    //loops through every consumer thread till it exits and stops the thread
    for (id = 0; id < cons; id++){
        if (ts2[id]) {
            kthread_stop(ts2[id]);
        }
    }
    //printk(KERN_INFO "Goodbye, Reem!\n");     
}

module_init(producer_consumer_init);     
module_exit(producer_consumer_exit);