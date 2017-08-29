/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : player_v1.3.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/10/06
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

/*-----------------------------------------------------------*
 * 头文件                                                    *
 *-----------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#define __STDC_CONSTANT_MACROS      //ffmpeg要求
#ifdef __cplusplus
extern "C" {
#endif

#include "player.h"
#include "wrap_base.h"
#include "packet_queue.h"
#include "audio.h"
#include "video.h"


int playSync()
{
     SDL_Event      event;
     PlayerState    *ps = NULL;

     ps = (PlayerState *)av_malloc(sizeof(PlayerState));
     if (ps == NULL){
          fprintf(ERR_STREAM, "malloc ps error\n");
     }

     player_state_init(ps);

//     memcpy(ps->filename, FILE_NAME, strlen(FILE_NAME));
//     get_file_name(ps->filename, argc, argv);

     char *file = "/Users/zhangyun/Downloads/BBC.Wild.China.6.Tides.of.Change.美丽中国之六潮汐更迭.双语字幕.HR-HDTV.AC3.960×528.X264-人人影视制作.avi";
//     char *file = "/Users/zhangyun/Downloads/player_v1.5/naxienian.mp4";
     memcpy(ps->filename,file,strlen(file)+1);

     if (prepare_common(ps) != 0){
          fprintf(ERR_STREAM, "prepare common error\n");
          exit(-1);
     }

	 av_dump_format(ps->pformat_ctx, 0, ps->filename, 0);
     //至少有一种流，读流，解码。

    if (ps->audio_stream_index != -1){
    	 packet_queue_init(&ps->audio_packet_queue);
    	 prepare_audio(ps);
         play_audio(ps);
    }

    if ( ps->video_stream_index != -1)
    {
    	packet_queue_init(&ps->video_packet_queue);
    	prepare_video(ps);
    	play_video(ps);
    }

	SDL_CreateThread(decode_thread, "decode_thread", ps);

    while(1)
    {
        if (ps->quit == 1)
        {
            break;
        }

    	SDL_WaitEvent(&event);
        switch(event.type)
        {
            case ISSHE_REFRESH_EVENT:
            {
                decode_and_show(ps);
                break;
            }
            case SDL_WINDOWEVENT:
            {   SDL_GetWindowSize(ps->pwindow, &ps->window_w, &ps->window_h);
                break;
            }
            case SDL_QUIT:
            {
            	printf("SDL_QUIT！\n");
            	ps->quit = 1;
                SDL_Quit();
                break;
            }
            default:
            {
                break;
        	}
        }

    }

    //
    if (ps->video_stream_index != -1)
    {
    	sws_freeContext(ps->psws_ctx);
    }
    return 0;
}


int prepare_common(PlayerState *ps)
{

    av_register_all();
    if (SDL_Init(SDL_INIT_VIDEO |SDL_INIT_AUDIO | SDL_INIT_TIMER))
    {
         fprintf(ERR_STREAM, "init SDL error: %s\n", SDL_GetError());
         return -1;
    }

    //打开文件
    //pformat_ctx会有所指向，不用分配内存
    if (avformat_open_input(&ps->pformat_ctx, ps->filename, NULL, NULL) != 0)
    {
         fprintf(ERR_STREAM, "open input file error\n");
         return -1;
    }

    if (avformat_find_stream_info(ps->pformat_ctx, NULL) < 0)
    {
         fprintf(ERR_STREAM, "Couldn't find stream info\n");
         return -1;
    }

    ps->video_stream_index = -1;
    ps->audio_stream_index = -1;
    if (find_stream_index(ps->pformat_ctx,
                &ps->video_stream_index,
                &ps->audio_stream_index) == -2)
    {
         fprintf(ERR_STREAM, "Couldn't find any stream index\n");
         return -1;
    }

    return 0;
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/06
* Others     (其他): 只负责从流中读取packet
\*=======================================================================*/
int decode_thread(void *arg)
{
    PlayerState *ps = (PlayerState *)arg;
    AVPacket    *packet = av_packet_alloc();

    while(1)
    {
         if (ps->quit == 1)
         {
             break;
         }

         //如果队列数据过多，就等待以下
         if (ps->audio_packet_queue.nb_packets >=MAX_AUDIO_QUEUE_SIZE ||
         	 ps->video_packet_queue.nb_packets >= MAX_VIDEO_QUEUE_SIZE)
         {
         	  printf("过多数据，延时\n");
              SDL_Delay(100);
              continue;
         }

         if (av_read_frame(ps->pformat_ctx, packet) < 0)
         {
             if ((ps->pformat_ctx->pb->error) == 0)
             {
             	  printf("读完延时\n");
                  SDL_Delay(100); //不是出错，可能是读完了
//                  ps->quit = 1;
                  break;//continue;
             }
             else
             {
             	 printf("出错，延时\n");
                 continue; 		//出错了，继续读，这里
             }
         }

         //读取到数据了
         if (packet->stream_index == ps->video_stream_index)
         {
              packet_queue_put(&ps->video_packet_queue, packet);
         }

         if (packet->stream_index == ps->audio_stream_index)
         {
             packet_queue_put(&ps->audio_packet_queue, packet);
         }

    }

    av_packet_free(&packet);
    return 0;
}


void player_state_init(PlayerState *ps)
{
	ps->pformat_ctx 		= NULL;
	ps->quit 				= 0;

    ps->audio_stream_index 	= -1;
    ps->paudio_stream 		= NULL;

    ps->paudio_codec_ctx 	= NULL;
    ps->paudio_codec 		= NULL;
    ps->audio_buf_size		= 0;
    ps->audio_buf_index 	= 0;
     //视频
    ps->video_stream_index 		= -1;
    ps->pvideo_stream 			= NULL;
    ps->pvideo_codec_ctx 		= NULL;
    ps->pvideo_codec 			= NULL;
    ps->video_buf				= NULL;
    ps->video_buf_size 			= 0;
    ps->video_buf_index 		= 0;
    ps->psws_ctx 				= NULL;

    ps->pixel_w 				= 0;
    ps->pixel_h 				= 0;
    ps->window_w 				= 0;
    ps->window_h 				= 0;

    ps->pwindow 				= NULL;
    ps->prenderer 				= NULL;
    ps->ptexture 				= NULL;
    ps->pixfmt 					= AV_PIX_FMT_YUV420P;

    ps->audio_clock 				= 0.0;
    ps->video_clock 				= 0.0;
    ps->pre_frame_pts				= 0.0;		//前一帧显示时间
 //   ps->cur_frame_pkt_pts			= 0.0; 		//当前帧在packet中标记的pts
    ps->pre_cur_frame_delay 		= 40e-3; 	//当前帧和前一帧的延时，前面两个相减的结果
    ps->cur_frame_pts 				= 0.0;		//packet.pts
    ps->delay 						= 40;


//    ps->frame_timer 				= (double)av_gettime()/1000000.0;
}


#ifdef __cplusplus
}
#endif
