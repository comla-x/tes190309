#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/module.h>
#include <asm/string.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/delay.h>
#define   	CMD_GET_KEY1  		_IOR('k',  1,  char*)
#define   	CMD_GET_KEY2  		_IOR('k',  2,  char*)
#define  	BASEMINOR  	110
#define  	MINORCNT	4
#define  	DEVNAME	"cdev_key"

static struct file_operations g_tfops;	//�ļ����������ṹ��
dev_t g_devnum;	//�豸��
//unsigned long g_devnum;	//�豸��
static struct cdev * g_ptcdev;	//��׼�ַ��豸�ṹ��ָ��
struct class *  g_ptclass = NULL;	//�豸��

struct key_into
{
	char  name[12];
	int  gpio;
	int  value;
};

struct key_into g_tkey[4];
long cdev_key_ioctl (struct file * filp, unsigned int cmd, unsigned long arg)
{
	unsigned char key_statu = 1;
	char buf[32] = {0};
	unsigned long ret;

	//printk("%d\n", cmd);
	switch(cmd)
	{
		case CMD_GET_KEY1:
			key_statu = gpio_get_value(g_tkey[0].gpio);
			if(key_statu == 0)
			{
				msleep(10);
				key_statu = gpio_get_value(g_tkey[0].gpio);
				if(key_statu == 0)
				{
					while(1)
					{
						key_statu = gpio_get_value(g_tkey[0].gpio);
						if(key_statu == 1)
						{
							break;
						}
					}

				}
				strcpy(buf,g_tkey[0].name);
				ret = copy_to_user((void *)arg, buf, strlen(buf) + 1);
				//printk("==== %s has been pressed!\n",g_tkey[0].name);
			}
			break;
			
		case CMD_GET_KEY2:
			key_statu = gpio_get_value(g_tkey[1].gpio);
			if(key_statu == 0)
			{
				msleep(10);
				key_statu = gpio_get_value(g_tkey[1].gpio);
				if(key_statu == 0)
				{
					while(1)
					{
						key_statu = gpio_get_value(g_tkey[1].gpio);
						if(key_statu == 1)
						{
							break;
						}
					}

				}
				strcpy(buf,g_tkey[1].name);
				ret = copy_to_user((void *)arg, buf, strlen(buf) + 1);
				//printk("==== %s has been pressed!\n",g_tkey[1].name);
			}
			break;
			
		default:printk("*********11111 error cmd *********\n");
			break;
	}

	return 0;
}




void struct_init(struct file_operations * fops, struct  key_into * k)
{
	/* �ļ����������ṹ�� */
	fops->owner= THIS_MODULE;
	fops->unlocked_ioctl = cdev_key_ioctl;

	/* �����ṹ�� */
	strcpy(k[0].name, "key1");
	k[0].gpio 	= EXYNOS4_GPX3(2);
	k[0].value 	= 1;

	strcpy(k[1].name, "key2");
	k[1].gpio 	= EXYNOS4_GPX3(2);
	k[1].value 	= 1;
}






static int __init cdev_key_init(void)
{
	struct_init(&g_tfops, &g_tkey[0]);

	/* ע���׼�ַ��豸 */
	alloc_chrdev_region(&g_devnum,  BASEMINOR,  MINORCNT,  DEVNAME);	//�����ַ��豸��
	g_ptcdev = cdev_alloc();	//�����׼�ַ��豸���ͽṹ��
	cdev_init(g_ptcdev,  &g_tfops);	//�Ա�׼�ַ��豸�ṹ���ʼ��
	cdev_add(g_ptcdev,  g_devnum,  MINORCNT);	//����׼�ַ��豸���뵽�ں�
	printk("========== create a new cdev ==========\n");
	printk("--- major: %d --- minor: %d  ---\n",  MAJOR(g_devnum), MINOR(g_devnum)); 
	
	/* �豸��Դ���롢��ʼ�� */
	gpio_request(g_tkey[0].gpio, g_tkey[0].name);
	gpio_direction_input(g_tkey[0].gpio);

	/* �����豸�ڵ��ļ� */
	g_ptclass = class_create(THIS_MODULE,  "button");
	device_create(g_ptclass, NULL, g_devnum, NULL, "auto_key1");
	
	return 0;
}




static void __exit cdev_key_exit(void)
{
	/* �ͷ��豸��Դ */
	gpio_free(g_tkey[0].gpio);

	/* �ͷű�׼�ַ��豸�ṹ�� */
	cdev_del(g_ptcdev);
	kfree(g_ptcdev);
	unregister_chrdev_region(g_devnum,  MINORCNT);

	/* �Ƴ��豸�ڵ��ļ� */
	device_destroy(g_ptclass, g_devnum);
	class_destroy(g_ptclass);
}














module_init(cdev_key_init);	//ģ���ʼ��
module_exit(cdev_key_exit);	//ģ���˳�

MODULE_LICENSE("GPL");

EXPORT_SYMBOL(g_devnum);
EXPORT_SYMBOL(g_ptclass);

