#include "doppler.h"
#include "main.h"
#include "cmsis_os.h"
typedef struct Node
{
  uint32_t value;
  struct Node* next;
  struct Node* previor;
}NodeDef,*pNodeDef;

extern osMessageQId myQueue01Handle;
/*
void DopplerPinDown(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,)
* ������Χ:


* ����ֵ˵��:void 
* ���ܽ���:   
  ������ѡ�ܽŵ�ƽ
*
* �ؼ���ע��
* - 

  
* δ�����:



*/ 
void DopplerPinDown(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  HAL_GPIO_WritePin(GPIOx,GPIO_Pin,GPIO_PIN_RESET);
}
/*
void DopplerPinUp(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,)
* ������Χ:


* ����ֵ˵��:void 
* ���ܽ���:   
  ������ѡ�ܽŵ�ƽ
*
* �ؼ���ע��
* - 

  
* δ�����:



*/ 
void DopplerPinUp(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  HAL_GPIO_WritePin(GPIOx,GPIO_Pin,GPIO_PIN_SET);
}

//��������������˸���ƺ���
void DopplerEnergyFlashing(uint32_t level)
{
  static uint32_t FlashingInterval = 0;
  static uint8_t FlashingFlag = 0;
  if(FlashingInterval >= level)
  {
    FlashingInterval = 0;
    if(FlashingFlag == 0)
    {
      DopplerPinUp(SIGNAL_LED_GPIO_Port,SIGNAL_LED_Pin);
      FlashingFlag = 1;
    }
    else
    {
      DopplerPinDown(SIGNAL_LED_GPIO_Port,SIGNAL_LED_Pin);
      FlashingFlag = 0;
    }
  }
  FlashingInterval++;
}
//������ֵ����Сֵ������ADC�ɼ����ݵ�ǰʮ����������
void FillLinkList(pNodeDef HeadNode,uint32_t* buffer)
{
  pNodeDef pNode_1 = pvPortMalloc(sizeof(NodeDef));
  pNodeDef pNode_2 = NULL;
  uint32_t i = 0;
  
  
  pNode_1->value = buffer[0];
  pNode_1->next = HeadNode;
  pNode_1->previor = HeadNode;
  
  HeadNode->next = pNode_1;
  HeadNode->previor = pNode_1;
  pNode_2 = pNode_1;
  //��һ������ADC�ɼ�������ѡʮ������������Сֵ����
  for(i = 0;i < 9; i++)
  {
    pNode_1 = pvPortMalloc(sizeof(NodeDef));    
    if(pNode_1 != NULL)
    {
      pNode_1->value = buffer[i+1];
      pNode_1->next = HeadNode->next;
      pNode_1->previor = HeadNode;
      HeadNode->next = pNode_1;
      pNode_2->previor = pNode_1;
      pNode_2 = pNode_1;
    }
  }
}
//���뷨��ð�ݷ���ǰʮ�����ݽ�������
void InsertSort(pNodeDef HeadNode,bool8 IsAscendingSort)
{
  pNodeDef pNode_1 = NULL;
  pNodeDef pNode_2 = NULL;
  pNodeDef pNode_3 = NULL;
  pNodeDef pNode_4 = NULL;
  uint8_t linklistlength = 0;
  uint8_t CompareCounter = 0;
  
  if(IsAscendingSort == TRUE)
  {
    pNode_1 = HeadNode->next;
    pNode_2 = pNode_1->next;
    
    while(pNode_2 != HeadNode)
    {
      if(pNode_1->value > pNode_2->value)
      {
        //����ͷ��Ϊ������
        HeadNode->value = pNode_2->value;
        //��ǰ�ȽϽڵ�ָ��
        pNode_3 = pNode_1;
        pNode_4 = pNode_2;
        while(HeadNode->value < pNode_3->value)
        {
          //���ϴ�ֵ����
          pNode_4->value = pNode_3->value;
          //��ǰ�ȽϽڵ�ָ��ǰ��
          pNode_3 = pNode_3->previor;
          pNode_4 = pNode_4->previor;
        }
        pNode_4->value = HeadNode->value;
      }
      pNode_1 = pNode_1->next;
      pNode_2 = pNode_2->next;
    }
  }
  else if(IsAscendingSort == FALSE)
  {
    //bubbling sort
    for(linklistlength = 0;linklistlength < 9;linklistlength++)
    {
      pNode_1 = HeadNode->next;
      pNode_2 = pNode_1->next;
      for(CompareCounter = 0;CompareCounter < 9 - linklistlength;CompareCounter++)
      {
        if(pNode_1->value < pNode_2->value)
        {
          HeadNode->value = pNode_2->value;
          pNode_2->value = pNode_1->value;
          pNode_1->value = HeadNode->value;
        }
        pNode_1 = pNode_1->next;
        pNode_2 = pNode_2->next;
      }
    }
    
  }
}
//free����������������ڴ�
void LinkListFree(pNodeDef HeadNode)
{
  pNodeDef pNode_1 = NULL;
  pNodeDef pNode_2 = NULL;
  
  pNode_1 = HeadNode->next;
  pNode_2 = pNode_1->next;
  while(pNode_1 != HeadNode)
  {
    HeadNode->next = pNode_2;
    pNode_2->previor = HeadNode;
    vPortFree(pNode_1);
    pNode_1 = pNode_2;
    pNode_2 = pNode_2->next;
  }
  vPortFree(HeadNode);
}
//ɸѡ��ADC�ɼ��������������е����ֵ����Сֵ
void Filtrate(pNodeDef HeadNode,uint32_t *Buffer,
              uint32_t FiltrateLength,uint32_t StartIndex,bool8 IsMinimun)
{
  pNodeDef pNodeData = NULL;
  pNodeDef pNode_1 = NULL;
  pNodeDef pNode_2 = NULL;
  pNodeDef pNode_3 = NULL;
  pNodeDef pNode_4 = NULL;
  bool8 IsInsert = FALSE;
  uint32_t i = 0;
  if(IsMinimun == TRUE)
  {
    for(i = StartIndex;i < FiltrateLength; i++)
    {
      pNodeData = pvPortMalloc(sizeof(NodeDef));
      pNodeData->value = Buffer[i];
      pNode_1 = HeadNode;
      pNode_2 = pNode_1->next;
      IsInsert = FALSE;
      while(pNode_2 != HeadNode)
      {
        if(pNodeData->value < pNode_2->value)
        {
          pNode_1->next = pNodeData;
          pNodeData->previor = pNode_1;
          pNodeData->next = pNode_2;
          pNode_2->previor = pNodeData;
          pNode_3 = HeadNode->previor;
          pNode_4 = pNode_3->previor;
          pNode_4->next = HeadNode;
          HeadNode->previor = pNode_4;
          //���ݲ���������ĩβ����free��
          vPortFree(pNode_3);
          IsInsert = TRUE;
          break;
        }
        else
        {
          pNode_1 = pNode_1->next;
          pNode_2 = pNode_2->next;
        }
      }
      //û�в����������ڵ�free��
      if(IsInsert == FALSE)
      {
        vPortFree(pNodeData);
      }
    }
  }
  else  if(IsMinimun == FALSE)
  {
    for(i = StartIndex;i < FiltrateLength; i++)
    {
      pNodeData = pvPortMalloc(sizeof(NodeDef));
      pNodeData->value = Buffer[i];
      pNode_1 = HeadNode;
      pNode_2 = pNode_1->next;
      IsInsert = FALSE;
      while(pNode_2 != HeadNode)
      {
        if(pNodeData->value > pNode_2->value)
        {
          pNode_1->next = pNodeData;
          pNodeData->previor = pNode_1;
          pNodeData->next = pNode_2;
          pNode_2->previor = pNodeData;
          pNode_3 = HeadNode->previor;
          pNode_4 = pNode_3->previor;
          pNode_4->next = HeadNode;
          HeadNode->previor = pNode_4;
          //���ݲ���������ĩβ����free��
          vPortFree(pNode_3);
          IsInsert = TRUE;
          break;
        }
        else
        {
          pNode_1 = pNode_1->next;
          pNode_2 = pNode_2->next;
        }
      }
      //û�в����������ڵ�free��
      if(IsInsert == FALSE)
      {
        vPortFree(pNodeData);
      }
    }
  }
}
//��������ƽ��ֵ����
float LinkListAverageValueGet(pNodeDef HeadNode)
{
  pNodeDef pNode_1 = NULL;
  float average = 0,i = 0;
  
  pNode_1 = HeadNode->next;
  while(pNode_1 != HeadNode)
  {
    average += (float)pNode_1->value;
    i++;
    pNode_1 = pNode_1->next;
  }
  average /= i;
  return average;
}
//�������ɼ����ݷ�������
float SensorDataHandle()
{  
  float AverageMax = 0;
  float AverageMin = 0;
  float energy = 0;
  pNodeDef FrontMin = pvPortMalloc(sizeof(NodeDef));
  pNodeDef FrontMax = pvPortMalloc(sizeof(NodeDef));
  
  FrontMin->next = NULL;
  FrontMin->previor = NULL;
  
  FillLinkList(FrontMin,SensorResultDMA);
  FillLinkList(FrontMax,SensorResultDMA);
  //��������
  InsertSort(FrontMin,TRUE);
  InsertSort(FrontMax,FALSE);
  //ɸѡ
  Filtrate(FrontMin,SensorResultDMA,DMA_BUFF_SIZE,10,TRUE);
  Filtrate(FrontMax,SensorResultDMA,DMA_BUFF_SIZE,10,FALSE);
  //����ƽ��ֵ
  AverageMax = LinkListAverageValueGet(FrontMax);
  AverageMin = LinkListAverageValueGet(FrontMin);
  energy = AverageMax - AverageMin;
  //�黹�ڴ�
  LinkListFree(FrontMin);
  LinkListFree(FrontMax);
  return energy;
}
//LED��˸Ƶ�ʻ�ȡ����
uint32_t LedFlashingFrequencyGet(uint32_t Energy)
{
  uint32_t value = 0;
  if(Energy < 100)
  {
    value = 300;
  }
  else if((Energy >= 100)&&(Energy < 500))
  {
    value = 240;
  }
  else if((Energy >= 1000)&&(Energy < 1500))
  {
    value = 200;
  }
  else if((Energy >= 1500)&&(Energy < 2000))
  {
    value = 160;
  }
  else if((Energy >= 2000)&&(Energy < 2500))
  {
    value = 120;
  }
  else if((Energy >= 2500)&&(Energy < 3000))
  {
    value = 100;
  }
  else if((Energy >= 3000)&&(Energy < 3500))
  {
    value = 80;
  }
  else if((Energy >= 3500)&&(Energy < 4000))
  {
    value = 60;
  }
  else
  {
    value = 40;
  }
  return value;
}



