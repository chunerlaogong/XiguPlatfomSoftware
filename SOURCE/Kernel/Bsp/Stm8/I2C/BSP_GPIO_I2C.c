/*
********************************************************************************
*
*                                 BSP_GPIO_I2C.c
*
* File          : BSP_IIC.c
* Version       : V1.0
* Author        : Lhl
* Mode          : Thumb2
* Toolchain     :
* Description   : IIC驱动程序（GPIO模拟IIC）
*
* History       :
* Date          : 2018.05.08
*******************************************************************************/
#include "BSP_GPIO_I2C.h"
#include "Delay.h"

/*******************************************************************************
* Function Name :BSP_IICInit(void)
* Description   :模拟IIC初始化
* Output        :
* Other         :
* Date          :2018.05.08
*******************************************************************************/
void BSP_I2cInit(PGPIO_I2C_TypeDef pGpioI2cStructer)
{
    I2C_INIT(pGpioI2cStructer->GPIO_SCL_BASE, pGpioI2cStructer->GPIO_SCL_PIN,
             pGpioI2cStructer->GPIO_SDA_BASE,
             pGpioI2cStructer->GPIO_SDA_PIN);       //I2C初始化
    I2C_SCL_SET(pGpioI2cStructer->GPIO_SCL_BASE, pGpioI2cStructer->GPIO_SCL_PIN);
    I2C_SDA_SET(pGpioI2cStructer->GPIO_SDA_BASE, pGpioI2cStructer->GPIO_SDA_PIN);
}
/*******************************************************************************
* Function Name : void BSP_I2cStart(PGPIO_I2C_TypeDef pGpioI2cStructer)
* Description   : I2C开始信号
* Author        : YJD
* Input         : gpioI2cStructer: 自定义模拟I2C结构体
* Output        :
* Other         :
* Date          : 2018.05.11
*******************************************************************************/
uint8_t BSP_I2cStart(PGPIO_I2C_TypeDef pGpioI2cStructer)
{
    I2C_SDA_SET(pGpioI2cStructer->GPIO_SDA_BASE, pGpioI2cStructer->GPIO_SDA_PIN);
    I2C_SCL_SET(pGpioI2cStructer->GPIO_SCL_BASE, pGpioI2cStructer->GPIO_SCL_PIN);
    DelayUs(5);
    I2C_SDA_CLR(pGpioI2cStructer->GPIO_SDA_BASE, pGpioI2cStructer->GPIO_SDA_PIN);
    DelayUs(5);
    I2C_SCL_CLR(pGpioI2cStructer->GPIO_SCL_BASE, pGpioI2cStructer->GPIO_SCL_PIN);
    return 1;
}
/*******************************************************************************
* Function Name : void BSP_I2cStop(PGPIO_I2C_TypeDef pGpioI2cStructer)
* Description   : IIC停止信号
* Author        : YJD
* Input         : gpioI2cStructer: 自定义模拟I2C结构体
* Output        :
* Other         :
* Date          : 2018.05.11
*******************************************************************************/
void BSP_I2cStop(PGPIO_I2C_TypeDef pGpioI2cStructer)
{
    I2C_SCL_CLR(pGpioI2cStructer->GPIO_SCL_BASE, pGpioI2cStructer->GPIO_SCL_PIN);
    I2C_SDA_SET(pGpioI2cStructer->GPIO_SDA_BASE, pGpioI2cStructer->GPIO_SDA_PIN);
    DelayUs(5);
    I2C_SDA_CLR(pGpioI2cStructer->GPIO_SDA_BASE, pGpioI2cStructer->GPIO_SDA_PIN);
    I2C_SCL_SET(pGpioI2cStructer->GPIO_SCL_BASE, pGpioI2cStructer->GPIO_SCL_PIN);
    DelayUs(5);
    I2C_SDA_SET(pGpioI2cStructer->GPIO_SDA_BASE, pGpioI2cStructer->GPIO_SDA_PIN);
}

/*******************************************************************************
* Function Name : int IIC_Wait_Ack(uint8_t Channel)
* Description   : 等待应答信号到来
* Input         : Channel:通道
* Output        :
* Other         :
* Date          : 2018.05.11
*******************************************************************************/
int BSP_I2c_Wait_Ack(PGPIO_I2C_TypeDef pGpioI2cStructer)
{
    uint8_t ucErrTime = 0;
    I2C_SDA_SET(pGpioI2cStructer->GPIO_SDA_BASE, pGpioI2cStructer->GPIO_SDA_PIN);
    DelayUs(5);
    I2C_SCL_SET(pGpioI2cStructer->GPIO_SCL_BASE, pGpioI2cStructer->GPIO_SCL_PIN);
    DelayUs(5);
    while(I2C_SDA_READ(pGpioI2cStructer->GPIO_SDA_BASE,
                       pGpioI2cStructer->GPIO_SDA_PIN ))
    {
        ucErrTime++;
        if(ucErrTime > 150)
        {
            BSP_I2cStop(pGpioI2cStructer);
            return 0;
        }
        DelayUs(5);
    }
    I2C_SCL_SET(pGpioI2cStructer->GPIO_SCL_BASE, pGpioI2cStructer->GPIO_SCL_PIN);
    DelayUs(5);
    I2C_SCL_CLR(pGpioI2cStructer->GPIO_SCL_BASE, pGpioI2cStructer->GPIO_SCL_PIN);
    return 1;
}

/*******************************************************************************
* Function Name : void I2C_Ack(uint8_t Channel)
* Description   : 发送一个Ack信号
* Input         : Channel:通道
* Output        :
* Other         :
* Date          : 2018.05.11
*******************************************************************************/
void BSP_I2c_Ack(PGPIO_I2C_TypeDef pGpioI2cStructer)
{
    I2C_SCL_CLR(pGpioI2cStructer->GPIO_SCL_BASE, pGpioI2cStructer->GPIO_SCL_PIN);
    I2C_SDA_CLR(pGpioI2cStructer->GPIO_SDA_BASE, pGpioI2cStructer->GPIO_SDA_PIN);
    DelayUs(5);
    I2C_SCL_SET(pGpioI2cStructer->GPIO_SCL_BASE, pGpioI2cStructer->GPIO_SCL_PIN);
    DelayUs(5);
    I2C_SCL_CLR(pGpioI2cStructer->GPIO_SCL_BASE, pGpioI2cStructer->GPIO_SCL_PIN);
    DelayUs(5);
    I2C_SDA_SET(pGpioI2cStructer->GPIO_SDA_BASE, pGpioI2cStructer->GPIO_SDA_PIN);
}

/*******************************************************************************
* Function Name : void I2C_NAck(uint8_t Channel)
* Description   : 发送一个NAck信号
* Input         : Channel:通道
* Output        :
* Other         :
* Date          : 2018.05.11
*******************************************************************************/
void BSP_I2c_NAck(PGPIO_I2C_TypeDef pGpioI2cStructer)
{
    I2C_SCL_CLR(pGpioI2cStructer->GPIO_SCL_BASE, pGpioI2cStructer->GPIO_SCL_PIN);
    I2C_SDA_SET(pGpioI2cStructer->GPIO_SDA_BASE, pGpioI2cStructer->GPIO_SDA_PIN);
    DelayUs(5);
    I2C_SCL_SET(pGpioI2cStructer->GPIO_SCL_BASE, pGpioI2cStructer->GPIO_SCL_PIN);
    DelayUs(5);
    I2C_SCL_CLR(pGpioI2cStructer->GPIO_SCL_BASE, pGpioI2cStructer->GPIO_SCL_PIN);
    I2C_SDA_CLR(pGpioI2cStructer->GPIO_SDA_BASE, pGpioI2cStructer->GPIO_SDA_PIN);
}

/*******************************************************************************
* Function Name : uint8_t IIc8BitRead(uint8_t m_uACK,uint8_t Channel)
* Description   : IIC读出1字节数据
* Input         : Address:地址
* Output        :
* Other         :
* Date          : 2018.05.11
*******************************************************************************/
uint8_t BSP_I2cReadByte(uint8_t m_uACK, PGPIO_I2C_TypeDef pGpioI2cStructer)
{
    uint8_t i = 0;
    uint8_t Temp = 0;
    for(i = 0; i < 8; i++)
    {
        I2C_SCL_CLR(pGpioI2cStructer->GPIO_SCL_BASE, pGpioI2cStructer->GPIO_SCL_PIN);
        DelayUs(5);
        I2C_SCL_SET(pGpioI2cStructer->GPIO_SCL_BASE, pGpioI2cStructer->GPIO_SCL_PIN);
        Temp = Temp << 1;
        if(I2C_SDA_READ(pGpioI2cStructer->GPIO_SDA_BASE, pGpioI2cStructer->GPIO_SDA_PIN ))
            Temp++;
        DelayUs(5);
    }
    if(m_uACK)   //如果有ACK
    {
        BSP_I2c_Ack(pGpioI2cStructer);
    }
    else
        BSP_I2c_NAck(pGpioI2cStructer);
    return Temp;
}

/*******************************************************************************
* Function Name : uint8_t IIc8BitWrite(uint8_t data , uint8_t Channel)
* Description   : IIC写入1字节数据
* Input         : data: 需要写入的数据          Channel:通道
* Output        :
* Other         :
* Date          : 2018.05.11
*******************************************************************************/
uint8_t BSP_I2cWriteByte(uint8_t data, PGPIO_I2C_TypeDef pGpioI2cStructer)
{
    uint8_t i = 0;
//  I2C_SCL_CLR(pGpioI2cStructer->GPIO_SCL_BASE, pGpioI2cStructer->GPIO_SCL_PIN);
    for(i = 0; i < 8; i++)
    {
        if(data & 0x80)//取最高位
        {
            I2C_SDA_SET(pGpioI2cStructer->GPIO_SDA_BASE, pGpioI2cStructer->GPIO_SDA_PIN);
        }
        else
        {
            I2C_SDA_CLR(pGpioI2cStructer->GPIO_SDA_BASE, pGpioI2cStructer->GPIO_SDA_PIN);
        }
        data = data << 1;
        DelayUs(5);
        I2C_SCL_SET(pGpioI2cStructer->GPIO_SCL_BASE, pGpioI2cStructer->GPIO_SCL_PIN);
        DelayUs(5);
        I2C_SCL_CLR(pGpioI2cStructer->GPIO_SCL_BASE, pGpioI2cStructer->GPIO_SCL_PIN);
        DelayUs(5);

    }
    return BSP_I2c_Wait_Ack(pGpioI2cStructer);
}

/*******************************************************************************
* Function Name : uint8_t BSP_I2cWriteBytes(uint8_t Address, PGPIO_I2C_TypeDef pGpioI2cStructer,uint8_t* pData, uint8_t len)
* Description   : IIC写数据
* Input         :
* Output        :
* Other         :
* Date          : 2018.05.11
*******************************************************************************/
uint8_t BSP_I2cWriteBytes(uint8_t Address, PGPIO_I2C_TypeDef pGpioI2cStructer,
                          uint8_t* pData, uint8_t len)
{
    int i;
    if (!BSP_I2cStart(pGpioI2cStructer))
        return 1;
    if(!(BSP_I2cWriteByte((pGpioI2cStructer->SLAVE_ADDRESS) << 1, pGpioI2cStructer)))
        return 1;
    if(!(BSP_I2cWriteByte(Address, pGpioI2cStructer)))
        return 1;
    for (i = 0; i < len; i++)
    {
        if(!(BSP_I2cWriteByte(pData[i], pGpioI2cStructer)))
            return 1;
    }
    BSP_I2cStop(pGpioI2cStructer);
    return 0;
}


/*******************************************************************************
* Function Name : uint8_t BSP_I2cReadBytes(uint8_t Address, PGPIO_I2C_TypeDef pGpioI2cStructer,uint8_t* pData, uint8_t len)
* Description   : IIC读出数据
* Input         : Address:地址 Channel:通道 pData:返回的数据 Len:长度
* Output        :
* Other         :
* Date          : 2018.05.11
*******************************************************************************/
uint8_t BSP_I2cReadBytes(uint8_t Address, PGPIO_I2C_TypeDef pGpioI2cStructer,
                         uint8_t* pData, uint8_t len)
{
    BSP_I2cStart(pGpioI2cStructer);     //开始电平
    if(!(BSP_I2cWriteByte((pGpioI2cStructer->SLAVE_ADDRESS << 1) + I2C_WRITE,
                          pGpioI2cStructer)))
        return 1;
    if(!(BSP_I2cWriteByte(Address, pGpioI2cStructer)))
        return 1;
    if (!BSP_I2cStart(pGpioI2cStructer))
        return 1;
    if(!(BSP_I2cWriteByte((pGpioI2cStructer->SLAVE_ADDRESS << 1) + I2C_READ,
                          pGpioI2cStructer)))
        return 1;
    while (len)
    {
        if (len == 1)
            *pData = BSP_I2cReadByte(0, pGpioI2cStructer);
        else
            *pData = BSP_I2cReadByte(1, pGpioI2cStructer);
        pData++;
        len--;
    }
    BSP_I2cStop(pGpioI2cStructer);
    return 0;
}




