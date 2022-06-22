////////////////////////////////////////////////////////////////////////////////
/// @file     WWDG.H
/// @author   YQ Liu
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE CONTAINS ALL THE FUNCTIONS PROTOTYPES FOR THE WWDG
///           EXAMPLES.
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
#ifndef __WWDG_H
#define __WWDG_H

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Example_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup EXAMPLE_WWDG
/// @brief WWDG example modules
/// @{


////////////////////////////////////////////////////////////////////////////////
/// @defgroup WWDG_Exported_Constants
/// @{

#define WATCHDOG_WWDG       0x70
#define RELOAD_WWDG         0x7F

/// @}


////////////////////////////////////////////////////////////////////////////////
/// @defgroup WWDG_Exported_Variables
/// @{
#ifdef _WWDG_C_
#define GLOBAL

GLOBAL bool mayDay = false;

#else
#define GLOBAL extern

GLOBAL bool mayDay;

#endif


#undef GLOBAL

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup WWDG_Exported_Functions
/// @{

/// @}


/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif /* WWDG_H */
       ////////////////////////////////////////////////////////////////////////////////
