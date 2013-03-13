#pragma  once

#include "AE_IO.h"
#include "AEConfig.h"

extern "C" {
	typedef struct {
		char				name[256];	// "SDK_FILE ";
		unsigned char		hasAudio;					// 	0x00 = no audio
		//	0x0f = audio uncompressed
		//	0xff = audio RLE comp. 
		//	NOTE: for demo purposes; the plug-in does NOT compress audio
		unsigned char		hasVideo;					// always 32 bit
		//	0x00 = no video
		//	0x0f = video uncompressed
		//	0xff = video RLE comp.
		unsigned long		widthLu;					// width of frame in pixels
		unsigned long		heightLu;					// height of frame in pixels
		unsigned long		rowbytesLu;					// total bytes in row, aka width * 4, we strip any row padding
		unsigned long		numFramesLu;				// number of frames in file
		A_Time				fpsT;
		short				bitdepthS;
		A_FpLong			rateF;
		long				avg_bit_rateL;
		long				block_sizeL;
		long				frames_per_blockL;
		AEIO_SndEncoding	encoding;
		AEIO_SndSampleSize	bytes_per_sample;
		AEIO_SndChannels	num_channels;
		short				padS;
		unsigned long		duration_in_msF;
		A_Time				durationT;
		int					codecId;
		int					decoderId;
	}UIO_Plugin_Header;
};