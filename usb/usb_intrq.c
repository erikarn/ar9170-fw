#include "dt_defs.h"
#include "gv_extr.h"
#include "string.h"
#include "intrq_defs.h"

/************************************************************************/
/*                                                                      */
/*    FUNCTION DESCRIPTION                  zfPushIntrINQ               */
/*      Insert data into Interrupt IN Queue                             */
/*                                                                      */
/*    ROUTINES CALLED                                                   */
/*                                                                      */
/*    INPUTS                                                            */
/*      buffer: buffer to copy into Interrupt IN Queue                  */
/*      len: length needed to be copied                                 */
/*                                                                      */
/*    OUTPUTS                                                           */
/*      0: Success                                                      */
/*      Other: Error code                                               */
/*                                                                      */
/*    AUTHOR                                                            */
/*      Yuan-Gu Wei   ZyDAS Communication Corporation        2007.2     */
/*                                                                      */
/************************************************************************/
u8_t *zfGetFreeIntrINQTailBuf(void)
{
	u8_t *buf = NULL;

	if (((zgIntrINQTail + 1) & (ZM_INTRQ_NUM - 1)) != zgIntrINQHead) {
		buf = (u8_t *) zgIntrINQ[zgIntrINQTail];

		zgIntrINQTail += 1;
		zgIntrINQTail &= (ZM_INTRQ_NUM - 1);
		zgIntrINQNum++;
	}

	return buf;
}

/************************************************************************/
/*                                                                      */
/*    FUNCTION DESCRIPTION                  zfPushIntrINQ               */
/*      Pop up data from Interrupt IN Queue to USB Response buffer      */
/*                                                                      */
/*    ROUTINES CALLED                                                   */
/*                                                                      */
/*    INPUTS                                                            */
/*      void                                                            */
/*                                                                      */
/*    OUTPUTS                                                           */
/*      void                                                            */
/*                                                                      */
/*    AUTHOR                                                            */
/*      Yuan-Gu Wei   ZyDAS Communication Corporation        2007.2     */
/*                                                                      */
/************************************************************************/
u8_t *zfGetIntrINQHeadBuf(void)
{
	u8_t *buf = NULL;

//    if (zgIntrINQNum != 0)
	if (zgIntrINQNum > 0) {
		buf = (u8_t *) zgIntrINQ[zgIntrINQHead];
		zgIntrINQHead += 1;
		zgIntrINQHead &= ZM_INTRQ_NUM - 1;
		zgIntrINQNum--;
	}

	return buf;
}
