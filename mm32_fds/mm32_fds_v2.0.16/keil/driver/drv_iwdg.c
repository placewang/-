////////////////////////////////////////////////////////////////////////////////
/// @file     DRV_IWDG.C
/// @author   YQ Liu
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE PROVIDES THE IWDG DRIVER LAYER FUNCTIONS.
////////////////////////////////////////////////////////////////////////////////
/// @attention
///
/// THE EXISTING FIRMWARE IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE
/// CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE
/// TIME. THEREFORE, MINDMOTION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR
/// CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH
/// HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN
/// CONNECTION WITH PRODUCTS MADE BY CUSTOMERS.
///
/// <H2><CENTER>&COPY; COPYRIGHT 2018-2019 MINDMOTION </CENTER></H2>
////////////////////////////////////////////////////////////////////////////////

// Define to prevent recursive inclusion  --------------------------------------
#define _DRV_IWDG_C_

// Files includes  -------------------------------------------------------------
#include <string.h>

#include "hal_iwdg.h"
#include "hal_rcc.h"


#include "hal_nvic.h"
#include "hal_exti.h"

#include "drv.h"
#include "common.h"
#include "drv_iwdg.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Driver_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup IWDG
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup IWDG_Exported_Functions
/// @{


#if defined(EX_IWDG)
////////////////////////////////////////////////////////////////////////////////
/// @brief  IWDG IRQHandler function.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
#if defined(__MM3O1) || defined(__MM0P1) || defined(__MM0Q1)
void WWDG_IWDG_IRQHandler(void)
{
    IWDG_ClearITPendingBit();
    if (instance.sync == emTYPE_Sync)
        ((fpIWDG)instance.sync)(void);
    else
        IWDG_irqStatus = true;
}
#endif
#endif


////////////////////////////////////////////////////////////////////////////////
/// @brief  IWDG overflow interrupt mode initialization.
/// @param  mode.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void DRV_IWDG_OverflowMode(bool mode)
{
#if defined(__MM3O1) || defined(__MM0P1) || defined(__MM0Q1)
    if (mode) {
        IWDG_irqStatus = false;
        IWDG_OverflowConfig(IWDG_Overflow_Interrupt);

        // IWDG Counter Overflow Interrupt

    u32 line;
    #if defined(__MM3O1)
        line    = EXTI_Line21;
    #endif
    #if defined(__MM0P1) || defined(__MM0Q1)
        line    = EXTI_Line24;
    #endif
        COMMON_ExitItFallingEnable(line);
        COMMON_NVIC_Configure(WWDG_IWDG_IRQn,  0,  3);
    }
    else
        IWDG_OverflowConfig(IWDG_Overflow_Reset);

#endif
}


////////////////////////////////////////////////////////////////////////////////
/// @brief  Config the instance of IWDG.
/// @param  pDcb: The pointer point to Device Control Block(DCB).
/// @param  idx: The trueIdx of the instance.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
static void InstanceConfig(tAPP_IWDG_DCB* pDcb, u8 idx)
{
    instance[idx].sync      = pDcb->sync;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Config the hardware of IWDG.
/// @param  pDcb: The pointer point to Device Control Block(DCB).
/// @param  idx: The trueIdx of the instance.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
static void HardwareConfig(tAPP_IWDG_DCB* pDcb, u8 idx)
{
//  tDRV_IWDG_DCB   dcb = {
//      .IWDGx  = (IWDG_TypeDef*)tbIpBase[tbSubHandleIdx[pDcb->hSub]],      //   instance->IWDGx,
//      .cnt    = pDcb->cnt,
//      .pres   = pDcb->pres,
//      .itEn   = pDcb->itEn
//  };
//
//  DVR_IWDG_Init(&dcb);

//  tDRV_IWDG_DCB   dcb = {
//      .IWDGx  = (IWDG_TypeDef*)tbIpBase[tbSubHandleIdx[pDcb->hSub]],      //   instance->IWDGx,
//      .cnt    = pDcb->cnt,
//      .pres   = pDcb->pres,
//      .itEn   = pDcb->itEn
//  };

    RCC_LSICmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

    // IWDG Prescaler
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(pDcb->pres);

    // Set IWDG Reload Value
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetReload(pDcb->cnt & 0xfff);

    DRV_IWDG_OverflowMode(pDcb->itEn);

    // Enable IWDG
    IWDG_ReloadCounter();
    IWDG_Enable();


}

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function opens the IWDG instance according to the specified
///         parameters in the IWDG_DCB_TypeDef
/// @param  pDcb: pointer to a IWDG_HandleTypeDef structure that contains
///                the configuration information for the specified IWDG module.
/// @retval  TRUE or FALSE
////////////////////////////////////////////////////////////////////////////////
static int IWDG_OpenFile(HANDLE handle, tAPP_IWDG_DCB* pDcb)
{
// Entry OpenFile Function
    s8 idx = DRV_OpenFileEntry((u32*)&tbIpBase, (u8*)&tbSubHandleIdx, pDcb->hSub, (u8*)(&instance[0]), sizeof(instance[0]), INSTANCE_NUM);
    if (idx == -1)      return 0;

// Instance configure
    InstanceConfig(pDcb, idx);

// Hardware configure
    HardwareConfig(pDcb, idx);

// Exit OpenFile Function
    return DRV_OpenFileExit(handle, (int*)&instance[idx], idx);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function closes the IWDG instance according to the ID
///          in the HANDLE.
/// @param  handle: pointer to a HANDLE structure that contains
///                the instance for the specified IWDG.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
static void IWDG_CloseFile(HANDLE handle)
{
    if (handle->idx == -1)      return;
    RCC_LSICmd(DISABLE);
    DRV_CloseFileCommon(&instance[handle->idx].sPrefix, sizeof(instance[0]));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  the IWDG read data according to the ID in the HANDLE.
/// @param  handle: pointer to a HANDLE structure that contains
///                the instance for the specified IWDG.
/// @param  ptr: Pointer to data buffer.
/// @param  count: Amount of data to be read.
/// @retval  sent data number.
////////////////////////////////////////////////////////////////////////////////
static int IWDG_ReadFile(HANDLE handle, s8 hSub, u8* ptr, u16 len)
{
    if (hSub == -1) return 0;
    int ret = (int)(IWDG_irqStatus);
    IWDG_irqStatus = false;
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Write data to IWDG and then get the processes data
/// @param  handle: pointer to HANDLE which contains the operation information
///         for the specified peripheral.
/// @param  ptr: pointer to input and output data
///         annote: input data and output data use the same memory address
/// @param  count: data length
/// @retval Process result
///         This parameter can be ERROR or SUCCESS
////////////////////////////////////////////////////////////////////////////////
static int IWDG_WriteFile(HANDLE handle, s8 hSub, u8* ptr, u16 len)
{
    if (hSub == -1) return 0;
    IWDG_ReloadCounter();           // 0xAAAA
    IWDG_Enable();
    return IWDG_GetReload();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  IWDG CreateFile
/// @param  handle: pointer to HANDLE which contains the operation information
///         for the specified peripheral.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void IWDG_CreateFile(HANDLE handle)
{
    static bool first = true;
    if (first) {
        first = false;
        memset(&instance[0], 0xFF, sizeof(instance[0]) * INSTANCE_NUM);
    }

    handle->instance    = (int*)&instance[0];
    handle->idx         = -1;
    handle->size        = sizeof(tbIpBase);
    handle->openFile    = (int*)&IWDG_OpenFile;
    handle->closeFile   = (int*)&IWDG_CloseFile;
    handle->readFile    = (int*)&IWDG_ReadFile;
    handle->writeFile   = (int*)&IWDG_WriteFile;
}




/// @}

/// @}

/// @}

