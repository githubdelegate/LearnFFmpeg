//
//  zy_audio.h
//  player-ios
//
//  Created by zhangyun on 04/09/2017.
//  Copyright © 2017 zhangyun. All rights reserved.
//

#ifndef zy_audio_h
#define zy_audio_h

#include <stdio.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/time.h>
#include <SDL.h>
#include "zy_packet_queue.h"
#include "zy_play_context.h"

int prepare_audio(ZYPlayerContext *pCtx);
int play_audio(ZYPlayerContext *ps);
double get_audio_clock(ZYPlayerContext *ps);

#endif /* zy_audio_h */
