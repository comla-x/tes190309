#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/module.h>
#include <asm/string.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/device.h>
/*#define		GPM4CON	0x110002e0    //led
#define		GPM4DAT	0x110002e4
#define 	GPX3CON 0x11000c60	//key1 ~ 3 -> GPX3_2 ~ 5
#define 	GPX3DAT 0x11000c64  	*/
#define		INTERBUFSIZE 32

struct  keyinfo  {
	char  name[12];
	int  gpio;
	int  value;
};
struct  ledinfo  {
	char	name[12];
	int  	gpio;
	int  	value;
};

struct ledmotion 
{
	char  	on[4];
	char  	off[5];
	char  	xor[5];
};
static struct ledmotion ledmot = {
	.on		= "_on",
	.off 	= "_off",
	.xor	= "_xor"
};

/* 1.�����ļ����������ṹ�� */
static struct file_operations g_top;	//�ļ����������ṹ��
static int major = 0;	//�����ַ����豸��

/* ������ָ�� */
struct class * g_ptclass = NULL;


static   struct  keyinfo  g_tarrkey[] = {
		{"key1_on",   EXYNOS4_GPX3(2),  1},
		{"key2_on",   EXYNOS4_GPX3(3),  1},
		{"key3_on",   EXYNOS4_GPX3(4),  1},
		{"key4_on",   EXYNOS4_GPX3(5),  1},
};
static   struct  ledinfo  g_tarrled[] = {
		{"led1", EXYNOS4X12_GPM4(0), 1},
		{"led2", EXYNOS4X12_GPM4(1), 1},
		{"led3", EXYNOS4X12_GPM4(2), 1},
		{"led4", EXYNOS4X12_GPM4(3), 1},
};

//�ض���write����
ssize_t misc_led_write (struct file * flip, const char __user * buf, size_t count, loff_t * offset)
{
	u32 ret,i;
	char inter_buf[INTERBUFSIZE] = {0};
	int led_value = 0;
	ret = copy_from_user(inter_buf, buf, INTERBUFSIZE);	//��ȡӦ�ò������
	for(i = 0;i < ARRAY_SIZE(g_tarrled);i++)
	{
		if(strncmp(inter_buf, g_tarrled[i].name, 4) == 0)	//�ж����ĸ�led
		{
			if(strcmp(inter_buf + 4,ledmot.on) == 0)	//����
			{
				gpio_set_value(g_tarrled[i].gpio,0);
			}
			else if(strcmp(inter_buf + 4,ledmot.off) == 0)	//����
			{
				gpio_set_value(g_tarrled[i].gpio,1);
			}
			else if(strcmp(inter_buf + 4,ledmot.xor) == 0)	//�����
			{
				led_value = gpio_get_value(g_tarrled[i].gpio);
				led_value = !led_value;
				gpio_set_value(g_tarrled[i].gpio,led_value);
			}
			break;
		}
	}
	
	printk("--write:%s\n",inter_buf);
	return min(count, sizeof(inter_buf));
}

//�ض���read����
ssize_t misc_key_read (struct file * flip, char __user * buf, size_t count, loff_t * offset)
{
	int key_value;
	long ret,i;


	for(i = 0;i < ARRAY_SIZE(g_tarrkey);i++)
	{
		key_value = gpio_get_value(g_tarrkey[i].gpio);	//��ȡ����״̬
		if(key_value == 0)
		{
			while(1)	//�������ּ��
			{
				key_value = gpio_get_value(g_tarrkey[i].gpio);
				if(key_value == 1)
				{
					break;
				}
			}
			ret = copy_to_user(buf, g_tarrkey[i].name, min(count,strlen(g_tarrkey[i].name)));	//�������͵�Ӧ�ò�
			printk("--read:%s\n",g_tarrkey[i].name);	
			break;
		}
	}
	return min(count,strlen(g_tarrkey[i].name));	//����ʵ�ʶ�ȡ�����ֽ���
}

//���ļ����������ṹ����и�ֵ,�����ṩ�ӿ�
void file_ops_init(struct file_operations * ops)
{
	ops->owner	=	THIS_MODULE;
	ops->read	=	misc_key_read;
	ops->write	=	misc_led_write;
}




static int __init misc_led_init(void)
{
	int  i = 0;
	file_ops_init(&g_top);
	/* 2.ע�������ַ��豸���� */
	major = register_chrdev(0, "chrdevs_key_led", &g_top);
	printk("========== create a new chrdevs ==========\n");
	printk("name:gpio_key_led | major:%d\n",major);

	g_ptclass = class_create(THIS_MODULE, "key_led");
	device_create(g_ptclass, NULL, MKDEV(major, 0), NULL, "auto_chrdevs_key_led");

	/* ����gpio����ʼ��	*/
	for(i = 0;  i < ARRAY_SIZE(g_tarrkey);  i++)
	{
		gpio_request(g_tarrkey[i].gpio,   g_tarrkey[i].name);
		gpio_direction_input(g_tarrkey[i].gpio);
	}

	for(i = 0;  i < ARRAY_SIZE(g_tarrled);  i++)
	{
		gpio_request(g_tarrled[i].gpio,   g_tarrled[i].name);
		gpio_direction_output(g_tarrled[i].gpio, 1);
	}		
	

	

	
	
	return 0;
	
}




static void __exit misc_led_exit(void)
{
	int  i = 0;
	/* �ͷ�gpio */
	for(i = 0;  i < ARRAY_SIZE(g_tarrkey);  i++)
	{
		gpio_free(g_tarrkey[i].gpio);
	}
	for(i = 0;  i < ARRAY_SIZE(g_tarrled);  i++)
	{
		gpio_free(g_tarrled[i].gpio);
	}
	
	unregister_chrdev(major,  "chrdevs_key_led");
	device_destroy(g_ptclass, MKDEV(major, 0));
}


module_init(misc_led_init);	//ģ���ʼ��
module_exit(misc_led_exit);	//ģ���˳�

MODULE_LICENSE("GPL");







