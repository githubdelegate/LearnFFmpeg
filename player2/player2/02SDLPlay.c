////
////  02SDLPlay.c
////  player2
////
////  Created by zhangyun on 10/08/2017.
////  Copyright © 2017 zhangyun. All rights reserved.
////
//
//#include "02SDLPlay.h"
//
//#ifndef ffmpeg_h
//#define ffmpeg_h
//#include "avcodec.h"
//#include "avformat.h"
//#include "avfilter.h"
//#include "swscale.h"
//#endif
//
//
//#ifndef sdl_h
//#define sdl_h
//#include "SDL.h"
//#include "SDL_thread.h"
//#endif
//
//
//void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) {
//    FILE *pFile;
//    char szFilename[32];
//    int  y;
//    
//    // Open file
//    sprintf(szFilename, "frame%d.ppm", iFrame);
//    pFile=fopen(szFilename, "wb");
//    printf("%s", szFilename);
//    if(pFile==NULL)
//        return;
//    
//    // Write header
//    fprintf(pFile, "P6\n%d %d\n255\n", width, height);
//    
//    // Write pixel data
//    for(y=0; y<height; y++)
//        fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);
//    
//    // Close file
//    fclose(pFile);
//}
//
//
//int playSDL2(){
//    AVFormatContext *pFormatCtx = NULL;
//    int videoStream;
//    unsigned i;
//    AVCodecContext *pCodecCtxOrig = NULL;
//    AVCodecContext *pCodecCtx = NULL;
//    AVCodec *pCodec = NULL;
//    AVFrame *pFrame = NULL;
//    AVPacket packet;
//    struct SwsContext *sws_ctx = NULL;
//    SDL_Event event;
//    SDL_Window *screen;
//    SDL_Renderer *renderer;
//    SDL_Texture *texture;
//    Uint8 *yPlane, *uPlane, *vPlane;
//    size_t yPlaneSz, uvPlaneSz;
//    int uvPitch;
//    
//    // 1. 设置文件路径
////        char *filepath = "/Users/zhangyun/Documents/ffmpegtest.mp4";
//    char *filepath = "/Users/zhangyun/Downloads/BBC.Wild.China.6.Tides.of.Change.美丽中国之六潮汐更迭.双语字幕.HR-HDTV.AC3.960×528.X264-人人影视制作.avi";
//    // 2. 注册全部的mutex demutex
//    // Register all formats and codecs
//    av_register_all();
//    
//    // 3. 初始化SDL
//    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
//        fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
//        exit(1);
//    }
//    
//    // 4. 根据路径 打开文件流 并获取头部信息
//    // Open video file
//    if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0)
//        return -1; // Couldn't open file
//
//    /*
//     Read packets of a media file to get stream information. This is useful for file formats with no headers such as MPEG. This function also computes the re≥al framerate in case of MPEG-2 repeat frame mode. The logical file position is not changed by this function; examined packets may be buffered for later processing.
//     Note
//     this function isn't guaranteed to open all the codecs, so options being non-empty at return is a perfectly normal behavior.
//     To Do
//     Let the user decide somehow what information is needed so that we do not waste time getting stuff the user does not need.
//     
//     */
//    // Retrieve stream information
//    if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
//        return -1; // Couldn't find stream information
//    
//    // Dump information about file onto standard error
//    av_dump_format(pFormatCtx, 0, filepath, 0);
//    
//    // 6.Find the first video stream
//    videoStream = -1;
//    for (i = 0; i < pFormatCtx->nb_streams; i++)
//        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
//            videoStream = i;
//            break;
//        }
//    if (videoStream == -1)
//        return -1; // Didn't find a video stream
//    
//    // 7. 创建 code 上下文
//    pCodecCtxOrig = av_malloc(sizeof(AVCodecContext));
//    avcodec_parameters_to_context(pCodecCtxOrig,pFormatCtx->streams[videoStream]->codecpar);
//    
//    // 8. 查找解码器
//    pCodec = avcodec_find_decoder(pCodecCtxOrig->codec_id);
//    if (pCodec == NULL) {
//        fprintf(stderr, "Unsupported codec!\n");
//        return -1; // Codec not found
//    }
//    
//    // Copy context
//    pCodecCtx = avcodec_alloc_context3(pCodec);
//    if (avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoStream]->codecpar)!= 0) {
//        fprintf(stderr, "Couldn't copy codec context");
//        return -1; // Error copying codec context
//    }
//    
//    /*
//     Initialize the AVCodecContext to use the given AVCodec. Prior to using this function the context has to be allocated with avcodec_alloc_context3().
//     The functions avcodec_find_decoder_by_name(), avcodec_find_encoder_by_name(), avcodec_find_decoder() and avcodec_find_encoder() provide an easy way for retrieving a codec.
//     */
//    // 9. 打开解码器
//    // Open codec
//    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
//        return -1; // Could not open codec
//    
//    // Allocate video frame
//    pFrame = av_frame_alloc();
//    
//    // Make a screen to put our video
//    screen = SDL_CreateWindow(
//                              "FFmpeg Tutorial",
//                              SDL_WINDOWPOS_UNDEFINED,
//                              SDL_WINDOWPOS_UNDEFINED,
//                              pCodecCtx->width,
//                              pCodecCtx->height,
//                              0
//                              );
//    
//    if (!screen) {
//        fprintf(stderr, "SDL: could not create window - exiting\n");
//        exit(1);
//    }
//    
//    renderer = SDL_CreateRenderer(screen, -1, 0);
//    if (!renderer) {
//        fprintf(stderr, "SDL: could not create renderer - exiting\n");
//        exit(1);
//    }
//    
//    // Allocate a place to put our YUV image on that screen
//    texture = SDL_CreateTexture(
//                                renderer,
//                                SDL_PIXELFORMAT_YV12,
//                                SDL_TEXTUREACCESS_STREAMING,
//                                pCodecCtx->width,
//                                pCodecCtx->height
//                                );
//    if (!texture) {
//        fprintf(stderr, "SDL: could not create texture - exiting\n");
//        exit(1);
//    }
//    
//    // initialize SWS context for software scaling
//    sws_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
//                             pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
//                             AV_PIX_FMT_YUV420P,
//                             SWS_BILINEAR,
//                             NULL,
//                             NULL,
//                             NULL);
//    
//    // set up YV12 pixel array (12 bits per pixel)
//    yPlaneSz = pCodecCtx->width * pCodecCtx->height;
//    uvPlaneSz = pCodecCtx->width * pCodecCtx->height / 4;
//    yPlane = (Uint8*)malloc(yPlaneSz);
//    uPlane = (Uint8*)malloc(uvPlaneSz);
//    vPlane = (Uint8*)malloc(uvPlaneSz);
//    if (!yPlane || !uPlane || !vPlane) {
//        fprintf(stderr, "Could not allocate pixel buffers - exiting\n");
//        exit(1);
//    }
//    
//    int ret = 0;
//    uvPitch = pCodecCtx->width / 2;
//    
//    /*
//     
//     Return the next frame of a stream. This function returns what is stored in the file, and does not validate that what is there are valid frames for the decoder. It will split what is stored in the file into frames and return one for each call. It will not omit invalid data between valid frames so as to give the decoder the maximum information possible for decoding.
//     If pkt->buf is NULL, then the packet is valid until the next av_read_frame() or until avformat_close_input(). Otherwise the packet is valid indefinitely. In both cases the packet must be freed with av_packet_unref when it is no longer needed. For video, the packet contains exactly one frame. For audio, it contains an integer number of frames if each frame has a known fixed size (e.g. PCM or ADPCM data). If the audio frames have a variable size (e.g. MPEG audio), then it contains one frame.
//     pkt->pts, pkt->dts and pkt->duration are always set to correct values in AVStream.time_base units (and guessed if the format cannot provide them). pkt->pts can be AV_NOPTS_VALUE if the video format has B-frames, so it is better to rely on pkt->dts if you do not decompress the payload.
//     
//     */
//    while (av_read_frame(pFormatCtx, &packet) >= 0) {
//        // Is this a packet from the video stream?
//        if (packet.stream_index == videoStream) {
//            // Decode video frame
//            ret = avcodec_send_packet(pCodecCtx,&packet);
//            if (ret < 0) {
//                fprintf(stderr, "send packet error");
//            }
//            while (ret >= 0) {
//                ret = avcodec_receive_frame(pCodecCtx, pFrame);
//                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
//                    break;
//                }else if (ret < 0){
//                    fprintf(stderr, "receive frame error");
//                }
//                
//                if (ret >= 0) {
//                    uint8_t *dst[3];
//                    dst[0] = yPlane;
//                    dst[1] = uPlane;
//                    dst[2] = vPlane;
//                    
//                    int size[3];
//                    size[0] = pCodecCtx->width;
//                    size[1] = uvPitch;
//                    size[2] = uvPitch;
//                    
//                    // Convert the image into YUV format that SDL uses
//                    int oh = sws_scale(sws_ctx, (uint8_t const * const *) pFrame->data,
//                              pFrame->linesize, 0, pCodecCtx->height, dst,
//                              size);
//                    
//                    if (oh < 0) {
//                        fprintf(stderr, "sws scale errro");
//                    }
//                    
////                    SDL_Rect *rect = malloc(sizeof(SDL_Rect));
////                    int r = random() % 2;
////                    if (r == 0) {
////                        rect->x = 0;
////                        rect->y = 0;
////                        rect->w = 100;
////                        rect->h = 100;
////                    }else if(r == 1){
////                        rect->x = 100;
////                        rect->y = 100;
////                        rect->w = 100;
////                        rect->h = 100;
////                    }else{
////                        rect->x = 200;
////                        rect->y = 200;
////                        rect->w = 100;
////                        rect->h = 100;
////                    }
//                    
//                 int res = SDL_UpdateYUVTexture(
//                                         texture,
//                                         NULL,
//                                         yPlane,
//                                         pCodecCtx->width,
//                                         uPlane,
//                                         uvPitch,
//                                         vPlane,
//                                         uvPitch
//                                         );
//                    
//                    if (res != 0) {
//                        fprintf(stderr, "sdl update texture error");
//                    }
//                    SDL_RenderClear(renderer);
//                    SDL_RenderCopy(renderer, texture, NULL, NULL);
//                    SDL_RenderPresent(renderer);
//                }
//            }
//        }
//        // Free the packet that was allocated by av_read_frame
//        av_packet_unref(&packet);
//        SDL_PollEvent(&event);
//        switch (event.type) {
//            case SDL_QUIT:
//                SDL_DestroyTexture(texture);
//                SDL_DestroyRenderer(renderer);
//                SDL_DestroyWindow(screen);
//                SDL_Quit();
//                exit(0);
//                break;
//            default:
//                break;
//        }
//        
//    }// while
//    
//    // Free the YUV frame
//    av_frame_free(&pFrame);
//    free(yPlane);
//    free(uPlane);
//    free(vPlane);
//    
//    // Close the codec
//    avcodec_close(pCodecCtx);
//    avcodec_close(pCodecCtxOrig);
//    
//    // Close the video file
//    avformat_close_input(&pFormatCtx);
//    return 0;
//}
