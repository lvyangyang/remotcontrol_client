
// udplearnDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "udplearn.h"
#include "udplearnDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//================--保存packet数据--------------------
struct buffer_data {
	uint8_t *ptr;
	size_t size; ///< size left in the buffer
}bd;

//-------------------socket设置数据定义-----------------
WSADATA wsad;
static SOCKET s, tcp_s;
SOCKADDR_IN addr;
SOCKADDR_IN peeraddr, server_addr;
BOOL bBoardcast;
BOOL isconnected = 0;
DWORD threadid;
HANDLE hthread;
LPVOID lparam;
//bool beconscan;
bool online;

char szBuf[100] = { 0 };
SOCKADDR_IN remote;
int len = sizeof(remote);
//---------------------接受UDP广播------------------------
DWORD WINAPI udpproc(LPVOID lparam)
{
	bool becon = 1;


	while (becon)
	{

		if (recvfrom(s, szBuf, 100, 0, (LPSOCKADDR)&remote, &len) == SOCKET_ERROR)
		{
			//AfxMessageBox(_T("socket error"));
			break;
		}

		//int namelength;
		//getpeername(s, (LPSOCKADDR)&peeraddr, &namelength);
		peeraddr = *(SOCKADDR_IN*)szBuf;

		AfxMessageBox(_T("get into thread"));
		::SendMessage(*((HWND*)lparam), WM_UDPMESSAGE, 123, (LPARAM)&szBuf);
		becon = 0;

	}
	return 1;
}
//--------------------通过UDP接受信息链接TCP---------------------------------------
DWORD WINAPI recvmanu(LPVOID lparam)
{
	int i=0;
	int vkey = 0;
	char *eventbuffer = (char *)malloc(sizeof(SDL_Event));
	int oncerecv = 0;
	int reside_width = 1920 / 2;
	int reside_height = 1080 / 2;
	
//	int totalrecv = 0;
	while (1)
	{
		oncerecv = recv(tcp_s, eventbuffer, sizeof(SDL_Event), 0);
		SDL_Event myevent=*(SDL_Event*)eventbuffer;
		if (oncerecv==SOCKET_ERROR)
		break;
		switch(myevent.type)
		{
		case SDL_WINDOWEVENT:
			if (myevent.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				reside_width = myevent.window.data1;
				reside_height = myevent.window.data2;
				break;
			}
			if (myevent.window.event == SDL_WINDOW_MAXIMIZED)
			{
				reside_width = 1920;
				reside_height = 1020;
			}

		case SDL_MOUSEMOTION:
			mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE, myevent.motion.x*34.1*1920/reside_width, myevent.motion.y*64.5*1020/reside_height, 0, 0);
			break;
		case SDL_MOUSEBUTTONDOWN:
			if(myevent.button.button==SDL_BUTTON_LEFT)
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			if (myevent.button.button == SDL_BUTTON_RIGHT)
				mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
			break;
		case SDL_MOUSEBUTTONUP:
				if (myevent.button.button == SDL_BUTTON_LEFT)
					mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
				if (myevent.button.button == SDL_BUTTON_RIGHT)
					mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
				break;
		case SDL_KEYDOWN:
		//-------------a---z-----
			if (myevent.key.keysym.scancode <= 29 && myevent.key.keysym.scancode >= 4)
			{
				keybd_event(myevent.key.keysym.scancode + 61, 0, 0, 0);
				break;
			}
			//-----------0---9----------
			if (myevent.key.keysym.scancode < 39 && myevent.key.keysym.scancode >= 30)
			{
				keybd_event(myevent.key.keysym.scancode + 19, 0, 0, 0);
				break;
			}
			if (myevent.key.keysym.scancode == 39)
			{
				keybd_event(myevent.key.keysym.scancode + 9, 0, 0, 0);
				break;
			}
			//-------f1------f12------------
			if (myevent.key.keysym.scancode < 69 && myevent.key.keysym.scancode >= 58)
			{
				keybd_event(myevent.key.keysym.scancode + 54, 0, 0, 0);
				break;
			}
			//-----------pad0--------pad9-----------------------
		
			if (myevent.key.keysym.scancode < 98 && myevent.key.keysym.scancode >= 89)
			{
				keybd_event(myevent.key.keysym.scancode + 8, 0, 0, 0);
				break;
			}
			if (myevent.key.keysym.scancode == 98)
			{
				keybd_event(myevent.key.keysym.scancode - 2, 0, 0, 0);
				break;
			}
			//------------------------mescular---left-------------------
			switch (myevent.key.keysym.scancode)
			{

			case SDL_SCANCODE_RETURN:vkey = VK_RETURN;
				break;
			case SDL_SCANCODE_ESCAPE:vkey = VK_ESCAPE;
				break;
			case SDL_SCANCODE_BACKSPACE:vkey = VK_BACK;
				break;
			case SDL_SCANCODE_TAB:vkey = VK_TAB;
				break;
			case SDL_SCANCODE_SPACE:vkey = VK_SPACE;
				break;
			case SDL_SCANCODE_MINUS:vkey = VK_OEM_MINUS;
				break;
			case SDL_SCANCODE_EQUALS:vkey = VK_OEM_PLUS;
				break;
			case SDL_SCANCODE_LEFTBRACKET:vkey = VK_OEM_4;
				break;
			case SDL_SCANCODE_RIGHTBRACKET:vkey = VK_OEM_6;
				break;
			case SDL_SCANCODE_BACKSLASH:vkey = VK_OEM_5;
				break;
			case SDL_SCANCODE_SEMICOLON:vkey = VK_OEM_1;
				break;
			case SDL_SCANCODE_APOSTROPHE:vkey = 222;
				break;
			case SDL_SCANCODE_COMMA:vkey = VK_OEM_COMMA;
				break;
			case SDL_SCANCODE_PERIOD:vkey = VK_OEM_PERIOD;
				break;
			case SDL_SCANCODE_SLASH:vkey = VK_OEM_2;
				break;
			case SDL_SCANCODE_CAPSLOCK:vkey = VK_CAPITAL;
				break;
			case SDL_SCANCODE_PRINTSCREEN:vkey = VK_SNAPSHOT;
				break;
			case SDL_SCANCODE_SCROLLLOCK:vkey = VK_SCROLL;
				break;
			case SDL_SCANCODE_PAUSE:vkey = VK_PAUSE;
				break;
			case SDL_SCANCODE_INSERT:vkey = VK_INSERT;
				break;
			case SDL_SCANCODE_HOME:vkey = VK_HOME;
				break;
			case SDL_SCANCODE_PAGEUP:vkey = VK_PRIOR;
				break;
			case SDL_SCANCODE_DELETE:vkey = VK_DELETE;
				break;
			case SDL_SCANCODE_END:vkey = VK_END;
				break;
			case SDL_SCANCODE_PAGEDOWN:vkey = VK_NEXT;
				break;
			case SDL_SCANCODE_RIGHT:vkey = VK_RIGHT;
				break;
			case SDL_SCANCODE_LEFT:vkey = VK_LEFT;
				break;
			case SDL_SCANCODE_DOWN:vkey = VK_DOWN;
				break;
			case SDL_SCANCODE_UP:vkey = VK_UP;
				break;
			case SDL_SCANCODE_NUMLOCKCLEAR:vkey = VK_NUMLOCK;
				break;
			case SDL_SCANCODE_KP_DIVIDE:vkey = VK_DIVIDE;
				break;
			case SDL_SCANCODE_KP_MULTIPLY:vkey = VK_MULTIPLY;
				break;
			case SDL_SCANCODE_KP_MINUS:vkey = VK_OEM_MINUS;
				break;
			case SDL_SCANCODE_KP_PLUS:vkey = VK_OEM_PLUS;
				break;
			case SDL_SCANCODE_KP_ENTER:vkey = VK_RETURN;
				break;
			case SDL_SCANCODE_KP_PERIOD:vkey = VK_OEM_PERIOD;
				break;
//--------------键盘底部 左边-------------------------
			case SDL_SCANCODE_LCTRL:vkey = VK_CONTROL;
				break;

			case SDL_SCANCODE_LSHIFT:vkey = VK_LSHIFT;
				break;

			case SDL_SCANCODE_LALT:vkey = VK_MENU;
				break;

			case SDL_SCANCODE_RCTRL:vkey = VK_CONTROL;
				break;
			case SDL_SCANCODE_RALT:vkey = VK_MENU;
				break;
			case SDL_SCANCODE_RSHIFT:vkey = VK_RSHIFT;
				break;
			default:
				break;
			}
			keybd_event(vkey, 0, 0, 0);
			break;
		case SDL_KEYUP:
			//-------------a---z-----
			if (myevent.key.keysym.scancode <= 29 && myevent.key.keysym.scancode >= 4)

			{
				keybd_event(myevent.key.keysym.scancode + 61, 0, KEYEVENTF_KEYUP, 0);
				break;
			}
			//-----------0---9----------
			if (myevent.key.keysym.scancode < 39 && myevent.key.keysym.scancode >= 30)
			{
				keybd_event(myevent.key.keysym.scancode + 19, 0, KEYEVENTF_KEYUP, 0);
				break;
			}
			if (myevent.key.keysym.scancode == 39)
			{
				keybd_event(myevent.key.keysym.scancode + 9, 0, KEYEVENTF_KEYUP, 0);
				break;
			}
			//-------f1------f12------------
			if (myevent.key.keysym.scancode < 69 && myevent.key.keysym.scancode >= 58)
			{
				keybd_event(myevent.key.keysym.scancode + 54, 0, KEYEVENTF_KEYUP, 0);
				break;
			}
			//-----------pad0--------pad9-----------------------
			if (myevent.key.keysym.scancode < 98 && myevent.key.keysym.scancode >= 89)
			{
				keybd_event(myevent.key.keysym.scancode + 8, 0, KEYEVENTF_KEYUP, 0);
				break;
			}
			if (myevent.key.keysym.scancode == 98)
			{
				keybd_event(myevent.key.keysym.scancode - 2, 0, KEYEVENTF_KEYUP, 0);
				break;
			}
			//------------------------mescular---left-------------------
			switch (myevent.key.keysym.scancode)
			{

			case SDL_SCANCODE_RETURN:vkey = VK_RETURN;
				break;
			case SDL_SCANCODE_ESCAPE:vkey = VK_ESCAPE;
				break;
			case SDL_SCANCODE_BACKSPACE:vkey = VK_BACK;
				break;
			case SDL_SCANCODE_TAB:vkey = VK_TAB;
				break;
			case SDL_SCANCODE_SPACE:vkey = VK_SPACE;
				break;
			case SDL_SCANCODE_MINUS:vkey = VK_OEM_MINUS;
				break;
			case SDL_SCANCODE_EQUALS:vkey = VK_OEM_PLUS;
				break;
			case SDL_SCANCODE_LEFTBRACKET:vkey = VK_OEM_4;
				break;
			case SDL_SCANCODE_RIGHTBRACKET:vkey = VK_OEM_6;
				break;
			case SDL_SCANCODE_BACKSLASH:vkey = VK_OEM_5;
				break;
			case SDL_SCANCODE_SEMICOLON:vkey = VK_OEM_1;
				break;
			case SDL_SCANCODE_APOSTROPHE:vkey = 222;
				break;
			case SDL_SCANCODE_COMMA:vkey = VK_OEM_COMMA;
				break;
			case SDL_SCANCODE_PERIOD:vkey = VK_OEM_PERIOD;
				break;
			case SDL_SCANCODE_SLASH:vkey = VK_OEM_2;
				break;
			case SDL_SCANCODE_CAPSLOCK:vkey = VK_CAPITAL;
				break;
			case SDL_SCANCODE_PRINTSCREEN:vkey = VK_SNAPSHOT;
				break;
			case SDL_SCANCODE_SCROLLLOCK:vkey = VK_SCROLL;
				break;
			case SDL_SCANCODE_PAUSE:vkey = VK_PAUSE;
				break;
			case SDL_SCANCODE_INSERT:vkey = VK_INSERT;
				break;
			case SDL_SCANCODE_HOME:vkey = VK_HOME;
				break;
			case SDL_SCANCODE_PAGEUP:vkey = VK_PRIOR;
				break;
			case SDL_SCANCODE_DELETE:vkey = VK_DELETE;
				break;
			case SDL_SCANCODE_END:vkey = VK_END;
				break;
			case SDL_SCANCODE_PAGEDOWN:vkey = VK_NEXT;
				break;
			case SDL_SCANCODE_RIGHT:vkey = VK_RIGHT;
				break;
			case SDL_SCANCODE_LEFT:vkey = VK_LEFT;
				break;
			case SDL_SCANCODE_DOWN:vkey = VK_DOWN;
				break;
			case SDL_SCANCODE_UP:vkey = VK_UP;
				break;
			case SDL_SCANCODE_NUMLOCKCLEAR:vkey = VK_NUMLOCK;
				break;
			case SDL_SCANCODE_KP_DIVIDE:vkey = VK_DIVIDE;
				break;
			case SDL_SCANCODE_KP_MULTIPLY:vkey = VK_MULTIPLY;
				break;
			case SDL_SCANCODE_KP_MINUS:vkey = VK_OEM_MINUS;
				break;
			case SDL_SCANCODE_KP_PLUS:vkey = VK_OEM_PLUS;
				break;
			case SDL_SCANCODE_KP_ENTER:vkey = VK_RETURN;
				break;
			case SDL_SCANCODE_KP_PERIOD:vkey = VK_OEM_PERIOD;
				break;
				//--------------键盘底部 左边-------------------------
			case SDL_SCANCODE_LCTRL:vkey = VK_CONTROL;
				break;

			case SDL_SCANCODE_LSHIFT:vkey = VK_LSHIFT;
				break;

			case SDL_SCANCODE_LALT:vkey = VK_MENU;
				break;

			case SDL_SCANCODE_RCTRL:vkey = VK_CONTROL;
				break;
			case SDL_SCANCODE_RALT:vkey = VK_MENU;
				break;
			case SDL_SCANCODE_RSHIFT:vkey = VK_RSHIFT;
				break;
			default:
				break;
			}
	
			keybd_event(vkey,0, KEYEVENTF_KEYUP,  0);
			break;

		default:break;
		}

		//CreateThread(NULL, 0, udpproc, (LPVOID)lparam, 0, NULL);
		//::SendMessage(*((HWND*)lparam), WM_TCPMESSAGE, 123, (LPARAM)&szBuf);
	}
	free(eventbuffer);
	return 2;
}
//------------------------编码-----------------------------------------------------
int encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt)
{
	int ret;

	/* send the frame to the encoder */
	if (frame)
		//	printf("Send frame %d\n", frame->pts);

		ret = avcodec_send_frame(enc_ctx, frame);
	if (ret < 0) {

		return -1;
	}

	while (ret >= 0) {
		ret = avcodec_receive_packet(enc_ctx, pkt);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return -2;
		else if (ret < 0) {

			return -3;
		}
	//	fwrite(pkt->data, 1, pkt->size, outfile);
		bd.ptr = pkt->data;
		bd.size = pkt->size;

		//printf("Write packet %d ", pkt->pts);
		//printf("size= %d\n", pkt->size);
		return 233;
	}
}
//--------------------------------循环发送----------------------------
void cyclesend(void)
{
/*	struct buffer_data {
		uint8_t *ptr;
		size_t size; ///< size left in the buffer
	}bd;
	bd = *(buffer_data*)pParam;
	*/
	int actualsend;
	int totalsend = 0;
	/*
	char *sendbuffer;
	int cpsize = 4096;
	int leftsize = bd.size;
	sendbuffer = (char *)malloc(4096);
	*/
	while (1)
	{
		actualsend = send(tcp_s, (char *)bd.ptr + totalsend, bd.size - totalsend, 0);
		if (actualsend == SOCKET_ERROR)
		{
			online = 0;
			break;
		}
		totalsend += actualsend;
		//bd.ptr += actualsend;
		//bd.size -= actualsend;
		if (bd.size - totalsend <= 0)
			break;
	}

}
//-------------------------------截图--编码--发送----解码--显示------------------------
DWORD WINAPI screenshot(LPVOID pParam)
{
	online = 1;
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
	SDL_Event myevent;

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
	//----------send width height------------

	struct w_h
	{
		uint16_t width;
		uint16_t height;

	}screen_w_h;
	screen_w_h.width = nScreenWidth;
	screen_w_h.height = nScreenHeight;
	send(tcp_s, (char *)&screen_w_h, 4, 0);
	//----set width height----------
	int width_5 = nScreenWidth;
	int height_6 = nScreenHeight;



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
	screen = SDL_CreateWindow("send_screen", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		screen_w, screen_h,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (!screen) {
		//printf("SDL: could not create window - exiting:%s\n", SDL_GetError());
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
	img_convert_ctx = sws_getContext(width_5, height_6, AV_PIX_FMT_BGR24,
		width_5, height_6, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

	out_img_convert_ctx = sws_getContext(width_5, height_6, AV_PIX_FMT_YUV420P,
		width_5 / 2, height_6 / 2, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);



	yuv_buff = (uint8_t *)malloc(width_5 * height_6 * 1.5);
	decodec_yuv_buff = (uint8_t *)malloc(width_5 * height_6 * 1.5);
	out_yuv_buff = (uint8_t *)malloc(width_5 * height_6 * 1.5 / 4);


	m_pRGBFrame = av_frame_alloc();
	m_pYUVFrame = av_frame_alloc();
	decodec_Frame = av_frame_alloc();
	out_pYUVFrame = av_frame_alloc();

	m_pYUVFrame->format = AV_PIX_FMT_YUV420P;
	m_pYUVFrame->height = height_6;
	m_pYUVFrame->width = width_5;

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
	encodecctx->width = width_5;
	encodecctx->height = height_6;
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

	decodecctx->width = width_5;
	decodecctx->height = height_6;
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
		//	fprintf(stderr, "Could not open codec: %s\n");
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
	/*
	fopen_s(&outfile, "out.h264", "wb");
	if (!outfile) {
		//	printf("Could not open %s\n");
		exit(1);
	}
*/


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
			AV_PIX_FMT_BGR24, width_5, height_6, 1);

		av_image_fill_arrays(m_pYUVFrame->data, m_pYUVFrame->linesize, yuv_buff,
			AV_PIX_FMT_YUV420P, width_5, height_6, 1);

		av_image_fill_arrays(decodec_Frame->data, decodec_Frame->linesize, decodec_yuv_buff,
			AV_PIX_FMT_YUV420P, width_5, height_6, 1);

		av_image_fill_arrays(out_pYUVFrame->data, out_pYUVFrame->linesize, out_yuv_buff,
			AV_PIX_FMT_YUV420P, width_5 / 2, height_6 / 2, 1);

		// 翻转RGB图像  
		m_pRGBFrame->data[0] += m_pRGBFrame->linesize[0] * (height_6 - 1);
		m_pRGBFrame->linesize[0] *= -1;
		m_pRGBFrame->data[1] += m_pRGBFrame->linesize[1] * (height_6 / 2 - 1);
		m_pRGBFrame->linesize[1] *= -1;
		m_pRGBFrame->data[2] += m_pRGBFrame->linesize[2] * (height_6 / 2 - 1);
		m_pRGBFrame->linesize[2] *= -1;


		//将RGB转化为YUV  
		//将RGB转化为YUV  
		//sws_scale(scxt, m_pRGBFrame->data, m_pRGBFrame->linesize, 0, pCodecCtx->height, m_pYUVFrame->data, m_pYUVFrame->linesize);
		sws_scale(img_convert_ctx, (const unsigned char* const*)m_pRGBFrame->data, m_pRGBFrame->linesize, 0, height_6,
			m_pYUVFrame->data, m_pYUVFrame->linesize);

		//--------------编码-------------
		m_pYUVFrame->pts = i++;


		int issuccess = encode(encodecctx, m_pYUVFrame, packet);
		if (issuccess != 233)
			continue;
		cyclesend();
		//--------------发送-------------
	/*	struct buffer_data {
			uint8_t *ptr;
			size_t size; ///< size left in the buffer
		}send_bd;
		send_bd.ptr = (uint8_t*)malloc(bd.size);
		memcpy(send_bd.ptr, bd.ptr, bd.size);
		send_bd.size = bd.size;
		CreateThread(NULL, 0, cyclesend, (LPVOID)&send_bd, 0, NULL);
*/
		//Sleep(900);
		//----------------------解码--本地显示---------------------------


		cppkt->data = bd.ptr;
		cppkt->size = bd.size;
		ret = avcodec_send_packet(decodecctx, cppkt);
		got_picture = avcodec_receive_frame(decodecctx, decodec_Frame);
		if (got_picture != 0)
		{
			continue;
		}

		//-----------------大小转换------------------------
		sws_scale(out_img_convert_ctx, (const unsigned char* const*)decodec_Frame->data, decodec_Frame->linesize, 0, height_6,
			out_pYUVFrame->data, out_pYUVFrame->linesize);

		SDL_PollEvent(&myevent);
		SDL_UpdateTexture(sdlTexture, &sdlRect, out_pYUVFrame->data[0], out_pYUVFrame->linesize[0]);
		SDL_RenderClear(sdlRenderer);
		SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
		SDL_RenderPresent(sdlRenderer);
	
		if (!online)
			break;
	}
	free(yuv_buff);
	free(decodec_yuv_buff);
	free(out_yuv_buff);

	sws_freeContext(img_convert_ctx);
	sws_freeContext(out_img_convert_ctx);

	av_frame_free(&m_pRGBFrame);
	av_frame_free(&m_pYUVFrame);
	av_frame_free(&decodec_Frame);
	av_frame_free(&out_pYUVFrame);

	avcodec_close(encodecctx);
	avcodec_close(decodecctx);

	SDL_DestroyTexture(sdlTexture);
	SDL_DestroyRenderer(sdlRenderer);
	SDL_DestroyWindow(screen);

	::ReleaseDC(hDesktopWnd, hDesktopDC);
	DeleteDC(hCaptureDC);
	DeleteObject(hCaptureBitmap);
//	fclose(outfile);
	return -1;
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

														// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CudplearnDlg 对话框



CudplearnDlg::CudplearnDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_UDPLEARN_DIALOG, pParent)

{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CudplearnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	
}

BEGIN_MESSAGE_MAP(CudplearnDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT_VALUE, &CudplearnDlg::OnEnChangeEditValue)
	ON_BN_CLICKED(IDC_BUTTON1, &CudplearnDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CudplearnDlg::OnBnClickedButton2)
	ON_MESSAGE(WM_UDPMESSAGE, &CudplearnDlg::udpmessage)
END_MESSAGE_MAP()


// CudplearnDlg 消息处理程序

BOOL CudplearnDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

									// TODO: 在此添加额外的初始化代码
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CudplearnDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CudplearnDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CudplearnDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CudplearnDlg::OnEnChangeEditValue()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CudplearnDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(FALSE);
	CreateThread(NULL, 0, udpproc, (LPVOID)&m_hWnd, 0, NULL);

}



//-------------------------开始接受广播-------------------------------
void CudplearnDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	WSAStartup(MAKEWORD(2, 2), &wsad);
	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(4677);
	bBoardcast = TRUE;
	AfxMessageBox(_T("open socket"));
	if (SOCKET_ERROR == bind(s, (LPSOCKADDR)&addr, sizeof(addr)))
	{
		AfxMessageBox(_T("socket error"));

		/*if (INVALID_SOCKET != s)
		{

		closesocket(s);
		s = INVALID_SOCKET;
		*/
		WSACleanup();
	}
	memset(&remote, 0, sizeof(remote));
	//lparam = &m_hWnd;
	hthread = CreateThread(NULL, 0, udpproc, (LPVOID)&m_hWnd, 0, &threadid);

}
//-----------------------------收到UDP后辅助链接TCP-------------------
LRESULT CudplearnDlg::udpmessage(WPARAM wParam, LPARAM lParam)
{
	tcp_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (tcp_s == INVALID_SOCKET)
	{
		WSACleanup();
		//CreateThread(NULL, 0, udpproc, (LPVOID)&m_hWnd, 0, NULL);

		return 1;
	}
	server_addr.sin_family = peeraddr.sin_family;
	server_addr.sin_addr = peeraddr.sin_addr;
	server_addr.sin_port = peeraddr.sin_port;

	int iresult;
	iresult = connect(tcp_s, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (SOCKET_ERROR == iresult)
	{
		WSACleanup();
		//CreateThread(NULL, 0, udpproc, (LPVOID)&m_hWnd, 0, NULL);

		return 1;

	}
	CreateThread(NULL, 0, recvmanu, NULL, 0, NULL);
	CreateThread(NULL, 0, screenshot, NULL, 0, NULL);

}



