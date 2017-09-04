//
//  zy_video.h
//  player-ios
//
//  Created by zhangyun on 04/09/2017.
//  Copyright © 2017 zhangyun. All rights reserved.
//

#ifndef zy_video_h
#define zy_video_h

#include <stdio.h>
#include "zy_play_context.h"
#include "zy_packet_queue.h"
#include "zy_filter.h"
#include "zy_audio.h"

int prepare_video(ZYPlayerContext *ps);
int play_video(ZYPlayerContext *ps);

int decode_and_show(void *arg);

int refresh_fun(void *arg);
double get_frame_pts(ZYPlayerContext *ps, AVFrame *pframe);
double get_delay(ZYPlayerContext *ps);

#endif /* zy_video_h */
