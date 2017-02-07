/* ram_000.h */
/*************************************************************************************************/
/* [@] 20151105 */ /*  DaoHVN : Create new */
/*************************************************************************************************/
#ifndef RAM_H
#define RAM_H

#include "..\Header\header.h"
#include "..\Predefine\Predefine.h"

/*** struct variable - need be back up ***/
typedef struct	_lst_glb_bk
{
	uchar8 CAN_V_Data[16][10]; 					/* 1 *//* CAN Data Channel V */
	uint16 RealSpeed; 							/* 2 *//* Real speed of vehicle */
} lst_glb_bk;

extern	lst_glb_bk vListBk, vListBk_z ;

/*** struct variable - do not need be back up ***/
typedef struct	_lst_glb
{
	uchar8 CAN_V_RX_DLC[16];					/* 1 *//* CAN CTRL1 DLC Resgiter channel V */
} lst_glb;

extern	lst_glb vList;


/*** Re-define struct variable - do not need be back up ***/
#define vUC8_CAN_V_RX_DLC				vList.CAN_V_RX_DLC				/* 1 *//* CAN CTRL1 DLC Resgiter channel V */


/*** Re-define struct variable - need be back up ***/
#define	vUC8_CAN_V_Data					vListBk.CAN_V_Data				/* 1 *//* CAN Data Channel V */


#endif /* ram.h */
