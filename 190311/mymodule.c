#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>

#define GPX3CON 0x11000c60	//key1 ~ 3 -> GPX3_2 ~ 5
#define GPX3DAT 0x11000c64  
#define GPX0CON 0x114000A0	//beep GPD0_0
#define GPX0DAT 0x114000A4
#define GPM4CON 0x110002e0
#define GPM4DAT 0x110002e4

#define REGSIZE 4
/* 
* 	��ں��� -- ģ�����ʱ�Զ�����
*	ģ�����ʱҪ�������в����������д������
*/
static  int    __init   my_module_init(void)
{
	
	void __iomem * keyConPtr;
	void __iomem * keyDatPtr;
	void __iomem * beepConPtr;
	void __iomem * beepDatPtr;
	void __iomem * ledConPtr;
	void __iomem * ledDatPtr;

	unsigned int keyValue = 0;
	unsigned int value;
	
	keyConPtr 	= ioremap(GPX3CON, REGSIZE);
	keyDatPtr   = ioremap(GPX3DAT, REGSIZE);
	
	beepConPtr	= ioremap(GPX0CON, REGSIZE);
	beepDatPtr 	= ioremap(GPX0DAT, REGSIZE);

	ledConPtr  	= ioremap(GPM4CON, REGSIZE);
	ledDatPtr 	= ioremap(GPM4DAT, REGSIZE);
	

	value = readl(keyConPtr);
	value &= ~( 0xf << (2 * 4) );	//key1
	value &= ~( 0xf << (3 * 4) );	//key2
	writel(value, keyConPtr);	//��ʼ��key1��2��Ӧ�˿�
	 		
	value = readl(beepConPtr);
	value &= ~( 0xf << (0 * 4) );	//beep
	value |=  0x1 << (0 * 4) ;
	writel(value, beepConPtr);	//��ʼ��beep��Ӧ�˿�
	
	value = readl(ledConPtr);
	value &= ~( 0xf << (0 * 4) );	//key1
	value &= ~( 0xf << (1 * 4) );	//key2
	value &= ~( 0xf << (2 * 4) );	//key3
	value &= ~( 0xf << (3 * 4) );	//key4

	value |=  0x1 << (0 * 4);	//key1
	value |=  0x1 << (1 * 4);	//key2
	value |=  0x1 << (2 * 4);	//key3
	value |=  0x1 << (3 * 4);	//key4
	writel(value,ledConPtr);

	
	while(1)
	{
		keyValue = readl(keyDatPtr);
		keyValue = keyValue & ( 1 << (1 * 2) );	//key1
		printk("%x\n",keyValue);
		if(!keyValue)
		{
			value = readl(beepDatPtr);
			value |= 1 << (1 * 0);
			writel(value, beepDatPtr);

			value = readl(ledDatPtr);
			value &= ~0xf;
			writel(value,ledDatPtr);
		}
		keyValue = readl(keyDatPtr);
		keyValue = keyValue & ( 1 << (1 * 3) );	//key2
		if(!keyValue)
		{
			//value = 50000000000000;
			//while(value--);
			value = readl(beepDatPtr);
			value &= ~( 1 << (1 * 0) );
			writel(value, beepDatPtr);

			value = readl(ledDatPtr);
			value |= 0xf;
			writel(value,ledDatPtr);
		}
		
	}
	


	printk("---my first moudle---\n");
	
	return  0;	// ���뷵��0��ģ����ܼ��سɹ�
}

/*
*	���ں��� -- ģ��ж��ʱ�Զ�����
*	ģ��ж��ʱҪ�������в������������������
*/
static  void   __exit   my_module_exit(void)
{
	iounmap;
	printk("---my moudle finish---\n");
}


/* ģ����������������ĸ�������ģ�����ʱ���õ���ں��� */
module_init(my_module_init);		// ��������ں����ĺ�����
/* ģ����������������ĸ�������ģ��ж��ʱ���õĳ��ں��� */
module_exit(my_module_exit);	//�����ǳ��ں����ĺ�����

MODULE_LICENSE("GPL");



