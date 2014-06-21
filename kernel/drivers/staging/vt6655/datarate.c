

#include "ttype.h"
#include "tmacro.h"
#include "mac.h"
#include "80211mgr.h"
#include "bssdb.h"
#include "datarate.h"
#include "card.h"
#include "baseband.h"
#include "srom.h"

/*---------------------  Static Definitions -------------------------*/




/*---------------------  Static Classes  ----------------------------*/


 extern WORD TxRate_iwconfig; //2008-5-8 <add> by chester
/*---------------------  Static Variables  --------------------------*/
//static int          msglevel                =MSG_LEVEL_DEBUG;
static int          msglevel                =MSG_LEVEL_INFO;
const BYTE acbyIERate[MAX_RATE] =
{0x02, 0x04, 0x0B, 0x16, 0x0C, 0x12, 0x18, 0x24, 0x30, 0x48, 0x60, 0x6C};

#define AUTORATE_TXOK_CNT       0x0400
#define AUTORATE_TXFAIL_CNT     0x0064
#define AUTORATE_TIMEOUT        10

/*---------------------  Static Functions  --------------------------*/

void s_vResetCounter (
    PKnownNodeDB psNodeDBTable
    );



void
s_vResetCounter (
    PKnownNodeDB psNodeDBTable
    )
{
    BYTE            ii;

    // clear statistic counter for auto_rate
    for(ii=0;ii<=MAX_RATE;ii++) {
        psNodeDBTable->uTxOk[ii] = 0;
        psNodeDBTable->uTxFail[ii] = 0;
    }
}

/*---------------------  Export Variables  --------------------------*/


/*---------------------  Export Functions  --------------------------*/


BYTE
DATARATEbyGetRateIdx (
    BYTE byRate
    )
{
    BYTE    ii;

    //Erase basicRate flag.
    byRate = byRate & 0x7F;//0111 1111

    for (ii = 0; ii < MAX_RATE; ii ++) {
        if (acbyIERate[ii] == byRate)
            return ii;
    }
    return 0;
}



#define AUTORATE_TXCNT_THRESHOLD        20
#define AUTORATE_INC_THRESHOLD          30




WORD
wGetRateIdx(
    BYTE byRate
    )
{
    WORD    ii;

    //Erase basicRate flag.
    byRate = byRate & 0x7F;//0111 1111

    for (ii = 0; ii < MAX_RATE; ii ++) {
        if (acbyIERate[ii] == byRate)
            return ii;
    }
    return 0;
}

void
RATEvParseMaxRate (
    void *pDeviceHandler,
    PWLAN_IE_SUPP_RATES pItemRates,
    PWLAN_IE_SUPP_RATES pItemExtRates,
    BOOL bUpdateBasicRate,
    PWORD pwMaxBasicRate,
    PWORD pwMaxSuppRate,
    PWORD pwSuppRate,
    PBYTE pbyTopCCKRate,
    PBYTE pbyTopOFDMRate
    )
{
PSDevice  pDevice = (PSDevice) pDeviceHandler;
UINT  ii;
BYTE  byHighSuppRate = 0;
BYTE  byRate = 0;
WORD  wOldBasicRate = pDevice->wBasicRate;
UINT  uRateLen;


    if (pItemRates == NULL)
        return;

    *pwSuppRate = 0;
    uRateLen = pItemRates->len;

    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"ParseMaxRate Len: %d\n", uRateLen);
    if (pDevice->eCurrentPHYType != PHY_TYPE_11B) {
        if (uRateLen > WLAN_RATES_MAXLEN)
            uRateLen = WLAN_RATES_MAXLEN;
    } else {
        if (uRateLen > WLAN_RATES_MAXLEN_11B)
            uRateLen = WLAN_RATES_MAXLEN_11B;
    }

    for (ii = 0; ii < uRateLen; ii++) {
    	byRate = (BYTE)(pItemRates->abyRates[ii]);
        if (WLAN_MGMT_IS_BASICRATE(byRate) &&
            (bUpdateBasicRate == TRUE))  {
            // Add to basic rate set, update pDevice->byTopCCKBasicRate and pDevice->byTopOFDMBasicRate
            CARDbAddBasicRate((void *)pDevice, wGetRateIdx(byRate));
            DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"ParseMaxRate AddBasicRate: %d\n", wGetRateIdx(byRate));
        }
        byRate = (BYTE)(pItemRates->abyRates[ii]&0x7F);
        if (byHighSuppRate == 0)
            byHighSuppRate = byRate;
        if (byRate > byHighSuppRate)
            byHighSuppRate = byRate;
        *pwSuppRate |= (1<<wGetRateIdx(byRate));
    }
    if ((pItemExtRates != NULL) && (pItemExtRates->byElementID == WLAN_EID_EXTSUPP_RATES) &&
        (pDevice->eCurrentPHYType != PHY_TYPE_11B)) {

        UINT  uExtRateLen = pItemExtRates->len;

        if (uExtRateLen > WLAN_RATES_MAXLEN)
            uExtRateLen = WLAN_RATES_MAXLEN;

        for (ii = 0; ii < uExtRateLen ; ii++) {
            byRate = (BYTE)(pItemExtRates->abyRates[ii]);
            // select highest basic rate
            if (WLAN_MGMT_IS_BASICRATE(pItemExtRates->abyRates[ii])) {
            	// Add to basic rate set, update pDevice->byTopCCKBasicRate and pDevice->byTopOFDMBasicRate
                CARDbAddBasicRate((void *)pDevice, wGetRateIdx(byRate));
                DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"ParseMaxRate AddBasicRate: %d\n", wGetRateIdx(byRate));
            }
            byRate = (BYTE)(pItemExtRates->abyRates[ii]&0x7F);
            if (byHighSuppRate == 0)
                byHighSuppRate = byRate;
            if (byRate > byHighSuppRate)
                byHighSuppRate = byRate;
            *pwSuppRate |= (1<<wGetRateIdx(byRate));
            //DBG_PRN_GRP09(("ParseMaxRate : HighSuppRate: %d, %X\n", wGetRateIdx(byRate), byRate));
        }
    } //if(pItemExtRates != NULL)

    if ((pDevice->byPacketType == PK_TYPE_11GB) && CARDbIsOFDMinBasicRate((void *)pDevice)) {
        pDevice->byPacketType = PK_TYPE_11GA;
    }

    *pbyTopCCKRate = pDevice->byTopCCKBasicRate;
    *pbyTopOFDMRate = pDevice->byTopOFDMBasicRate;
    *pwMaxSuppRate = wGetRateIdx(byHighSuppRate);
    if ((pDevice->byPacketType==PK_TYPE_11B) || (pDevice->byPacketType==PK_TYPE_11GB))
       *pwMaxBasicRate = pDevice->byTopCCKBasicRate;
    else
       *pwMaxBasicRate = pDevice->byTopOFDMBasicRate;
    if (wOldBasicRate != pDevice->wBasicRate)
        CARDvSetRSPINF((void *)pDevice, pDevice->eCurrentPHYType);

     DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"Exit ParseMaxRate\n");
}


#define AUTORATE_TXCNT_THRESHOLD        20
#define AUTORATE_INC_THRESHOLD          30

void
RATEvTxRateFallBack (
    void *pDeviceHandler,
    PKnownNodeDB psNodeDBTable
    )
{
PSDevice        pDevice = (PSDevice) pDeviceHandler;
WORD            wIdxDownRate = 0;
UINT            ii;
//DWORD           dwRateTable[MAX_RATE]  = {1,   2,   5,   11,  6,    9,    12,   18,  24,  36,  48,  54};
BOOL            bAutoRate[MAX_RATE]    = {TRUE,TRUE,TRUE,TRUE,FALSE,FALSE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE};
DWORD           dwThroughputTbl[MAX_RATE] = {10, 20, 55, 110, 60, 90, 120, 180, 240, 360, 480, 540};
DWORD           dwThroughput = 0;
WORD            wIdxUpRate = 0;
DWORD           dwTxDiff = 0;

    if (pDevice->pMgmt->eScanState != WMAC_NO_SCANNING) {
        // Don't do Fallback when scanning Channel
        return;
    }

    psNodeDBTable->uTimeCount ++;

    if (psNodeDBTable->uTxFail[MAX_RATE] > psNodeDBTable->uTxOk[MAX_RATE])
        dwTxDiff = psNodeDBTable->uTxFail[MAX_RATE] - psNodeDBTable->uTxOk[MAX_RATE];

    if ((psNodeDBTable->uTxOk[MAX_RATE] < AUTORATE_TXOK_CNT) &&
        (dwTxDiff < AUTORATE_TXFAIL_CNT) &&
        (psNodeDBTable->uTimeCount < AUTORATE_TIMEOUT)) {
        return;
    }

    if (psNodeDBTable->uTimeCount >= AUTORATE_TIMEOUT) {
        psNodeDBTable->uTimeCount = 0;
    }


    for(ii=0;ii<MAX_RATE;ii++) {
        if (psNodeDBTable->wSuppRate & (0x0001<<ii)) {
            if (bAutoRate[ii] == TRUE) {
                wIdxUpRate = (WORD) ii;
            }
        } else {
            bAutoRate[ii] = FALSE;
        }
    }

    for(ii=0;ii<=psNodeDBTable->wTxDataRate;ii++) {
        if ( (psNodeDBTable->uTxOk[ii] != 0) ||
             (psNodeDBTable->uTxFail[ii] != 0) ) {
            dwThroughputTbl[ii] *= psNodeDBTable->uTxOk[ii];
            if (ii < RATE_11M) {
                psNodeDBTable->uTxFail[ii] *= 4;
            }
            dwThroughputTbl[ii] /= (psNodeDBTable->uTxOk[ii] + psNodeDBTable->uTxFail[ii]);
        }
//        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"Rate %d,Ok: %d, Fail:%d, Throughput:%d\n",
//                       ii, psNodeDBTable->uTxOk[ii], psNodeDBTable->uTxFail[ii], dwThroughputTbl[ii]);
    }
    dwThroughput = dwThroughputTbl[psNodeDBTable->wTxDataRate];

    wIdxDownRate = psNodeDBTable->wTxDataRate;
    for(ii = psNodeDBTable->wTxDataRate; ii > 0;) {
        ii--;
        if ( (dwThroughputTbl[ii] > dwThroughput) &&
             (bAutoRate[ii]==TRUE) ) {
            dwThroughput = dwThroughputTbl[ii];
            wIdxDownRate = (WORD) ii;
        }
    }
    psNodeDBTable->wTxDataRate = wIdxDownRate;
    if (psNodeDBTable->uTxOk[MAX_RATE]) {
        if (psNodeDBTable->uTxOk[MAX_RATE] >
           (psNodeDBTable->uTxFail[MAX_RATE] * 4) ) {
            psNodeDBTable->wTxDataRate = wIdxUpRate;
        }
    }else { // adhoc, if uTxOk =0 & uTxFail = 0
        if (psNodeDBTable->uTxFail[MAX_RATE] == 0)
            psNodeDBTable->wTxDataRate = wIdxUpRate;
    }
//2008-5-8 <add> by chester
TxRate_iwconfig=psNodeDBTable->wTxDataRate;
    s_vResetCounter(psNodeDBTable);
//    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"Rate: %d, U:%d, D:%d\n", psNodeDBTable->wTxDataRate, wIdxUpRate, wIdxDownRate);

    return;

}

BYTE
RATEuSetIE (
    PWLAN_IE_SUPP_RATES pSrcRates,
    PWLAN_IE_SUPP_RATES pDstRates,
    UINT                uRateLen
    )
{
    UINT ii, uu, uRateCnt = 0;

    if ((pSrcRates == NULL) || (pDstRates == NULL))
        return 0;

    if (pSrcRates->len == 0)
        return 0;

    for (ii = 0; ii < uRateLen; ii++) {
        for (uu = 0; uu < pSrcRates->len; uu++) {
            if ((pSrcRates->abyRates[uu] & 0x7F) == acbyIERate[ii]) {
                pDstRates->abyRates[uRateCnt ++] = pSrcRates->abyRates[uu];
                break;
            }
        }
    }
    return (BYTE)uRateCnt;
}
