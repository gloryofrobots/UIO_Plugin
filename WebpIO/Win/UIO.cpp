#	include "UIO.h"
#	include "Plugin.h"

static AEGP_Command	S_import_cmd = 0;

static SPBasicSuite	*S_PicaBasic_SuiteP = 0;

static	AEGP_PluginID S_mem_id = 0;

#define LOG(msg)   UIO_Plugin::getPlugin()->getLogSystem()->log(msg)

#ifdef DEBUG
	#define LOG_FUNCTION_NAME() \
	LOG( __FUNCTION__ ); \
	LOG("-------------------------------------------------"); 
#else
	#define LOG_FUNCTION_NAME() ;
#endif

static A_Err AddToFileToProject( const SPBasicSuite * suite, const UIO_Plugin::String & _path )
{
	A_Err 				err = A_Err_NONE;
	AEGP_SuiteHandler suites(suite);


	AEGP_ProjectH		projH			= NULL;

	AEGP_ItemH			
		root_itemH		= NULL, 
		new_folderH		= NULL;


	A_char	nameZ[AEGP_MAX_PROJ_NAME_SIZE] = {'\0'},
		pathZ[AEGP_MAX_PROJ_NAME_SIZE] = {'\0'};

	// First, make a folder to contain everything.

	ERR(suites.ProjSuite3()->AEGP_GetProjectByIndex(0, &projH));
	ERR(suites.ProjSuite3()->AEGP_GetProjectRootFolder(projH, &root_itemH));
	ERR(suites.ProjSuite3()->AEGP_GetProjectName(projH, nameZ));
	ERR(suites.ProjSuite3()->AEGP_GetProjectPath(projH, pathZ));

	AEGP_ItemH			footage_itemH	= NULL;
	AEGP_FootageH		footageH		= NULL;
	AEGP_FootageLayerKey key;

	suites.ANSICallbacksSuite1()->strcpy(key.nameAC, _path.c_str());
	ERR( suites.FootageSuite3()->AEGP_NewFootage(
		S_mem_id, 
		_path.c_str(), 
		&key,
		NULL,
		FALSE, 
		NULL, 
		&footageH));

	ERR(suites.FootageSuite3()->AEGP_AddFootageToProject(footageH, root_itemH, &footage_itemH));

	return err;
}

static A_Err ImportWebPFile( const SPBasicSuite * suite, UIO_Plugin::Platform * _platform )
{
	A_Err err = A_Err_NONE ,err2 = A_Err_NONE;

	AEGP_SuiteHandler suites(suite);

	char buffer[260];

	if ( _platform->openFileDialog(buffer) == false  )
	{
		return A_Err_GENERIC;
	}

	UIO_Plugin::String path(buffer);

	err = AddToFileToProject( suite, path );

	return err;
}

/////////////////////////////////////////////////////

static A_Err DeathHook(	
	AEGP_GlobalRefcon unused1 ,
	AEGP_DeathRefcon unused2)
{
	try
	{
		UIO_Plugin::destroyPlugin();
	}
	catch(...)
	{
		return A_Err_GENERIC;
	}
	return A_Err_NONE;
}

A_Err UpdateMenuHook(
	AEGP_GlobalRefcon		plugin_refconPV,		/* >> */
	AEGP_UpdateMenuRefcon	refconPV,				/* >> */
	AEGP_WindowType			active_window)			/* >> */
{
	A_Err 				err = A_Err_NONE;
	AEGP_SuiteHandler	suites(S_PicaBasic_SuiteP);

	if (S_import_cmd) {
		err = suites.CommandSuite1()->AEGP_EnableCommand(S_import_cmd);
	}

	return err;
}


static A_Err ShowDialog()
{
	A_Err 				err = A_Err_NONE;
	
	ERR( ImportWebPFile( S_PicaBasic_SuiteP, UIO_Plugin::getPlugin()->getPlatform() ));
		
	return err;
}

A_Err 	CommandHook(
	AEGP_GlobalRefcon	plugin_refconPV,		/* >> */
	AEGP_CommandRefcon	refconPV,				/* >> */
	AEGP_Command		command,				/* >> */
	AEGP_HookPriority	hook_priority,			/* >> */
	A_Boolean			already_handledB,		/* >> */
	A_Boolean			*handledPB)				/* << */
{
	A_Err 				err = A_Err_NONE;
	AEGP_SuiteHandler	suites(S_PicaBasic_SuiteP);

	*handledPB = FALSE;
	if ( (command == S_import_cmd) )
	{
		err = ShowDialog();
		*handledPB = TRUE;
	}
	else
	{
		return err;
	}
	return err;
}


static A_Err
PretendToReadFileHeader(
						AEIO_BasicData	*basic_dataP,
						UIO_Plugin_Header	*file,
						const char * file_path )
{
	LOG_FUNCTION_NAME();
	A_Err err = A_Err_NONE;
	AEGP_SuiteHandler suites(basic_dataP->pica_basicP);
	
	if (file_path != NULL )	{
		suites.ANSICallbacksSuite1()->strcpy(file->name, file_path);
	} else {
		err = A_Err_GENERIC;
		return err;
	}

	if ( UIO_Plugin::pushNewHeader( file ) == false )
	{
		return A_Err_GENERIC;
	}
	
	return err;
}


static A_Err	
UIO_InitInSpecFromFile(
	AEIO_BasicData	*basic_dataP,
	const A_char	*file_pathZ,
	AEIO_InSpecH	specH)
{ 
	/*	Read the file referenced by the path. Use the 
		file information to set all fields of the AEIO_InSpecH.
	*/
	
	A_Err err						=	A_Err_NONE,
		  err2						=	A_Err_NONE;

	AEIO_Handle		optionsH		=	NULL,
					old_optionsH	=	NULL;
	UIO_Plugin_Header	header;

	A_Fixed			native_fps		=	FLOAT_2_FIX(29.97);		

	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);

	AEFX_CLR_STRUCT(header);

	//suites.ANSICallbacksSuite1()->strcpy(S_path, file_pathZ);
		
	ERR(PretendToReadFileHeader(basic_dataP, &header, file_pathZ));

	if (!err) {
		UIO_Plugin_Header	*new_headerP 	=	NULL;

		/*		What are we doing here? 

			+	Allocate a new OptionsH to hold our file header info.
			+	Lock it in memory, copy our local header into the OptionsH.
			+	If there's an old OptionsH, get rid of it. 
			+	Unlock handle so AE can move it at will.
		
		*/
		
		ERR(suites.MemorySuite1()->AEGP_NewMemHandle(	S_mem_id,
														"SDK_IO optionsH", 
														sizeof(UIO_Plugin_Header), 
														AEGP_MemFlag_CLEAR, 
														&optionsH));
													
		if (optionsH){
			ERR(suites.MemorySuite1()->AEGP_LockMemHandle(optionsH, reinterpret_cast<void **>(&new_headerP)));
		}	
		if (new_headerP){
			
			*new_headerP = header;

			ERR(suites.IOInSuite1()->AEGP_SetInSpecOptionsHandle(	specH, 
																	optionsH, 
																	reinterpret_cast<void **>(&old_optionsH)));
			
			//	Do NOT free the old options handle. There is old code
			//	in the bowels of AE that does a flat (bytecopy) of the 
			//	input to output OptionsH in the case of a sync.
			/*
			if (old_optionsH){
				ERR(suites.MemorySuite1()->AEGP_FreeMemHandle(old_optionsH));
			}*/
		}
				
		/*	
			Set specH information based on what we (pretended to) read from the file.
		*/
		
		ERR(suites.IOInSuite1()->AEGP_SetInSpecDepth(specH, new_headerP->bitdepthS)); // always 32 bits for .webp files
		ERR(suites.IOInSuite1()->AEGP_SetInSpecDuration(specH, &(new_headerP->durationT)));
		ERR(suites.IOInSuite1()->AEGP_SetInSpecDimensions(	specH, 
															static_cast<A_long>(new_headerP->widthLu),
															static_cast<A_long>(new_headerP->heightLu)));

		ERR(suites.IOInSuite1()->AEGP_SetInSpecNativeFPS(specH, native_fps));

		if (!err){
			AEIO_AlphaLabel	alpha;
			AEFX_CLR_STRUCT(alpha);
			
			alpha.alpha		=	AEIO_Alpha_STRAIGHT;
			alpha.flags		=	AEIO_AlphaPremul;
			alpha.version	=	AEIO_AlphaLabel_VERSION;
			alpha.red		=	0;
			alpha.green		=	0;
			alpha.blue		=	0;
			
			err = suites.IOInSuite1()->AEGP_SetInSpecAlphaLabel(specH, &alpha);
		}

		ERR2(suites.MemorySuite1()->AEGP_UnlockMemHandle(optionsH));
	}


	return err;
}


static A_Err
UIO_DisposeInSpec(
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH	specH)
{ 
	LOG_FUNCTION_NAME();
	A_Err				err			=	A_Err_NONE; 
	AEIO_Handle			optionsH	=	NULL;
	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);	
	
	ERR(suites.IOInSuite1()->AEGP_GetInSpecOptionsHandle(specH, reinterpret_cast<void**>(&optionsH)));

	if (optionsH) {
		ERR(suites.MemorySuite1()->AEGP_FreeMemHandle(optionsH));
	}
	return err;
};

static A_Err
UIO_FlattenOptions(
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH	specH,
	AEIO_Handle		*flat_optionsPH)
{ 
	LOG_FUNCTION_NAME();
	A_Err	err		=	A_Err_NONE,
			err2	=	A_Err_NONE; 

	AEIO_Handle			optionsH	= NULL;
	
	UIO_Plugin_Header		*flat_headerP	= NULL,
						*old_headerP	= NULL;

	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);	
	
	//	Given the AEIO_InSpecH, acquire the non-flat
	//	options handle and use it to create a flat
	//	version. Do NOT de-allocate the non-flat
	//	options handle!
	
	ERR(suites.IOInSuite1()->AEGP_GetInSpecOptionsHandle(specH, reinterpret_cast<void**>(&optionsH)));

	if (optionsH) {
		ERR(suites.MemorySuite1()->AEGP_LockMemHandle(optionsH, reinterpret_cast<void**>(&old_headerP)));
	}
			
	if (old_headerP){
		ERR(suites.MemorySuite1()->AEGP_NewMemHandle(	S_mem_id,
														"flattened_options", 
														sizeof(UIO_Plugin_Header), 
														AEGP_MemFlag_CLEAR, 
														flat_optionsPH));
	}

	if (*flat_optionsPH){
		ERR(suites.MemorySuite1()->AEGP_LockMemHandle(*flat_optionsPH, (void**)&flat_headerP));
	}			
	if (!err && flat_headerP) {
		*flat_headerP = *old_headerP;
	}

	ERR2(suites.MemorySuite1()->AEGP_UnlockMemHandle(optionsH));
	ERR2(suites.MemorySuite1()->AEGP_UnlockMemHandle(*flat_optionsPH));

	return err;
};		

static A_Err
UIO_InflateOptions(
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH	specH,
	AEIO_Handle		flat_optionsH)
{ 
		A_Err	err		=	A_Err_NONE,
		err2	=	A_Err_NONE; 
	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);	
	LOG_FUNCTION_NAME();
	/*	During this function, 
	
		+	Create a non-flat options structure, containing the info from the 
			flat_optionsH you're passed.
		
		+	Set the options for the InSpecH using AEGP_SetInSpecOptionsHandle().
	*/
	A_Fixed			native_fps		=	FLOAT_2_FIX(29.97);
	AEIO_Handle			optionsH	= NULL
		,old_optionsH	=	NULL;
	
	UIO_Plugin_Header		*flat_headerP	= NULL,
			*nonflat_headerP	= NULL;

	if (flat_optionsH){
		ERR(suites.MemorySuite1()->AEGP_LockMemHandle(flat_optionsH, (void**)&flat_headerP));
	}

	if (!err && flat_headerP) {
		UIO_Plugin_Header	*new_headerP 	=	NULL;

		ERR(suites.MemorySuite1()->AEGP_NewMemHandle(	UIO_Plugin::getPlugin()->getId(),
														"SDK_IO optionsH", 
														sizeof(UIO_Plugin_Header), 
														AEGP_MemFlag_CLEAR, 
														&optionsH));													
		if (optionsH){
			ERR(suites.MemorySuite1()->AEGP_LockMemHandle(optionsH, reinterpret_cast<void **>(&new_headerP)));
		}	

		if (new_headerP){
				
			*new_headerP = *flat_headerP;
			if ( UIO_Plugin::pushNewHeader( new_headerP ) == false )
			{
				return A_Err_GENERIC;
			}

			ERR(suites.IOInSuite1()->AEGP_SetInSpecOptionsHandle(	specH, 
																	optionsH, 
																	reinterpret_cast<void **>(&old_optionsH)));
		}	
		
		ERR(suites.IOInSuite1()->AEGP_SetInSpecDepth(specH, new_headerP->bitdepthS)); // always 32 bits
		ERR(suites.IOInSuite1()->AEGP_SetInSpecDuration(specH, &(new_headerP->durationT)));
		ERR(suites.IOInSuite1()->AEGP_SetInSpecDimensions(	specH, 
															static_cast<A_long>(new_headerP->widthLu),
															static_cast<A_long>(new_headerP->heightLu)));
		
		ERR(suites.IOInSuite1()->AEGP_SetInSpecNativeFPS(specH, native_fps));
	}

	ERR2(suites.MemorySuite1()->AEGP_UnlockMemHandle(flat_optionsH));
	return A_Err_NONE; 
};		

static A_Err
UIO_SynchInSpec(
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH	specH, 
	A_Boolean		*changed0)
{ 
	LOG_FUNCTION_NAME();
	return AEIO_Err_USE_DFLT_CALLBACK;
};

static A_Err	
UIO_GetActiveExtent(
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH	specH,				/* >> */
	const A_Time	*tr,				/* >> */
	A_LRect			*extent)			/* << */
{ 
	LOG_FUNCTION_NAME();
	return AEIO_Err_USE_DFLT_CALLBACK; 
};		

static A_Err	
UIO_GetInSpecInfo(
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH	specH, 
	AEIO_Verbiage	*verbiageP)
{ 
	LOG_FUNCTION_NAME();
	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);
	
	suites.ANSICallbacksSuite1()->strcpy(verbiageP->name, "Magic Particles");
	suites.ANSICallbacksSuite1()->strcpy(verbiageP->type, "Astralax Magic Particles File");
	suites.ANSICallbacksSuite1()->strcpy(verbiageP->sub_type, "no particular subtype");

	return A_Err_NONE;
};
//#define DEBUG_DECODE 


static A_Err	
UIO_DrawSparseFrame(
	AEIO_BasicData					*basic_dataP,
	AEIO_InSpecH					specH, 
	const AEIO_DrawSparseFramePB	*sparse_framePPB, 
	PF_EffectWorld					*wP,
	AEIO_DrawingFlags				*draw_flagsP)
{ 
	LOG_FUNCTION_NAME();

	A_Err	err		=	A_Err_NONE;

	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);
	A_Rect				rectR	=	{0, 0, 0, 0};   
	AEIO_Handle			optionsH	= NULL;
    UIO_Plugin_Header * old_headerP;
	//	If the sparse_frame required rect is NOT all zeroes,
	//	use it. Otherwise, just blit the whole thing.
	if (!(sparse_framePPB->required_region.top	==
		sparse_framePPB->required_region.left	==
		sparse_framePPB->required_region.bottom ==
		sparse_framePPB->required_region.right))
	{
		rectR.top		= sparse_framePPB->required_region.top;
		rectR.bottom	= sparse_framePPB->required_region.bottom;
		rectR.left		= sparse_framePPB->required_region.left;
		rectR.right		= sparse_framePPB->required_region.right;
	}
	
	A_char	file_nameZ[AEGP_MAX_PATH_SIZE];
	
	ERR(suites.IOInSuite1()->AEGP_GetInSpecOptionsHandle(specH, reinterpret_cast<void**>(&optionsH)));

	if (optionsH)
	{
		ERR(suites.MemorySuite1()->AEGP_LockMemHandle(optionsH, reinterpret_cast<void**>(&old_headerP)));
	}

	ERR(suites.MemorySuite1()->AEGP_UnlockMemHandle(optionsH));

	if(err != A_Err_NONE )
	{
		return err;
	}

	suites.IOInSuite1()->AEGP_GetInSpecFilePath(specH, file_nameZ);

	UIO_Plugin::Frame frame;

	frame.options.height = wP->height;
	frame.options.width = wP->width;
	frame.options.pitch = wP->rowbytes;
	frame.options.time = ( sparse_framePPB->tr.value / sparse_framePPB->tr.scale ) * 1000;
	frame.options.scale = false;

	if( old_headerP->widthLu !=  wP->width || old_headerP->heightLu != wP->height )
	{
		frame.options.scale = true;
	}

	frame.data = (unsigned char * )wP->data;
	frame.size = wP->rowbytes * wP->height;

	if ( UIO_Plugin::fillFrame( &frame, old_headerP ) == false )
	{
		return A_Err_GENERIC;
	}

	return A_Err_NONE;
};

static A_Err	
UIO_GetDimensions(
	AEIO_BasicData			*basic_dataP,
	AEIO_InSpecH			 specH, 
	const AEIO_RationalScale *rs0,
	A_long					 *width0, 
	A_long					 *height0)
{ 
	LOG_FUNCTION_NAME();
	return AEIO_Err_USE_DFLT_CALLBACK; 
};
					
static A_Err	
UIO_GetDuration(
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH	specH, 
	A_Time			*tr)
{ 
	LOG_FUNCTION_NAME();
	return AEIO_Err_USE_DFLT_CALLBACK; 
};

static A_Err	
UIO_GetTime(
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH	specH, 
	A_Time			*tr)
{ 
	LOG_FUNCTION_NAME();
	return AEIO_Err_USE_DFLT_CALLBACK; 
};

static A_Err	
UIO_GetSound(
	AEIO_BasicData				*basic_dataP,	
	AEIO_InSpecH				specH,
	AEIO_SndQuality				quality,
	const AEIO_InterruptFuncs	*interrupt_funcsP0,	
	const A_Time				*startPT,	
	const A_Time				*durPT,	
	A_u_long					start_sampLu,
	A_u_long					num_samplesLu,
	void						*dataPV)
{ 
	LOG_FUNCTION_NAME();

	A_Err			err			=	A_Err_NONE;
	AEIO_Handle		optionsH	=	NULL;
	UIO_Plugin_Header	*headerP		=	NULL; 
	
	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);
	
	ERR(suites.IOInSuite1()->AEGP_GetInSpecOptionsHandle(specH, reinterpret_cast<void**>(&optionsH)));

	ERR(suites.MemorySuite1()->AEGP_LockMemHandle(optionsH, reinterpret_cast<void**>(&headerP)));

	if (!err) {
		A_char report[AEGP_MAX_ABOUT_STRING_SIZE] = {'\0'};
		
		suites.ANSICallbacksSuite1()->sprintf(report, "SDK_IO : %d samples of audio requested.", num_samplesLu); 
			
		ERR(suites.UtilitySuite3()->AEGP_ReportInfo(	basic_dataP->aegp_plug_id, report));
	}

	return err;
};

static A_Err	
UIO_InqNextFrameTime(
	AEIO_BasicData			*basic_dataP,
	AEIO_InSpecH			specH, 
	const A_Time			*base_time_tr,
	AEIO_TimeDir			time_dir, 
	A_Boolean				*found0,
	A_Time					*key_time_tr0)
{ 
	
    LOG_FUNCTION_NAME();
    A_Err				err			=	A_Err_NONE;
    AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);

    return AEIO_Err_USE_DFLT_CALLBACK;
};

static A_Err	
UIO_DisposeOutputOptions(
	AEIO_BasicData	*basic_dataP,
	void			*optionsPV)
{ 
	LOG_FUNCTION_NAME();

	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);
	AEIO_Handle			optionsH	=	static_cast<AEIO_Handle>(optionsPV);
	A_Err				err			=	A_Err_NONE;
	
	if (optionsH){
		ERR(suites.MemorySuite1()->AEGP_FreeMemHandle(optionsH));
	}
	return err;
};
static A_Err	
UIO_UserOptionsDialog(
	AEIO_BasicData			*basic_dataP,
	AEIO_OutSpecH			outH, 
	const PF_EffectWorld	*sample0,
	A_Boolean				*user_interacted0)
{ 
	LOG_FUNCTION_NAME();

	basic_dataP->msg_func(1, "Use the message func hanging off the AEIO_BasicData!");
	return A_Err_NONE;
};

static A_Err	
	UIO_WriteLabels(
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH	outH, 
	AEIO_LabelFlags	*written)
{ 
	LOG_FUNCTION_NAME();

	return AEIO_Err_USE_DFLT_CALLBACK;
};

static A_Err	
UIO_GetOutputInfo(
	AEIO_BasicData		*basic_dataP,
	AEIO_OutSpecH		outH,
	AEIO_Verbiage		*verbiageP)
{ 
	A_Err err			= A_Err_NONE;
	A_char* offsetP 	= NULL;

	LOG_FUNCTION_NAME();

	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);
	/*
	if (!err && S_path) {
		offsetP = strrchr(S_path, '\\'); 

		suites.ANSICallbacksSuite1()->strcpy(verbiageP->name, "filename");
		suites.ANSICallbacksSuite1()->strcpy(verbiageP->type, "(SDK) Fake File");
		suites.ANSICallbacksSuite1()->strcpy(verbiageP->sub_type, "no subtypes supported");
	}
	*/
	return err;
};

static A_Err	
UIO_SetOutputFile(
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH	outH, 
	A_char			*file_pathZ)
{ 
  	LOG_FUNCTION_NAME();

	return AEIO_Err_USE_DFLT_CALLBACK;
}

static A_Err	
UIO_StartAdding(
	AEIO_BasicData		*basic_dataP,
	AEIO_OutSpecH		outH, 
	A_long				flags)
{ 
	LOG_FUNCTION_NAME();

	A_Err	err				=	A_Err_NONE;
	UIO_Plugin_Header	header;
	A_Time	duration	=	{0,1};
	A_Fixed	fps			=	0;
	A_long	widthL 		= 	0,
			heightL 	= 	0;
	A_char name[AEGP_MAX_PATH_SIZE] = {'\0'};
	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);

	AEFX_CLR_STRUCT(header);
	//AEFX_CLR_STRUCT(*S_path);

	ERR(suites.IOOutSuite1()->AEGP_GetOutSpecDuration(outH, &duration));

	ERR(suites.IOOutSuite1()->AEGP_GetOutSpecDimensions(outH, &widthL, &heightL));

	if (name && widthL && heightL) {
		ERR(suites.IOOutSuite1()->AEGP_GetOutSpecFPS(outH, &fps));
		
		if (!err){
			header.fpsT.value	=	duration.value;
			header.fpsT.scale	=	duration.scale;

			header.hasVideo		=	TRUE;	
			header.hasAudio		=	FALSE;
			header.widthLu		=	(unsigned long)widthL;
			header.heightLu		=	(unsigned long)heightL;
			
			header.rowbytesLu	=	(unsigned long)(4 * widthL);

			header.numFramesLu	=	(unsigned long) fps; 

 			//ERR(suites.IOOutSuite1()->AEGP_GetOutSpecFilePath(outH, S_path));
			
			if (!err){
				/*
					+	Open file
					+	Write header
					+	(keep file open)
				*/
			}
		}
	}
	return err; 
};

static A_Err	
UIO_AddFrame(
	AEIO_BasicData			*basic_dataP,
	AEIO_OutSpecH			outH, 
	A_long					frame_index, 
	A_long					frames,
	const PF_EffectWorld	*wP, 
	const A_LPoint			*origin0,
	A_Boolean				was_compressedB,	
	AEIO_InterruptFuncs		*inter0)
{ 

	LOG_FUNCTION_NAME();

	A_Err err = A_Err_NONE;
	
	/*	
		+	reinterpret the provided frame into your format.
		+	append it to the already-opened file.
	*/

	return err; 
};
								
static A_Err	
UIO_EndAdding(
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH			outH, 
	A_long					flags)
{ 
	LOG_FUNCTION_NAME();

	/*
		+	Close file handle, clear out optionsH associated with add.
	*/
	
	return A_Err_NONE; 
};

static A_Err	
UIO_OutputFrame(
	AEIO_BasicData			*basic_dataP,
	AEIO_OutSpecH			outH, 
	const PF_EffectWorld	*wP)
{ 
	
	LOG_FUNCTION_NAME();

	A_Err		err	=	A_Err_NONE;
	/*
		+	Re-interpret the supplied PF_EffectWorld in your own
			format, and save it out to the outH's path.

	*/
	return err;
};

static A_Err	
UIO_GetSizes(
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH	outH, 
	A_u_longlong	*free_space, 
	A_u_longlong	*file_size)
{ 
	LOG_FUNCTION_NAME();

	return AEIO_Err_USE_DFLT_CALLBACK;
};

static A_Err	
UIO_Flush(
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH	outH)
{ 
	/*	free any temp buffers you kept around for
		writing.
	*/
	LOG_FUNCTION_NAME();

	return A_Err_NONE; 
};

static A_Err	
UIO_Idle(
	AEIO_BasicData			*basic_dataP,
	AEIO_ModuleSignature	sig,
	AEIO_IdleFlags			*idle_flags0)
{ 
	//LOG_FUNCTION_NAME();

	return A_Err_NONE; 
};	


static A_Err	
UIO_GetDepths(
	AEIO_BasicData			*basic_dataP,
	AEIO_OutSpecH			outH, 
	AEIO_SupportedDepthFlags		*which)
{ 
	/*	Enumerate possible output depths by ORing 
		together different AEIO_SupportedDepthFlags.
	*/
	LOG_FUNCTION_NAME();

	*which =	AEIO_SupportedDepthFlags_DEPTH_8	|
				AEIO_SupportedDepthFlags_DEPTH_32;

	return A_Err_NONE; 
};

static A_Err	
UIO_GetOutputSuffix(
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH	outH, 
	A_char			*suffix)
{ 
	LOG_FUNCTION_NAME();

	return AEIO_Err_USE_DFLT_CALLBACK;
};


static A_Err	
UIO_SeqOptionsDlg(
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH	specH,  
	A_Boolean		*user_interactedPB0)
{ 
	LOG_FUNCTION_NAME();

	basic_dataP->msg_func(0, "IO: Here's my sequence options dialog!");
	return A_Err_NONE; 
};


static A_Err	
UIO_CloseSourceFiles(
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH			seqH)
{ 
	LOG_FUNCTION_NAME();

	return A_Err_NONE; 
};		// TRUE for close, FALSE for unclose

static A_Err	
UIO_CountUserData(
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH			inH,
	A_u_long 				typeLu,
	A_u_long				max_sizeLu,
	A_u_long				*num_of_typePLu)
{ 
	LOG_FUNCTION_NAME();

	return A_Err_NONE; 
};

static A_Err	
UIO_SetUserData(    
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH			outH,
	A_u_long				typeLu,
	A_u_long				indexLu,
	const AEIO_Handle		dataH)
{ 
	LOG_FUNCTION_NAME();

	return A_Err_NONE; 
};

static A_Err	
UIO_GetUserData(   
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH			inH,
	A_u_long 				typeLu,
	A_u_long				indexLu,
	A_u_long				max_sizeLu,
	AEIO_Handle				*dataPH)
{ 
	LOG_FUNCTION_NAME();

	return A_Err_NONE; 
};
                            
static A_Err	
UIO_AddMarker(		
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH 			outH, 
	A_long 					frame_index, 
	AEIO_MarkerType 		marker_type, 
	void					*marker_dataPV, 
	AEIO_InterruptFuncs		*inter0)
{ 
	/*	The type of the marker is in marker_type,
		and the text they contain is in 
		marker_dataPV.
	*/
	LOG_FUNCTION_NAME();

	return A_Err_NONE; 
};

static A_Err	
UIO_VerifyFileImportable(
	AEIO_BasicData			*basic_dataP,
	AEIO_ModuleSignature	sig, 
	const A_char			*file_pathZ, 
	A_Boolean				*importablePB)
{
	//	This function is an appropriate place to do
	//	in-depth evaluation of whether or not a given
	//	file will be importable; AE already does basic
	//	extension checking. Keep in mind that this 
	//	function should be fairly speedy, to keep from
	//	bogging down the user while selecting files.
	
	//	-bbb 3/31/04
	LOG_FUNCTION_NAME();
	LOG(file_pathZ);
	
	*importablePB = TRUE;
	return A_Err_NONE; 
}		
	
static A_Err	
UIO_InitOutputSpec(
	AEIO_BasicData			*basic_dataP,
	AEIO_OutSpecH			outH, 
	A_Boolean				*user_interacted)
{
	LOG_FUNCTION_NAME();

	A_Err err						= A_Err_NONE;
	AEIO_Handle		optionsH		= NULL, 
					old_optionsH	= 0;
	UIO_Plugin_Header	*fileP	=	new UIO_Plugin_Header;

	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);

	AEFX_CLR_STRUCT(*fileP);
	
	ERR(suites.MemorySuite1()->AEGP_NewMemHandle(	S_mem_id, 
													"InitOutputSpec options", 
													sizeof(UIO_Plugin_Header), 
													AEGP_MemFlag_CLEAR, 
													&optionsH));

	if (!err)
	{
		/*ERR(PretendToReadFileHeader(basic_dataP, fileP));

		ERR(suites.MemorySuite1()->AEGP_LockMemHandle(optionsH, reinterpret_cast<void**>(&fileP)));

		if (!err) {
			basic_dataP->msg_func(err, "Here is where you would modify the output spec,\rbased on user interaction.");

			if (rand() % 7){
				*user_interacted = TRUE;
			} else {
				*user_interacted = FALSE;
			}
			ERR(suites.MemorySuite1()->AEGP_UnlockMemHandle(optionsH));

			ERR(suites.IOOutSuite1()->AEGP_SetOutSpecOptionsHandle(outH, optionsH, reinterpret_cast<void**>(&old_optionsH)));

			if (old_optionsH) {
				ERR(suites.MemorySuite1()->AEGP_FreeMemHandle(old_optionsH));
			}
		}*/
	}
	return err;
}

static A_Err	
UIO_OutputInfoChanged(
	AEIO_BasicData		*basic_dataP,
	AEIO_OutSpecH		outH)
{
	/*	This function is called when either the user 
		or the plug-in has changed the output options info.
		You may want to update your plug-in's internal
		representation of the output at this time. 
		We've exercised the likely getters below.
	*/
	LOG_FUNCTION_NAME();

	A_Err err					=	A_Err_NONE;
	
	AEIO_AlphaLabel	alpha;
	AEFX_CLR_STRUCT(alpha);
	
	FIEL_Label		fields;
	AEFX_CLR_STRUCT(fields);

	A_short			depthS		=	0;
	A_Time			durationT	=	{0,1};

	A_Fixed			native_fps	=	0;
	A_Ratio			hsf			=	{1,1};
	A_Boolean		is_missingB	=	TRUE;
	
	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);	
	//AEFX_CLR_STRUCT(*S_path);

	//ERR(suites.IOOutSuite1()->AEGP_GetOutSpecFilePath(outH, S_path));

	//ERR(suites.IOOutSuite1()->AEGP_GetOutSpecIsMissing(outH, &is_missingB));

	//if (!is_missingB)
	//{
	//	// Find out all the details of the output spec; update
	//	// your options data as necessary.
	//	
	//	ERR(suites.IOOutSuite1()->AEGP_GetOutSpecAlphaLabel(outH, &alpha));
	//	ERR(suites.IOOutSuite1()->AEGP_GetOutSpecDepth(outH, &depthS));
	//	ERR(suites.IOOutSuite1()->AEGP_GetOutSpecInterlaceLabel(outH, &fields));
	//	ERR(suites.IOOutSuite1()->AEGP_GetOutSpecDuration(outH, &durationT));
	//	ERR(suites.IOOutSuite1()->AEGP_GetOutSpecFPS(outH, &native_fps));
	//	ERR(suites.IOOutSuite1()->AEGP_GetOutSpecHSF(outH, &hsf));
	//}
	return err;
}

static A_Err	
UIO_GetFlatOutputOptions(
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH	outH, 
	AEIO_Handle		*optionsPH)
{
	
	LOG_FUNCTION_NAME();

	A_Err			err			=	A_Err_NONE;
	UIO_Plugin_Header	*infoP		=	0;
	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);
	
	ERR(suites.MemorySuite1()->AEGP_NewMemHandle(	S_mem_id, 
													"flat optionsH", 
													sizeof(UIO_Plugin_Header), 
													AEGP_MemFlag_CLEAR, 
													optionsPH));

	ERR(suites.MemorySuite1()->AEGP_LockMemHandle(*optionsPH, reinterpret_cast<void**>(&infoP)));

	if (!err){
		suites.ANSICallbacksSuite1()->strcpy(infoP->name, "flatoptions");
		ERR(suites.MemorySuite1()->AEGP_UnlockMemHandle(*optionsPH));
	}

	return err;
}

static A_Err
	VerifyFileImportable(
	const char *		file_fullpathZ,		// >>
	AE_FIM_Refcon		refcon,				// >> the client defines this and it is stored with import callbacks
	A_Boolean			*importablePB)		// << 
{
	*importablePB = TRUE;	// Do a lot more checking than this in your plug-in! 
	return A_Err_NONE;
}

static A_Err
UIO_DrawAuxChannel( 
	AEIO_BasicData			*basic_dataP,
	AEIO_InSpecH			inH,
	A_long					chan_indexL,
	const AEIO_DrawFramePB	*pbP,
	PF_ChannelChunk			*chunkP)
{
	A_Err err = A_Err_NONE;
	return err;
}

//Set information for AEIO
static A_Err
ConstructModuleInfo(
	SPBasicSuite		*pica_basicP,			
	AEIO_ModuleInfo		*info)
{
	A_Err err = A_Err_NONE;

	AEGP_SuiteHandler	suites(pica_basicP);
	
	if (info) {
		//AEIO unique id
		info->sig						=	'WEBP';
		
		info->max_width					=	4096;
		info->max_height				=	4096;
		info->num_filetypes				=	1;
		info->num_extensions			=	1;
		//Number clipboard files
		info->num_clips					=	1;
		// Mac OS four character code for files created by your AEIO
		
		info->create_kind.type			=	'WEBP';
		info->create_kind.creator		=	AEIO_ANY_CREATOR;
		
		//File extension information. Unfortunately it has only 3 char for extension. 
		//So, we must do some tricks and implement our own import dialog
		info->create_ext.pad			=	'.';
		info->create_ext.extension[0]	=	'w';
		info->create_ext.extension[1]	=	'e';
		info->create_ext.extension[2]	=	'b';
		info->create_ext.extension[3]	=	'p';

		//The number of auxiliary extensions supported by your AEIO

		info->num_aux_extensionsS		=	0;
		
		suites.ANSICallbacksSuite1()
			->strcpy(info->name, "WebP image file");
		//your AEIO possibilities
		info->flags						=	AEIO_MFlag_INPUT					 
											//| AEIO_MFlag_OUTPUT					 
											| AEIO_MFlag_FILE					
											| AEIO_MFlag_VIDEO
											| AEIO_MFlag_HAS_META_DATA
											| AEIO_MFlag_CANT_CLIP
											//| AEIO_MFlag_NO_TIME
											//| AEIO_MFlag_INTERACTIVE_GET
                                            /*
                                            | AEIO_MFlag_INPUT_OPTIONS
											
                                            //| AEIO_MFlag_NO_TIME					 
											| AEIO_MFlag_HOST_FRAME_START_DIALOG	
											//| AEIO_MFlag_NO_OPTIONS*/
                                            ;

		info->read_kinds[0].mac.type			=	'PTC_';
		info->read_kinds[0].mac.creator			=	AEIO_ANY_CREATOR;
		info->read_kinds[1].ext					=	info->create_ext;
		info->read_kinds[2].ext.pad				=	'.';
		info->read_kinds[2].ext.extension[0]	=	'w';
		info->read_kinds[2].ext.extension[1]	=	'e';
		info->read_kinds[2].ext.extension[2]	=	'b';
		info->read_kinds[2].ext.extension[3]	=	'p';
	}
	else
	{
		err = A_Err_STRUCT;
	}
	
	return err;
}

A_Err
ConstructFunctionBlock(
	AEIO_FunctionBlock2	*funcs)
{
	if (funcs)	{
		funcs->AEIO_AddFrame				=	UIO_AddFrame;
		funcs->AEIO_CloseSourceFiles		=	UIO_CloseSourceFiles;
		funcs->AEIO_CountUserData			=	UIO_CountUserData;
		funcs->AEIO_DisposeInSpec			=	UIO_DisposeInSpec;
		funcs->AEIO_DisposeOutputOptions	=	UIO_DisposeOutputOptions;
		funcs->AEIO_DrawSparseFrame			=	UIO_DrawSparseFrame;
		funcs->AEIO_EndAdding				=	UIO_EndAdding;
		funcs->AEIO_FlattenOptions			=	UIO_FlattenOptions;
		funcs->AEIO_Flush					=	UIO_Flush;
		funcs->AEIO_GetActiveExtent			=	UIO_GetActiveExtent;
		funcs->AEIO_GetDepths				=	UIO_GetDepths;
		funcs->AEIO_GetDimensions			=	UIO_GetDimensions;
		funcs->AEIO_GetDuration				=	UIO_GetDuration;
		funcs->AEIO_GetSound				=	UIO_GetSound;
		funcs->AEIO_GetInSpecInfo			=	UIO_GetInSpecInfo;
		funcs->AEIO_GetOutputInfo			=	UIO_GetOutputInfo;
		funcs->AEIO_GetOutputSuffix			=	UIO_GetOutputSuffix;
		funcs->AEIO_GetSizes				=	UIO_GetSizes;
		funcs->AEIO_GetTime					=	UIO_GetTime;
		funcs->AEIO_GetUserData				=	UIO_GetUserData;
		funcs->AEIO_InflateOptions			=	UIO_InflateOptions;
		funcs->AEIO_InitInSpecFromFile		=	UIO_InitInSpecFromFile;
		funcs->AEIO_InqNextFrameTime		=	UIO_InqNextFrameTime;
		funcs->AEIO_OutputFrame				=	UIO_OutputFrame;
		funcs->AEIO_SeqOptionsDlg			=	UIO_SeqOptionsDlg;
		funcs->AEIO_SetOutputFile			=	UIO_SetOutputFile;
		funcs->AEIO_SetUserData				=	UIO_SetUserData;
		funcs->AEIO_StartAdding				=	UIO_StartAdding;
		funcs->AEIO_SynchInSpec				=	UIO_SynchInSpec;
		funcs->AEIO_UserOptionsDialog		=	UIO_UserOptionsDialog;
		funcs->AEIO_VerifyFileImportable	=	UIO_VerifyFileImportable;
		funcs->AEIO_WriteLabels				=	UIO_WriteLabels;
		funcs->AEIO_InitOutputSpec			=	UIO_InitOutputSpec;
		funcs->AEIO_GetFlatOutputOptions	=	UIO_GetFlatOutputOptions;
		funcs->AEIO_OutputInfoChanged		=	UIO_OutputInfoChanged;
		funcs->AEIO_AddMarker				=	UIO_AddMarker;
		funcs->AEIO_Idle					=	UIO_Idle;
		funcs->AEIO_DrawAuxChannel 			=	UIO_DrawAuxChannel;
		return A_Err_NONE;
	} else {
		return A_Err_STRUCT;
	}
}

//Entry point for AEIO. Perform initialization here
DllExport A_Err
EntryPointFunc(
	struct SPBasicSuite		*pica_basicP,			/* >> */
	A_long				 	major_versionL,			/* >> */		
	A_long					minor_versionL,			/* >> */		
	const A_char		 	*file_pathZ,			/* >> platform-specific delimiters */
	const A_char		 	*res_pathZ,				/* >> platform-specific delimiters */
	AEGP_PluginID			aegp_plugin_id,			/* >> */
	void					*global_refconPV)		/* << */
{
	
	if( UIO_Plugin::createPlugin( aegp_plugin_id ) == NULL )
	{
		return A_Err_GENERIC;
	}

	S_PicaBasic_SuiteP = pica_basicP;
	A_Err 				err					= A_Err_NONE;
	
	//AEIO Id
	S_mem_id = aegp_plugin_id;
	
	AEIO_ModuleInfo		info;
	AEIO_FunctionBlock2	funcs;
	AEGP_SuiteHandler	suites(pica_basicP);	

	AEFX_CLR_STRUCT(info);
	AEFX_CLR_STRUCT(funcs);
	
	ERR(suites.RegisterSuite3()->AEGP_RegisterDeathHook(aegp_plugin_id, DeathHook, 0));
	ERR(ConstructModuleInfo(pica_basicP, &info));
	ERR(ConstructFunctionBlock(&funcs));

	ERR(suites.RegisterSuite3()->AEGP_RegisterIO(	aegp_plugin_id,
													0,
													&info, 
													&funcs));

	ERR(suites.UtilitySuite3()->AEGP_RegisterWithAEGP(	NULL,
														"UIO",
														&S_mem_id));
	
	//Insert entry to import menu
	ERR(suites.CommandSuite1()->AEGP_GetUniqueCommand(&S_import_cmd));

	ERR(suites.CommandSuite1()->AEGP_InsertMenuCommand(S_import_cmd, "Import WebP image file", AEGP_Menu_IMPORT, AEGP_MENU_INSERT_AT_BOTTOM));

	ERR(suites.RegisterSuite2()->AEGP_RegisterCommandHook(	S_mem_id, 
		AEGP_HP_BeforeAE, 
		AEGP_Command_ALL, 
		CommandHook, 
		NULL));

	ERR(suites.RegisterSuite2()->AEGP_RegisterUpdateMenuHook(S_mem_id, UpdateMenuHook, NULL));
	
	LOG_FUNCTION_NAME();
	LOG(file_pathZ);
	LOG(res_pathZ);
	return err;
}
