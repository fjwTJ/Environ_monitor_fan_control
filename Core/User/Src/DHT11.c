#include "dht11.h"
/**
  * @brief  娓╂箍搴︿紶鎰熷櫒涓诲嚱鏁?
  * @param  void
  * @retval None
  */
static void DHT11_GPIO_MODE_SET(uint8_t mode);
uint8_t data[5] = {0};

void DHT11(void)
{
	if(!f_400ms) return;
	f_400ms = 0;
	if(DHT11_READ_DATA() == 1)
	{
	}
	else
	{
	}                           
}
 
/**
  * @brief  娓╂箍搴︿紶鎰熷櫒鍚姩淇″彿鍙戦€?
  * @param  void
  * @retval None
  */
void DHT11_START(void)
{
    DHT11_GPIO_MODE_SET(0);                         //  涓绘満璁剧疆涓鸿緭鍑烘ā寮?
    
    DHT11_PIN_RESET;                                //  涓绘満鎷変綆鐢靛钩
    
    HAL_Delay(20);                                  //  涓绘満绛夊緟 18 < ms > 30
    
    DHT11_GPIO_MODE_SET(1);                         //  涓绘満璁剧疆涓鸿緭鍏ユā寮忥紝绛夊緟DHT11绛斿簲
}                                                   //  鍥犱负璁剧疆浜嗕笂鎷夎緭鍏ワ紝GPIO -> 1
 
/**
  * @brief  璇诲彇涓€浣嶆暟鎹?1bit
  * @param  void
  * @retval 0/1
  */
unsigned char DHT11_READ_BIT(void)
{
    while(!DHT11_READ_IO);                          //  杩囧害鏁版嵁鐨勪綆鐢靛钩 
    
    Coarse_delay_us(40);

    if(DHT11_READ_IO)                               //  姝ゆ椂濡傛灉杩樹负楂樼數骞冲垯鏁版嵁涓?1
    {
        while(DHT11_READ_IO);                       //  杩囧害鏁版嵁鐨勯珮鐢靛钩
        return 1;
    }   
    else                                            //  鑻ユ鏃朵负浣庡垯涓?0
    {
        return 0;
    }
}
 
/**
  * @brief  璇诲彇涓€涓瓧鑺傛暟鎹?1byte / 8bit
  * @param  void
  * @retval temp
  */
unsigned char DHT11_READ_BYTE(void)
{
    uint8_t i,temp = 0;                             //  鏆傛椂瀛樺偍鏁版嵁
    
    for(i=0; i<8 ;i++)
    {
        temp <<= 1;                                 
        if(DHT11_READ_BIT())                        //  1byte -> 8bit
        {
            temp |= 1;                              //  0000 0001
        }
    }
    return temp;
}
 
/**
  * @brief  璇诲彇娓╂箍搴︿紶鎰熷櫒鏁版嵁 5byte / 40bit
  * @param  void
  * @retval 0/1/2
  */
unsigned char DHT11_READ_DATA(void)
{
    uint8_t i;
    
    DHT11_START();                                  //  涓绘満鍙戦€佸惎鍔ㄤ俊鍙?
    
    if(DHT11_Check())                               //  濡傛灉DHT11搴旂瓟     
    {  
        while(!DHT11_READ_IO);                      //  杩囧害DHT11绛斿淇″彿鐨勪綆鐢靛钩
        while(DHT11_READ_IO);                       //  杩囧害DHT11绛斿淇″彿鐨勯珮鐢靛钩
        
        for(i=0; i<5; i++)
        {                        
            data[i] = DHT11_READ_BYTE();            //  璇诲彇 5byte
        }
        
        if(data[0] + data[1] + data[2] + data[3] == data[4])
        {
            //printf("褰撳墠婀垮害锛?d.%d%%RH褰撳墠娓╁害锛?d.%d掳C--",data[0],data[1],data[2],data[3]);
            return 1;                               //  鏁版嵁鏍￠獙閫氳繃
        }
        else
        {
            return 0;                               //  鏁版嵁鏍￠獙澶辫触
        }
    }
    else                                            //  濡傛灉DHT11涓嶅簲绛?
    {
        return 2;
    }
}
 
/**
  * @brief  妫€娴嬫俯婀垮害浼犳劅鍣ㄦ槸鍚﹀瓨鍦?妫€娴婦HT11鐨勫簲绛斾俊鍙?
  * @param  void
  * @retval 0/1
  */
unsigned char DHT11_Check(void)
{

    Coarse_delay_us(40);
    if(DHT11_READ_IO == 0)                          //  妫€娴嬪埌DHT11搴旂瓟
    {
        return 1;
    }
    else                                            //  妫€娴嬪埌DHT11涓嶅簲绛?
    {
        return 0;
    }
}
 
/**
  * @brief  璁剧疆寮曡剼妯″紡
  * @param  mode: 0->out, 1->in
  * @retval None
  */
static void DHT11_GPIO_MODE_SET(uint8_t mode)
{
    if(mode)
    {
        /*  杈撳叆  */
        GPIO_InitTypeDef GPIO_InitStruct;
        GPIO_InitStruct.Pin = GPIO_PIN_6;                   //  9鍙峰紩鑴?
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;             //  杈撳叆妯″紡
        GPIO_InitStruct.Pull = GPIO_PULLUP;                 //  涓婃媺杈撳叆
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    else 
    {
        /*  杈撳嚭  */
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.Pin = GPIO_PIN_6;                //  9鍙峰紩鑴?
        GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;      //  Push Pull 鎺ㄦ尳杈撳嚭妯″紡
        GPIO_InitStructure.Pull = GPIO_PULLUP;              //  涓婃媺杈撳嚭
        GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;    //  楂橀€?
        HAL_GPIO_Init(GPIOA,&GPIO_InitStructure);
    }
}

/**
  * @brief  绋嬪簭寤舵椂 us , 蹇呴』鍦?72M 涓婚涓嬩娇鐢?
  * @param  us: <= 4294967295
  * @retval None
  */
void Coarse_delay_us(uint32_t us)
{
    uint32_t delay = (HAL_RCC_GetHCLKFreq() / 4000000 * us);
    while (delay--)
	{
		;
	}
}
