//
// Created by administrator on 2021/7/15.
//
#include <jni.h>
#include "android/log.h"
#include "mp4v2/mp4v2.h"
#include <list>
#include "FileList.h"
#include "AssistFile.h"
#include <list>
#include <string>
#include <vector>


#define MAX_PATH                 1024

#define  LOG_TAG    "MP4Helper"
#define  LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__);
#define ASSISTINFO_CLASS "com/samoon/sammp4/bean/AssistInfo"

using namespace std;
using std::string;  //使用string对象
using std::vector;  //使用vector


/**
 * char* 转jstrong
 * @param env
 * @param s
 * @param pat
 * @return
 */
 jstring charToString(JNIEnv *env, jclass s,char* pat){
     //定义java String类 strClass
     jclass strClass = (env)->FindClass("java/lang/String");
     //获取String(byte[],String)的构造器,用于将本地byte[]数组转换为一个新String
     jmethodID ctorID = (env)->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
     //建立byte数组
     jbyteArray bytes = (env)->NewByteArray(strlen(pat));
     //将char* 转换为byte数组
     (env)->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*) pat);
     // 设置String, 保存语言类型,用于byte数组转换至String时的参数
     jstring encoding = (env)->NewStringUTF("GB2312");
     //将byte数组转换为java String,并输出
     return (jstring) (env)->NewObject(strClass, ctorID, bytes, encoding);
 }

jobject test(JNIEnv *env, jclass clazz,AssistInfo_t assistInfo ){
    //获取ArrayList类引用
    jclass list_jcs = env->FindClass("java/util/ArrayList");
    if (list_jcs == NULL) {
        LOGI("ArrayList no  find!");
        return NULL;
    }

    //获取ArrayList构造函数id
    jmethodID list_init = env->GetMethodID(list_jcs, "<init>", "()V");
    //创建一个ArrayList对象
    jobject list_obj = env->NewObject(list_jcs, list_init, "");
    //获取ArrayList对象的add()的methodID
    jmethodID list_add = env->GetMethodID(list_jcs, "add",
                                          "(Ljava/lang/Object;)Z");
    //获取AssistInfo类
    jclass assist_cls = env->FindClass(ASSISTINFO_CLASS);
    //获取AssistInfo无参数的构造函数
//    jmethodID assist_init = env->GetMethodID(assist_cls, "<init>",
//                                          "()V");

    //获取AssistInfo 带参数的构造函数
//    jmethodID assist_init = env->GetMethodID(assist_cls, "<init>",
//                                             "(FFFLjava/lang/String;Ljava/lang/String;ID)V");


    jmethodID assist_init = env->GetMethodID(assist_cls, "<init>",
                                             "(FFFLjava/lang/String;Ljava/lang/String;ID)V");

    jstring js_lat=charToString(env,clazz,const_cast<char *>(assistInfo.gps_lat.data()));
    jstring js_lgt=charToString(env,clazz,const_cast<char *>(assistInfo.gps_lgt.data()));


    jobject assis_obj = env->NewObject(assist_cls, assist_init,assistInfo.gsensor_x,assistInfo.gsensor_y,assistInfo.gsensor_z,js_lat,js_lgt,
                                       assistInfo.spd,
                                       assistInfo.north_angle);
    env->CallBooleanMethod(list_obj, list_add, assis_obj);
    return list_obj;
 }


extern "C"
list<AssistInfo_t> getGpsStream(MP4FileHandle oMp4File,int VTrackId,int totalFrame,FileNode_t node)
{
    list<AssistInfo_t> assistInfos;
    AssistInfo_t info;
    unsigned char *pData = NULL;
    unsigned int nSize = 0;
    MP4Timestamp pStartTime;
    MP4Duration pDuration;
    MP4Duration pRenderingOffset;
    bool pIsSyncSample = 0;
    int nReadIndex = 0;

//    QFile file("F:/gps_src111.text");
//    file.open(QIODevice::WriteOnly | QIODevice::Text);

    while(nReadIndex < totalFrame)
    {
        nReadIndex ++;
        MP4ReadSample(oMp4File,VTrackId,nReadIndex,&pData,&nSize,&pStartTime,&pDuration,&pRenderingOffset,&pIsSyncSample);
        //  fwrite(pData,nSize,1,pFile);

        char* c_data = (char*)pData;

        if(node.dev_type==DEV_A12)
        {

        }else if(node.dev_type==DEV_HISILICON)
        {
            hiSAMOON_INFO_PACKET *track = (hiSAMOON_INFO_PACKET*)c_data;
            if(track==0x0){
                continue;
            }

            unsigned char lng_ddd = track->sm_gps_info.lng_ddd;
            unsigned char lng_mm = track->sm_gps_info.lng_mm;
            unsigned short lng_ssss = track->sm_gps_info.lng_ssss ;
            unsigned char  lat_ddd = track->sm_gps_info.lat_ddd;		//Latitude degree (000 - 179)
            unsigned char  lat_mm = track->sm_gps_info.lat_mm;		//Latitude minute (00-99)
            unsigned short lat_ssss=track->sm_gps_info.lat_ssss ;		//Latitude second (0000 - 5999)
            unsigned char  lng_indicator = track->sm_gps_info.lng_indicator;
            unsigned char  lat_indicator = track->sm_gps_info.lat_indicator;

            //w(87 '-') n(78 '+')

            //add cpu Temperature
            //mCpuTmps<<QString::number(track->sm_gps_info.temp);

            //GPS温度 替换结构体后没有这个元素 测试完毕需要打开
            //mCpuTmps.append(track->sm_gps_info.temp/100.0);
            char gps_status = track->sm_gps_info.gps_status;
            if(gps_status=='A')
            {
                double d_gps_lgt;
                double d_gps_lat;
                if(lng_indicator==87/*W*/ || lng_indicator==119/*w*/){
                    //西径
                    double d_gps_lgt_tmp = lng_ddd+(lng_mm/60.0)+(lng_ssss/3600.0/100.0);     //经度
                    d_gps_lgt = -d_gps_lgt_tmp;
                }

                if(lat_indicator==115/*S*/ || lat_indicator==83/*s*/){
                    //南纬
                    double d_gps_lat_tmp =lat_ddd+(lat_mm/60.0)+(lat_ssss/3600.0/100.0);     //纬度
                    d_gps_lat=-d_gps_lat_tmp;
                }

                if(lng_indicator==69/*E*/ || lng_indicator==101 /*e*/){
                    //东经
                    d_gps_lgt = lng_ddd+(lng_mm/60.0)+(lng_ssss/3600.0/100.0);     //经度
                }

                if(lat_indicator==78/*N*/ || lat_indicator==110 /*n*/){
                    //北纬
                    d_gps_lat = lat_ddd+(lat_mm/60.0)+(lat_ssss/3600.0/100.0);     //纬度
                }

                info.gps_lat = d_gps_lat;
                info.gps_lgt = d_gps_lgt;
            }


            info.spd=track->sm_gps_info.speed;
            info.gsensor_x=track->sm_gps_info.x_acc;
            info.gsensor_y=track->sm_gps_info.y_acc;
            info.gsensor_z=track->sm_gps_info.z_acc;

            assistInfos.push_back(info);

        }else{

            AssistFile *assisFile = new AssistFile();
            assisFile->ParseAssistDataForNovatek9667XA("",60);


        }
        pData = NULL;

    }
    return assistInfos;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_samoon_sammp4_MP4Help_parseMp4File(JNIEnv *env, jclass clazz, jstring path)
{
    // TODO: implement parseMp4File()

    //获取ArrayList类引用
    jclass list_jcs = env->FindClass("java/util/ArrayList");
    if (list_jcs == NULL) {
        LOGI("ArrayList no  find!");
        return NULL;
    }

    //获取ArrayList构造函数id
    jmethodID list_init = env->GetMethodID(list_jcs, "<init>", "()V");
    //创建一个ArrayList对象
    jobject list_obj = env->NewObject(list_jcs, list_init, "");
    //获取ArrayList对象的add()的methodID
    jmethodID list_add = env->GetMethodID(list_jcs, "add",
                                          "(Ljava/lang/Object;)Z");
    //获取AssistInfo类
    jclass assist_cls = env->FindClass(ASSISTINFO_CLASS);
    //获取AssistInfo无参数的构造函数
//    jmethodID assist_init = env->GetMethodID(assist_cls, "<init>",
//                                          "()V");

    //获取AssistInfo 带参数的构造函数
//    jmethodID assist_init = env->GetMethodID(assist_cls, "<init>",
//                                             "(FFFLjava/lang/String;Ljava/lang/String;ID)V");


    jmethodID assist_init = env->GetMethodID(assist_cls, "<init>",
                                             "(FFFLjava/lang/String;Ljava/lang/String;ID)V");

    const char *c_path = env->GetStringUTFChars(path,NULL);

    FileNode_t node ;

    MP4FileHandle oMp4File;
    MP4EnableGPMDAtom(true);

    int i;
    //unsigned int oFrameCount;
    unsigned int oGpsCount ;
    oMp4File=MP4Read(c_path);
    int /*videoIndex = -1,audioIndex = -1,*/textIndex = -1;
    uint32_t numSamples;

    if(!oMp4File){
        return list_obj;
    }

    MP4TrackId trackId = MP4_INVALID_TRACK_ID;
    uint32_t numTracks = MP4GetNumberOfTracks(oMp4File,NULL,0);


    for(i=0;i<numTracks;i++)
    {
        trackId = MP4FindTrackId(oMp4File, i,NULL,0);
        const char* trackType = MP4GetTrackType(oMp4File, trackId);

        //text track;
        if(strcmp(trackType,MP4_TEXT_TRACK_TYPE)==0)
        {
            textIndex = trackId;
            oGpsCount = MP4GetTrackNumberOfSamples(oMp4File, trackId);
            node.dev_type =DEV_A12;

        }else if(strcmp(trackType,"meta")==0)
        {
            textIndex = trackId;
            oGpsCount = MP4GetTrackNumberOfSamples(oMp4File, trackId);
            node.dev_type =DEV_HISILICON;

        }else{
            textIndex = trackId;
            oGpsCount = MP4GetTrackNumberOfSamples(oMp4File, trackId);
            node.dev_type =DEV_NT9667XA;
        }
    }

    if(textIndex>0){

        AssistFile *assisFile = new AssistFile();
        assisFile->ParseAssistDataForNovatek9667XA(c_path,1);


#if 0
        list<AssistInfo_t> assistInfos=  getGpsStream(oMp4File,textIndex,oGpsCount,node);
        list<AssistInfo_t>::iterator itor;
        itor=assistInfos.begin();
        while(itor!=assistInfos.end())
        {
            *itor++;
            float gx = itor->gsensor_x;
            float gy = itor->gsensor_y;
            float gz = itor->gsensor_z;
            string lat = itor->gps_lat;
            string lgt = itor->gps_lgt;
            int spd = itor->spd;
            double angle = itor->north_angle;


            jstring js_lat=charToString(env,clazz,const_cast<char *>(lat.data()));
            jstring js_lgt=charToString(env,clazz,const_cast<char *>(lgt.data()));


            jobject assis_obj = env->NewObject(assist_cls, assist_init,gx,gy,gz,js_lat,js_lgt,
                                             spd,
                                             angle);
            env->CallBooleanMethod(list_obj, list_add, assis_obj);

        }
#endif

    }
    return list_obj;

}




extern "C"
JNIEXPORT jobject JNICALL
Java_com_samoon_sammp4_MP4Help_checkPlatform(JNIEnv *env, jclass clazz, jstring path) {
    // TODO: implement checkPlatform()

    //获取ArrayList类引用
    jclass list_jcs = env->FindClass("java/util/ArrayList");
    if (list_jcs == NULL) {
        LOGI("ArrayList no  find!");
        return NULL;
    }

    //获取ArrayList构造函数id
    jmethodID list_init = env->GetMethodID(list_jcs, "<init>", "()V");
    //创建一个ArrayList对象
    jobject list_obj = env->NewObject(list_jcs, list_init, "");
    //获取ArrayList对象的add()的methodID
    jmethodID list_add = env->GetMethodID(list_jcs, "add",
                                          "(Ljava/lang/Object;)Z");
    //获取AssistInfo类
    jclass assist_cls = env->FindClass(ASSISTINFO_CLASS);

    jmethodID assist_init = env->GetMethodID(assist_cls, "<init>",
                                             "(FFFLjava/lang/String;Ljava/lang/String;ID)V");

    int dev_type = -1;
    int NT9667XAItemNum = 0;
    char buf[16] = {0};
    const char *c_path = env->GetStringUTFChars(path,NULL);
    MP4FileHandle hMp4file = MP4Read(c_path);
    char gps_buf[5] = {0};
    list<AssistInfo_t> assistInfos;
    if(MP4HaveAtom(hMp4file,"samn")){
        dev_type=DEV_HISILICON;
        uint8_t *value;
        uint32_t valuelen;
        bool success;
        uint64_t  customer_id =0;
        uint64_t  product_model =0;

        success = MP4GetBytesProperty (hMp4file, "samn.reserved1", &value, &valuelen);
        MP4GetIntegerProperty(hMp4file, "samn.customer_id", &customer_id);
        MP4GetIntegerProperty (hMp4file, "samn.product_model", &product_model);

        MP4Close(hMp4file);
        return NULL;
    }else if(MP4HaveAtom(hMp4file,"moov.gps ")) {
        uint64_t version = 0;
        uint64_t length = 0;
        bool success;
        MP4GetIntegerProperty(hMp4file, "moov.gps .version", &version);
        success = MP4GetIntegerProperty(hMp4file, "moov.gps .length", &length);
        if (success) {
            NT9667XAItemNum = length;
            dev_type = DEV_NT9667XA;

            AssistFile *assisFile = new AssistFile();
            int count = assisFile->ParseAssistDataForNovatek9667XA(c_path, NT9667XAItemNum);
            AssistInfoList vectors = *assisFile->GetAssistInfoList();
            int size = vectors.size();
            vector<AssistInfo_t>::iterator iter;
            int i = 0;
            for(iter=vectors.begin();iter!=vectors.end();iter++){
                i++;
                AssistInfo_t info=*iter;
                //LOGI("gps coordinates:%s,%s",info.gps_lgt.data(),info.gps_lat.data());

                jstring  js_lat = env->NewStringUTF(info.gps_lat.data());
                jstring  js_lgt = env->NewStringUTF(info.gps_lgt.data());

                jobject assis_obj = env->NewObject(assist_cls, assist_init,info.gsensor_x,info.gsensor_y,info.gsensor_z,js_lat,js_lgt,
                                                   info.spd,
                                                   info.north_angle);
                env->CallBooleanMethod(list_obj, list_add, assis_obj);

            }
            MP4Close(hMp4file);
            return list_obj;
        }
    }
    return list_obj;
}



unsigned char sps[64],pps[64];
int spslen = 0,ppslen = 0;

int get264stream(MP4FileHandle oMp4File,int VTrackId,int totalFrame,const char* outPath){
    if(!oMp4File) return -1;
    char NAL[5] = {0x00,0x00,0x00,0x01};
    unsigned char *pData = NULL;
    unsigned int nSize = 0;
    MP4Timestamp pStartTime;
    MP4Duration pDuration;
    MP4Duration pRenderingOffset;
    bool pIsSyncSample = 0;
    int nReadIndex = 0;
    FILE *pFile = NULL;
    pFile = fopen(/*"/sdcard/test_out.h264"*/outPath,"wb");
    if(pFile==NULL){
        LOGI("the file is empty");
        return -1;
    }
    while(nReadIndex < totalFrame)
    {
        nReadIndex ++;
        //printf("nReadIndex:%d\n",nReadIndex);
        MP4ReadSample(oMp4File,VTrackId,nReadIndex,&pData,&nSize,&pStartTime,&pDuration,&pRenderingOffset,&pIsSyncSample);

        //IDR֡ 帧，写入sps pps先
        if(pIsSyncSample)
        {
          //  fwrite(NAL,4,1,pFile);
           // fwrite(sps,spslen,1,pFile);

           // fwrite(NAL,4,1,pFile);
            //fwrite(pps,ppslen,1,pFile);

            fwrite(NAL,4,1,pFile);
            fwrite(sps,spslen,1,pFile);

            fwrite(NAL,4,1,pFile);
            fwrite(pps,ppslen,1,pFile);


        }
        //264frame
        if(pData && nSize > 4)
        {
            //标准的264帧，前面几个字节就是frame的长度.
            //需要替换为标准的264 nal 头.
            pData[0] = 0x00;
            pData[1] = 0x00;
            pData[2] = 0x00;
            pData[3] = 0x01;
           // fwrite(pData,nSize,1,pFile);
            fwrite(pData,nSize,1,pFile);
        }

        //如果传入MP4ReadSample的视频pData是null
        // 它内部就会new 一个内存
        //如果传入的是已知的内存区域，
        //则需要保证空间bigger then max frames size.
        free(pData);
        pData = NULL;
    }
    fflush(pFile);
    fclose(pFile);

    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_samoon_sammp4_MP4Help_testMp42H264(JNIEnv *env, jclass clazz, jstring path,jstring outPath) {
    // TODO: implement testMp42H264()
    const char *sMp4file = env->GetStringUTFChars(path,NULL);
    const  char *sOutMp4file = env->GetStringUTFChars(outPath,NULL);
    MP4FileHandle oMp4File;
    int i;
    //unsigned int oStreamDuration;
    unsigned int oFrameCount;
    oMp4File = MP4Read(sMp4file);
    int videoindex = -1,audioindex = -1;
    uint32_t numSamples;
    if (!oMp4File)
    {
        LOGI("Read error....%s\r\n",sMp4file);
        return -1;
    }

    MP4TrackId trackId = MP4_INVALID_TRACK_ID;
    uint32_t numTracks = MP4GetNumberOfTracks(oMp4File,NULL,0);
    printf("numTracks:%d\n",numTracks);

    for (i = 0; i < numTracks; i++){
        trackId = MP4FindTrackId(oMp4File, i,NULL,0);
        const char* trackType = MP4GetTrackType(oMp4File, trackId);
        if (MP4_IS_VIDEO_TRACK_TYPE(trackType)){
            videoindex= trackId;
            numSamples = MP4GetTrackNumberOfSamples(oMp4File, trackId);
            oFrameCount = numSamples;
            // read sps/pps
            uint8_t **seqheader;
            uint8_t **pictheader;
            uint32_t *pictheadersize;
            uint32_t *seqheadersize;
            uint32_t ix;
            MP4GetTrackH264SeqPictHeaders(oMp4File, trackId, &seqheader, &seqheadersize, &pictheader, &pictheadersize);
            for (ix = 0; seqheadersize[ix] != 0; ix++)
            {
                memcpy(sps, seqheader[ix], seqheadersize[ix]);
                spslen = seqheadersize[ix];
                free(seqheader[ix]);
            }
            free(seqheader);
            free(seqheadersize);
            for (ix = 0; pictheadersize[ix] != 0; ix++)
            {
                memcpy(pps, pictheader[ix], pictheadersize[ix]);
                ppslen = pictheadersize[ix];
                free(pictheader[ix]);
            }
            free(pictheader);
            free(pictheadersize);
        }
    }
    LOGI("----------->oFrameCount:%d",oFrameCount);
    if(videoindex >= 0)
        get264stream(oMp4File,videoindex,oFrameCount,sOutMp4file);

    MP4Close(oMp4File,0);
    return 0;
}