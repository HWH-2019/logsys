#include <linux/module.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <stdarg.h>

extern unsigned long volatile pfcount;
static struct timer_list test_timer;
static unsigned long pfcount_last;
static unsigned long pfcount_in_2;
static int count = 0;
static struct file* filp;

static char* path="/usr/src/text.txt";
module_param(path, charp, S_IRUGO);


MODULE_LICENSE("GPL");


struct file* file_open(const char* path, int flags, int rights) {
    struct file* filp = NULL;
    int err = 0;

    filp = filp_open(path, flags, rights);
    if(IS_ERR(filp)) {
        err = PTR_ERR(filp);
        return NULL;
    }
    return filp;
}

void file_close(struct file* file) {
    filp_close(file, NULL);
}



/*5,实现show函数
  作用是将内核数据输出到用户空间
  将在proc file输出时被调用
  */
static int my_proc_show(struct seq_file *m, void *v)
{
    /*这里不能使用printfk之类的函数
      要使用seq_file输出的一组特殊函数
      */
	seq_printf(m, "[latest] Number of page fault interrupts in 2 seconds: %ld !\n", pfcount_in_2);
    return 0;
}

//定时器的回调函数
static void irq_test_timer_function(struct timer_list  *timer)
{
	
	printk("%d Number of page fault interrupts in 2 seconds: %ld\n",count,pfcount - pfcount_last);
	pfcount_in_2 = pfcount - pfcount_last;
	
	char buf[100];
	sprintf(buf,"%d Number of page fault interrupts in 2 seconds: %ld\n",count,pfcount_in_2);
	
    	kernel_write(filp, buf, strlen(buf),&filp->f_pos);
	pfcount_last = pfcount;
	mod_timer(&test_timer, jiffies + 2 * HZ);
	count++;
}
 
static int my_proc_open(struct inode *inode, struct file *file)
{		
    /*4,在open函数中调用single_open绑定seq_show函数指针*/
    return single_open(file, my_proc_show, NULL);
}
 
/*2,填充proc_create函数中调用的flie_operations结构体
  其中my开头的函数为自己实现的函数，
  seq和single开头为内核实现好的函数，直接填充上就行
  open为必须填充函数
  */
static struct proc_ops my_fops = {
  .proc_open = my_proc_open,
  .proc_read = seq_read,
  .proc_lseek = seq_lseek,
  .proc_release = single_release,
};
 
static int __init my_init(void)
{

	
    printk(KERN_INFO "change to the new log file path: %s",path);
    filp = file_open(path,O_RDWR|O_TRUNC, 0777);
    
    
    struct proc_dir_entry *file;
    //创建父级目录，第二个参数NULL表示在/proc下
    //这里用我的学号当做文件名
    struct proc_dir_entry *parent = proc_mkdir("3190608027",NULL);

    /*1,
      首先要调用创建proc文件的函数，需要绑定flie_operations
      参数1：要创建的文件
      参数2：权限设置
      参数3：父级目录，如果传NULL，则在/proc下
      参数4：绑定flie_operations
      */
    file = proc_create("readpfcount", 0644, parent, &my_fops);
    if(!file)
        return -ENOMEM;
     
    //创建定时器  
    pfcount_last = pfcount;
	test_timer.expires  = jiffies + 2 * HZ;
	timer_setup(&test_timer, irq_test_timer_function, 0);
	
	add_timer(&test_timer);
	
	printk(KERN_INFO "already init and add timer\n");
    return 0;
}
 
/*6,删除proc文件*/
static void __exit my_exit(void)
{
  file_close(filp);
  printk(KERN_INFO "exit timer drv\n");
  del_timer(&test_timer);
  //移除目录及文件
  remove_proc_entry("readpfcount", NULL);
}
 
module_init(my_init);
module_exit(my_exit);



