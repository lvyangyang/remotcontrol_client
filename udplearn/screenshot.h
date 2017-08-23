#pragma once
#ifndef SCREENSHOT_H

extern "C"
{
#include "libavcodec/avcodec.h"  
#include "libavformat/avformat.h"  
#include "libswscale/swscale.h"  
#include "libavutil/imgutils.h"  
#include "libavutil/opt.h"
#include "SDL/SDL.h"  
};

#include "winuser.h"

int encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt, FILE *outfile);


DWORD cyclesend(LPVOID pParam);

//"screenshot.h"
DWORD screenshot(LPVOID pParam);

#define SCREENSHOT_H
#endif