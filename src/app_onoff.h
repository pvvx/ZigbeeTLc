#ifndef _INCLUDE_APP_ONOFF_H_
#define _INCLUDE_APP_ONOFF_H_

/**
 *  @brief Defined for on/off cluster attributes
 */
typedef struct{
//	u16	 onTime;
//	u16	 offWaitTime;
	u8	 startUpOnOff;
	bool onOff;
//	bool globalSceneControl;
}zcl_onOffAttr_t;

extern zcl_onOffAttr_t g_zcl_onOffAttrs;

/**
 *  @brief Defined for saving on/off attributes
 */
typedef struct _attribute_packed_{
	u8	onOff;
	u8	startUpOnOff;
}zcl_nv_onOff_t;

void cmdOnOff_set(bool status);
void remoteCmdOnOff(u8 srcEp, u8 cmd);
status_t app_onOffCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
void zcl_onoffConfig_save(void);

#endif /* _INCLUDE_APP_ONOFF_H_ */
