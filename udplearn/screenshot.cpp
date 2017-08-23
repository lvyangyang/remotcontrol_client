#include "screenshot.h"
#include  "stdafx.h"

//Windows  
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

struct buffer_data {
	uint8_t *ptr;
	size_t size; ///< size left in the buffer
};
static buffer_data bd;
int encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt, FILE *outfile)
{
	int ret;

	/* send the frame to the encoder */
	if (frame)
		printf("Send frame %d\n", frame->pts);

	ret = avcodec_send_frame(enc_ctx, frame);
	if (ret < 0) {
		fprintf(stderr, "Error sending a frame for encoding\n");
		return -1;
	}

	while (ret >= 0) {
		ret = avcodec_receive_packet(enc_ctx, pkt);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return -2;
		else if (ret < 0) {
			fprintf(stderr, "Error during encoding\n");
			return -3;
		}
		fwrite(pkt->data, 1, pkt->size, outfile);
		bd.ptr = pkt->data;
		bd.size = pkt->size;

		printf("Write packet %d ", pkt->pts);
		printf("size= %d\n", pkt->size);
		return 233;
	}
}

DWORD WINAPI cyclesend(LPVOID pParam)
{
	struct buffer_data {
		uint8_t *ptr;
		size_t size; ///< size left in the buffer
	}bd;
	bd = *(buffer_data*)pParam;
	char *sendbuffer;
	int cpsize = 4096;
	int leftsize = bd.size;
	sendbuffer = (char *)malloc(4096);
	while (1)
	{
		if (leftsize <= 4096)
			cpsize = leftsize;
		memcpy(sendbuffer, bd.ptr, cpsize);
		if (send(tcp_s, sendbuffer, cpsize, 0) == SOCKET_ERROR)
			break;
		bd.ptr += cpsize;
		leftsize -= cpsize;
		if (cpsize < 4096)
			break;
	}
	return 1;
}

DWORD WINAPI screenshot(LPVOID pParam)
{
	FILE *outfile;
	int ret;
	int got_picture;
	int i = 0;
	int picture_size;

	//---frame-------------------------
	AVFrame *m_pRGBFrame;
	AVFrame *m_pYUVFrame;
	AVFrame *decodec_Frame;
	AVFrame *out_pYUVFrame;
	struct SwsContext *img_convert_ctx;
	struct SwsContext *out_img_convert_ctx;
	uint8_t * yuv_buff;
	uint8_t * decodec_yuv_buff;
	uint8_t * out_yuv_buff;
	uint8_t *cppktbuf;

	//-------------codec------------
	AVCodec *encodec;
	AVCodec *decodec;
	AVPacket *packet;
	AVPacket *cppkt;
	AVCodecContext *encodecctx, *decodecctx;
	AVFormatContext *pFormatCtx;


	//-----------sdl----------------
	SDL_Window *screen;
	SDL_Renderer* sdlRenderer;
	SDL_Texture* sdlTexture;
	SDL_Rect sdlRect;

	//截图设置----------------------
	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	HWND hDesktopWnd = ::GetDesktopWindow();
	HDC hDesktopDC = ::GetDC(hDesktopWnd);
	HDC hCaptureDC = CreateCompatibleDC(hDesktopDC);
	BITMAP bmpScreen;
	HBITMAP hCaptureBitmap = CreateCompatibleBitmap(hDesktopDC,
		nScreenWidth, nScreenHeight);
	BITMAPINFOHEADER   bi;




	SelectObject(hCaptureDC, hCaptureBitmap);
	//截图动作----------------
	BitBlt(hCaptureDC, 0, 0, nScreenWidth, nScreenHeight, hDesktopDC, 0, 0, SRCCOPY);

	GetObject(hCaptureBitmap, sizeof(BITMAP), &bmpScreen);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmpScreen.bmWidth;
	bi.biHeight = bmpScreen.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;
	HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
	char *lpbitmap = (char *)GlobalLock(hDIB);
	GetDIBits(hCaptureDC, hCaptureBitmap, 0,
		(UINT)bmpScreen.bmHeight,
		lpbitmap,
		(BITMAPINFO *)&bi, DIB_RGB_COLORS);

	//sdl-------------------------------------------------------------------------------------------------
	int screen_w = 0, screen_h = 0;


	//sdl初始化-----------------------------------------------------------------------------------
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return -1;
	}

	screen_w = nScreenWidth / 2;
	screen_h = nScreenHeight / 2;
	//SDL 2.0 Support for multiple windows  
	screen = SDL_CreateWindow("Simplest ffmpeg player's Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		screen_w, screen_h,
		SDL_WINDOW_OPENGL);

	if (!screen) {
		printf("SDL: could not create window - exiting:%s\n", SDL_GetError());
		return -1;
	}

	sdlRenderer = SDL_CreateRenderer(screen, -1, 0);
	//IYUV: Y + U + V  (3 planes)  
	//YV12: Y + V + U  (3 planes)  
	sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, screen_w, screen_h);

	sdlRect.x = 0;
	sdlRect.y = 0;
	sdlRect.w = screen_w;
	sdlRect.h = screen_h;


	//------------------------------------------frame 结构完备 数据结构准备---------------------------------------------------------
	img_convert_ctx = sws_getContext(1920, 1080, AV_PIX_FMT_BGR24,
		1920, 1080, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

	out_img_convert_ctx = sws_getContext(1920, 1080, AV_PIX_FMT_YUV420P,
		1920 / 2, 1080 / 2, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);



	yuv_buff = (uint8_t *)malloc(1920 * 1080 * 1.5);
	decodec_yuv_buff = (uint8_t *)malloc(1920 * 1080 * 1.5);
	out_yuv_buff = (uint8_t *)malloc(1920 * 1080 * 1.5 / 4);


	m_pRGBFrame = av_frame_alloc();
	m_pYUVFrame = av_frame_alloc();
	decodec_Frame = av_frame_alloc();
	out_pYUVFrame = av_frame_alloc();

	m_pYUVFrame->format = AV_PIX_FMT_YUV420P;
	m_pYUVFrame->height = 1080;
	m_pYUVFrame->width = 1920;

	//--------------------------------------codecctx packet 结构完备  数据结构准备------------------------------
	avcodec_register_all();
	encodec = avcodec_find_encoder(AV_CODEC_ID_H264);
	decodec = avcodec_find_decoder(AV_CODEC_ID_H264);

	if (!encodec) {
		fprintf(stderr, "Codec '%s' not found\n");
		exit(1);
	}
	encodecctx = avcodec_alloc_context3(encodec);
	if (!encodecctx) {
		fprintf(stderr, "Could not allocate video codec context\n");
		exit(1);
	}
	decodecctx = avcodec_alloc_context3(decodec);
	packet = av_packet_alloc();
	if (!packet)
		exit(1);
	cppkt = av_packet_alloc();


	encodecctx->bit_rate = 1600000;
	/* resolution must be a multiple of two */
	encodecctx->width = 1920;
	encodecctx->height = 1080;
	/* frames per second */
	encodecctx->framerate = { 10, 1 };
	encodecctx->time_base = { 1, 10 };

	/* emit one intra frame every ten frames
	* check frame pict_type before passing frame
	* to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
	* then gop_size is ignored and the output of encoder
	* will always be I frame irrespective to gop_size
	*/
	encodecctx->gop_size = 25;
	encodecctx->max_b_frames = 1;
	encodecctx->pix_fmt = AV_PIX_FMT_YUV420P;
	encodecctx->codec_type = AVMEDIA_TYPE_VIDEO;

	decodecctx->width = 1920;
	decodecctx->height = 1080;
	//av_opt_set(encodecctx->priv_data, "tune", "zerolatency", 0);

	//H264, 设置为编码延迟为立即编码
	AVDictionary *param = NULL;
	if (encodecctx->codec_id == AV_CODEC_ID_H264)
	{
		av_dict_set(&param, "preset", "superfast", 0);
		av_dict_set(&param, "tune", "zerolatency", 0);
	}
	av_opt_set(decodecctx->priv_data, "tune", "zerolatency", 0);

	ret = avcodec_open2(encodecctx, encodec, &param);
	if (ret < 0) {
		fprintf(stderr, "Could not open codec: %s\n");
		exit(1);
	}

	ret = avcodec_open2(decodecctx, decodec, NULL);
	if (ret < 0) {
		fprintf(stderr, "Could not open codec: %s\n");
		exit(1);
	}
	//----------------------------解码准备-----------------------------------
	/*
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();
	unsigned char *aviobuffer = (unsigned char *)av_malloc(4096);
	AVIOContext *avio = avio_alloc_context(aviobuffer, 4096, 0, &bd, &read_packet, NULL, NULL);
	pFormatCtx->pb = avio;

	*/


	//----------文件-----------------------------
	fopen_s(&outfile, "out.h264", "wb");
	if (!outfile) {
		printf("Could not open %s\n");
		exit(1);
	}



	//----------------------------------------------开始循环-----------------------------------------------------------------------------------------------------------------------------
	while (1)
	{


		BitBlt(hCaptureDC, 0, 0, nScreenWidth, nScreenHeight, hDesktopDC, 0, 0, SRCCOPY);
		GetObject(hCaptureBitmap, sizeof(BITMAP), &bmpScreen);
		//GetObject(hCaptureBitmap,sizeof(BITMAP),(LPSTR)&CaptureBitmap);
		GetDIBits(hCaptureDC, hCaptureBitmap, 0,
			(UINT)bmpScreen.bmHeight,
			lpbitmap,
			(BITMAPINFO *)&bi, DIB_RGB_COLORS);
		//--------填充frame---------
		av_image_fill_arrays(m_pRGBFrame->data, m_pRGBFrame->linesize, (uint8_t *)lpbitmap,
			AV_PIX_FMT_BGR24, 1920, 1080, 1);

		av_image_fill_arrays(m_pYUVFrame->data, m_pYUVFrame->linesize, yuv_buff,
			AV_PIX_FMT_YUV420P, 1920, 1080, 1);

		av_image_fill_arrays(decodec_Frame->data, decodec_Frame->linesize, decodec_yuv_buff,
			AV_PIX_FMT_YUV420P, 1920, 1080, 1);

		av_image_fill_arrays(out_pYUVFrame->data, out_pYUVFrame->linesize, out_yuv_buff,
			AV_PIX_FMT_YUV420P, 1920 / 2, 1080 / 2, 1);

		// 翻转RGB图像  
		m_pRGBFrame->data[0] += m_pRGBFrame->linesize[0] * (1080 - 1);
		m_pRGBFrame->linesize[0] *= -1;
		m_pRGBFrame->data[1] += m_pRGBFrame->linesize[1] * (1080 / 2 - 1);
		m_pRGBFrame->linesize[1] *= -1;
		m_pRGBFrame->data[2] += m_pRGBFrame->linesize[2] * (1080 / 2 - 1);
		m_pRGBFrame->linesize[2] *= -1;


		//将RGB转化为YUV  
		//将RGB转化为YUV  
		//sws_scale(scxt, m_pRGBFrame->data, m_pRGBFrame->linesize, 0, pCodecCtx->height, m_pYUVFrame->data, m_pYUVFrame->linesize);
		sws_scale(img_convert_ctx, (const unsigned char* const*)m_pRGBFrame->data, m_pRGBFrame->linesize, 0, 1080,
			m_pYUVFrame->data, m_pYUVFrame->linesize);

		//--------------编码-------------
		m_pYUVFrame->pts = i++;


		int issuccess = encode(encodecctx, m_pYUVFrame, packet, outfile);
		if (issuccess != 233)
			continue;
		//--------------发送-------------
		CreateThread(NULL, 0, cyclesend, (LPVOID)&bd, 0, NULL);
		//______________解码--------------

		ret = avformat_open_input(&pFormatCtx, NULL, NULL, NULL);
		if (ret < 0)
			continue;

		ret = av_read_frame(pFormatCtx, cppkt);
		if (ret < 0)
			continue;

		cppkt->data = bd.ptr;
		cppkt->size = bd.size;
		ret = avcodec_send_packet(decodecctx, cppkt);
		got_picture = avcodec_receive_frame(decodecctx, decodec_Frame);
		if (got_picture != 0)
		{
			continue;
		}

		//-----------------大小转换------------------------
		sws_scale(out_img_convert_ctx, (const unsigned char* const*)decodec_Frame->data, decodec_Frame->linesize, 0, 1080,
			out_pYUVFrame->data, out_pYUVFrame->linesize);


		SDL_UpdateTexture(sdlTexture, &sdlRect, out_pYUVFrame->data[0], out_pYUVFrame->linesize[0]);
		SDL_RenderClear(sdlRenderer);
		SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &sdlRect);
		SDL_RenderPresent(sdlRenderer);
		//SDL End-----------------------  
		//Delay 40ms  
		//SDL_Delay(10);
	}

	sws_freeContext(img_convert_ctx);
	av_frame_free(&m_pRGBFrame);
	av_frame_free(&m_pYUVFrame);
	avcodec_close(encodecctx);
	avcodec_close(decodecctx);
	::ReleaseDC(hDesktopWnd, hDesktopDC);
	DeleteDC(hCaptureDC);
	DeleteObject(hCaptureBitmap);
	return -1;
}