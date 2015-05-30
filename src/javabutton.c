#include <PalmOS.h>
#include <PalmChars.h>
#include <SonyChars.h>
#include <jb_rscid.h>

static void playJavaSound() {
	MemHandle jvHandle;
	SndPtr jvPtr;
	
	jvHandle=DmGetResource('wave',JavaVoice);
	if(!jvHandle) { return; }
	jvPtr=MemHandleLock(jvHandle);
	SndPlayResource(jvPtr,1024,sndFlagAsync);
	MemHandleUnlock(jvHandle);
}

static void JavaFormResizeForm(FormType *frmP,RectangleType *fromBounds, RectangleType *toBounds) {
	Int16 heightDelta, widthDelta;
	UInt16 numObjects, i;
	heightDelta=(toBounds->extent.y-toBounds->topLeft.y)-(fromBounds->extent.y-fromBounds->topLeft.y);
	widthDelta=(toBounds->extent.x-toBounds->topLeft.x)-(fromBounds->extent.x-fromBounds->topLeft.x);
	numObjects=FrmGetNumberOfObjects(frmP);
	for(i=0; i<numObjects; i++) {
		Coord x,y;
		FrmGetObjectPosition(frmP,i,&x,&y);
		switch(FrmGetObjectType(frmP,i)) {
		case frmGraffitiStateObj:
			FrmSetObjectPosition(frmP,i,x+widthDelta,y+heightDelta);
			break;
		default:
			FrmSetObjectPosition(frmP,i,x+(widthDelta>>1),y+(heightDelta>>1));
			break;
		}
	}
}

static Boolean MainFormHandleEvent(EventPtr eventP) {
	Boolean handled = false;
	FormType *frmP;
	
	switch (eventP->eType) {
	case menuEvent: 
		if (eventP->data.menu.itemID == AboutMenuItm) {
			MenuEraseStatus(0);
			FrmAlert(AboutAlert);
			handled = true;
			break;
		}
		break;
	case frmOpenEvent:
		frmP = FrmGetActiveForm();
		FrmDrawForm(frmP);
		handled = true;
		break;
	case ctlSelectEvent:
		switch(eventP->data.ctlSelect.controlID) {
		case JavaButton:
			playJavaSound();
			handled=true;
			break;
		default:
			break;
		}
		break;
	case keyDownEvent:
		if(NavSelectPressed(eventP)) {
			playJavaSound();
			handled=true;
			break;
		}
		switch (eventP->data.keyDown.chr) {
		case vchrPageUp:
		case vchrJogUp:
		case vchrPageDown:
		case vchrJogDown:
		case vchrJogPush:
		case vchrJogPushedUp:
		case vchrJogPushedDown:
			playJavaSound();
			handled = true;
			break;
		}
		break;
	case winDisplayChangedEvent:
		{
			RectangleType curBounds,displayBounds;
			frmP=FrmGetActiveForm();
			WinGetBounds(FrmGetWindowHandle(frmP),&curBounds);
			WinGetBounds(WinGetDisplayWindow(), &displayBounds);
			JavaFormResizeForm(frmP,&curBounds,&displayBounds);
			FrmDrawForm(frmP);
			handled=true;
			break;
		}
	default:
		break;
  }
  return handled;
}

static Boolean dynamicInputAvail() {
	UInt32 version;
	Err err=FtrGet(pinCreator,pinFtrAPIVersion,&version);
	if(!err && version) {
		return true;
	} else {
		return false;
	}
}

static void setPalmDynInputParams(FormType *frmP) {
	Err err;
	Boolean dia=dynamicInputAvail();
	if(dia) {
		WinHandle frmH;
		err=FrmSetDIAPolicyAttr(frmP,frmDIAPolicyCustom);
		if(!err) {
			err=PINSetInputTriggerState(pinInputTriggerEnabled);
		}
		frmH=FrmGetWindowHandle(frmP);
		WinSetConstraintsSize(frmH,160,160,640,160,160,640);
	}
}

static void setSonyVirtSilkParams(frmP) {
	return;
}

static void dynInputOnMainFormLoad(FormType *frmP) {
	setPalmDynInputParams(frmP);
	setSonyVirtSilkParams(frmP);
}

static Boolean AppHandleEvent(EventPtr eventP) {
	Boolean handled = false;
	FormType *frmP;
	
	if (eventP->eType == frmLoadEvent) { /* Initialize and activate the form resource. */
		frmP = FrmInitForm(eventP->data.frmLoad.formID);

		FrmSetActiveForm(frmP);
		
		if (eventP->data.frmLoad.formID == MainForm) {
			dynInputOnMainFormLoad(frmP);
			FrmSetEventHandler(frmP, MainFormHandleEvent);
		}
		handled = true;
	} else if (eventP->eType == frmOpenEvent) { /* Load the form resource. */
		frmP = FrmGetActiveForm();
		FrmDrawForm(frmP);
		handled = true;
	} else if (eventP->eType == appStopEvent) { /* Unload the form resource. */
		frmP = FrmGetActiveForm();
		FrmEraseForm(frmP);
		FrmDeleteForm(frmP);
		handled = true;
	}
	
	return(handled);
}

static Boolean isTheDeviceSupported() {
	UInt32 version = 0;
	Err err;
	Boolean res=false;
	err=FtrGet(sysFileCSoundMgr,sndFtrIDVersion,&version);
	if(!err) {
		if(version) {
			return true;
		}
	}
	return res;
}

UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags) {
	EventType event;
	Err err = 0;

	switch (cmd) {
	case sysAppLaunchCmdNormalLaunch:
		if(isTheDeviceSupported() == false) {
			FrmAlert(OldAlert);
			break;
		}
		
		FrmGotoForm(MainForm);
		do {
			UInt16 MenuError;
			EvtGetEvent(&event, evtWaitForever);
			if (! SysHandleEvent(&event))
				if (! MenuHandleEvent(0, &event, &MenuError))
					if (! AppHandleEvent(&event))
						FrmDispatchEvent(&event);
		} while (event.eType != appStopEvent);
		break;
	case sysAppLaunchCmdNotify:
		if(((SysNotifyParamType*) cmdPBP)->notifyType==sysNotifyDisplayResizedEvent) {
			EventType resizedEvent;
			MemSet(&resizedEvent,sizeof(EventType),0);
			resizedEvent.eType=winDisplayChangedEvent;
			EvtAddUniqueEventToQueue(&resizedEvent,0,true);
		}
		break;
	default:
		break;
	}    
	return(err);
}
