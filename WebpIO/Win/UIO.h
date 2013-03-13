#pragma  once
#include "Config.h"

#ifdef AE_OS_WIN
#include <windows.h>
#endif

#include "AE_IO.h"
#include "AEConfig.h"
#include "AE_Macros.h"
#include "AE_EffectCBSuites.h"
#include "AEGP_SuiteHandler.h"
#include "AE_GeneralPlug.h"
#include "AE_Effect.h"
#include "AE_EffectUI.h"
#include "AE_EffectCB.h"
#include "AE_AdvEffectSuites.h"
#include "entry.h"

extern "C"{
DllExport A_Err
EntryPointFunc(
	struct SPBasicSuite		*pica_basicP,			/* >> */
	A_long				 	major_versionL,			/* >> */		
	A_long					minor_versionL,			/* >> */		
	const A_char		 	*file_pathZ,				/* >> platform-specific delimiters */
	const A_char		 	*res_pathZ,				/* >> platform-specific delimiters */
	AEGP_PluginID			aegp_plugin_id,			/* >> */
	void					*global_refconPV);			/* << */
};