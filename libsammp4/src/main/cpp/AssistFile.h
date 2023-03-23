//
// Created by administrator on 2021/7/15.
//

#ifndef YEECORECAMVIEWER_ANDROID_ASSISTFILE_H
#define YEECORECAMVIEWER_ANDROID_ASSISTFILE_H


#include <string>
#include <vector>

using namespace std;
using std::string;  //使用string对象
using std::vector;  //使用vector

typedef void *       HI_MW_PTR;

typedef enum {
    HI_FALSE = 0,
    HI_TRUE  = 1,
} HI_BOOL;

typedef unsigned char           HI_U8;
typedef unsigned short          HI_U16;
typedef unsigned int            HI_U32;

typedef signed char             HI_S8;
typedef short                   HI_S16;
typedef int                     HI_S32;

typedef unsigned long           HI_UL;
typedef signed long             HI_SL;

/*float*/
typedef float               	HI_FLOAT;
/*double*/
typedef double                  HI_DOUBLE;


typedef unsigned long long  HI_U64;
typedef long long           HI_S64;



typedef struct FileInfo_s {		//Total: 76Bytes

    unsigned int    magic_head;/*初始化固定，不能修改*/
    char   brand[12];
    char   model[8];
    char   sw_ver[12];
    char   serialno[12];
    char   encrypt_key[12];
    unsigned char   metadata_freq;
    char   reserve[11];
    unsigned int    magic_tail;/*初始化固定，不能修改*/
}FileInfo_t;

typedef struct GPSInfo_s{		//Total: 196Bytes

#if 0       //测试代码
    //File Information
     unsigned int    magic_head;
     char   brand[12];
     char   model[8];
     char   sw_ver[12];
     char   serialno[12];
     char   resv[24];
     unsigned int    magic_tail;

     //Magic struct : 8Bytes
     unsigned int    magic_head1;
     unsigned int    magic_head2;

     //Version
     unsigned char   version;		//1-255: V1-255
     char	        reserve[11];

     //Date & Time : 8Bytes
     unsigned short	year;
     unsigned char   mon;
     unsigned char   day;
     unsigned char   hour;
     unsigned char   min;
     unsigned char   sec;
     signed char       timezone;/*-12~+12*/

     //GSensor Info: 16Bytes
     unsigned char   range;		//0x02: 2G  0x04: 4G 0x08: 8G 0x10: 16G
     unsigned char   reserve1[3];
     float	            x_acc;
     float	            y_acc;
     float	            z_acc;

     //GPSInfo: 32Bytes
     char	        gps_status;		//'A', 'V'
     char 	        lng_indicator;		//'E': East, 'W', West
     char	        lat_indicator;		//'N': North, 'S': South
     unsigned char   lng_ddd;		//Longitude degree (000 - 179)
     unsigned char   lng_mm;		//Longitude minute (00 - 99)
     unsigned short	lng_ssss;		//Longitude second (0000 - 59999)
     unsigned char   lat_ddd;		//Latitude degree (000 - 179)
     unsigned char   lat_mm;		//Latitude minute (00-99)
     unsigned short	lat_ssss;		//Latitude second (0000 - 5999)
     unsigned short	speed;		//km/h
     unsigned short	angle;
     unsigned char   reserve2[17];

     //Reserve for feature functions
     unsigned char   reserve3[112];	//Reserved for device info

     //Magic tail: 8Bytes
     unsigned int    magic_tail1;
     unsigned int    magic_tail2;
#endif

#if 1
    //Magic struct : 8Bytes
    unsigned int    magic_head1;
    unsigned int    magic_head2;

    //Version & Marker & Key: 12Bytes
    unsigned char   version;		//1-255: V1-255
    unsigned char   important_mark;	//Marker:event trig flag:see SM_GPSINFO_IMPORTANT_MARK_e
    char	        reserve[10];

    //Date & Time : 8Bytes
    unsigned short	year;
    unsigned char   mon;
    unsigned char   day;
    unsigned char   hour;
    unsigned char   min;
    unsigned char   sec;
    signed char	    timezone;/*-12~+12*/

    //GSensor Info: 16Bytes
    unsigned char   range;		//0x02: 2G  0x04: 4G 0x08: 8G 0x10: 16G
    unsigned char   reserve1[3];
    float	            x_acc; /*使用g值*/
    float	            y_acc;
    float	            z_acc;

    //GPSInfo: 32Bytes
    char	        gps_status;	//'A', 'V'
    char 	        lng_indicator;	//'E': East, 'W', West
    char	        lat_indicator;	//'N': North, 'S': South
    unsigned char   lng_ddd;		//Longitude degree (000 - 179)
    unsigned char   lng_mm;		//Longitude minute (00 - 99)
    unsigned short	lng_ssss;		//Longitude second (0000 - 59999)
    unsigned char   lat_ddd;		//Latitude degree (000 - 179)
    unsigned char   lat_mm;		//Latitude minute (00-99)
    unsigned short	lat_ssss;		//Latitude second (0000 - 5999)
    unsigned short	speed;		//km/h
    unsigned short	angle;
    unsigned char   reserve2[17];

    //Device Info:  32Bytes
    unsigned char   temp;		//CPU Temperature(0-255)
    unsigned short	ntc;		//NTC Value (0-1204)
    unsigned char   acc;		//ACC Status(0: Off 1: On)
    unsigned char   md;		//MotionDetection
    unsigned char   reserve3[27];	//Reserved for device info

    //Debug Info: 32Bytes
    unsigned char   reserve4[32];

    //Reserve: 48Bytes
    unsigned char   reserve5[48];

    //Magic tail: 8Bytes
    unsigned int    magic_tail1;
    unsigned int    magic_tail2;
#endif
} GPSInfo_t;


//typedef struct{
//    uint32_t   Hour;
//    uint32_t Minute;
//    uint32_t Second;
//    char Status;
//    double Latitude;
//    char NSInd;
//    double Longitude;
//    char EWInd;
//    double Speed;
//    double Angle;
//    uint32_t Year;
//    uint32_t Month;
//    uint32_t Day;

//    int Xacc;
//    int Yacc;
//    int Zacc;

//    char GPS_str[128];
//    char key[12];

//}RMCINFO;


//Ford 20191129 add it.
typedef struct hiSAMOON_INFO_PACKET
{

    FileInfo_t sm_file_info;
    GPSInfo_t sm_gps_info;

}HI_SAMOON_INFO_PACKET;

//########################## Hisilicon ################################

#define UINT32 unsigned int

typedef enum {
    SHAKE_L,
    SHAKE_R,
}SHAKE;

typedef enum {
    ORIENT_0,
    ORIENT_90,
    ORIENT_180,
    ORIENT_270,
}ORIENT;

typedef struct {
    UINT32 Xacc;
    UINT32 Yacc;
    UINT32 Zacc;
}AXIS_DATA;

typedef struct {
    AXIS_DATA Axis;
    ORIENT    Ori;
    SHAKE	  SH;
}Gsensor_Data, *pGsensor_Data;

//RMC, Recommended Minimum sentence C
#if 1
typedef struct{

    UINT32    Hour;
    UINT32    Minute;
    UINT32    Second;
    char      Status;              //Status A=active or V=Void
    double     Latitude;
    char      NSInd;
    double     Longitude;
    char      EWInd;
    double     Speed;               //Speed over the ground in knots
    double    Angle;               //Track angle in degrees True
    UINT32    Year;
    UINT32    Month;
    UINT32    Day;

    int    Xacc;
    int    Yacc;
    int    Zacc;
    char   Gps_str[128];
    char   key[12];
}RMCINFO;
#else
typedef struct {

    UINT32    Hour;
    UINT32    Minute;
    UINT32    Second;
    UINT32    Year;
    UINT32    Month;
    UINT32    Day;
    CHAR      Status;              //Status A=active or V=Void
    CHAR      NSInd;
    CHAR      EWInd;
    CHAR      reservd;
    float     Latitude;
    float     Longitude;
    float     Speed;               //Speed over the ground in knots
    float     Angle;               //Track angle in degrees True

    int    Xacc;
    int    Yacc;
    int    Zacc;
    CHAR   Gps_str[128];
    CHAR   key[12];

}RMCINFO;
#endif

//5152 -- 16384
typedef struct NTGPSInfo_s{
    //  char IQVer[16];
    char IQBuildDate[16];
    RMCINFO rmcinfo;
    //  char IQDebugInfo[5120];
}NTGPSInfo_t;


//For Sunplus
typedef struct gps_mov_data {
    short gsensor_x;
    short gsensor_y;
    short gsensor_z;

    char  gps_info_rmc[126];
}sunplus_gps_mov_data_t;

typedef struct tagAssistInfo {
    float	gsensor_x;
    float	gsensor_y;
    float	gsensor_z;
    string gps_lat;
    string gps_lgt;
    int     spd;
    double  north_angle;
}AssistInfo_t, *PAssistInfo;

typedef union {
    unsigned int a;
    char b[4];
}Rdata;


typedef vector<AssistInfo_t>	AssistInfoList;

class AssistFile {
public:
    AssistFile();
    ~AssistFile(void);

    #define ENDIAN_CONV(b)	(b[0] << 24 | b[1] << 16 | b[2] << 8 | b[3])
    //@liangfu 20161005
    int VideoFPS;
    int ParseAssistFile(string strAstFile);
    int ParseAssistDataForSunplus(string strMOVFile);		//2013-03-04
    int ParseMOVSubtitle(string strMOVFile);				//2013-06-17  for A7s mov
    int ParseAssistDataForNovatek(string strMOVFile);		//2013-08-19  for NTK mov

    int ParseAssistDataForNovatek65x(string strMOVFile);		//2013-08-19  for NTK mov
    int ParseAssistDataForNovatek66x(string strMOVFile);		//2013-08-19  for NTK mov

    int ParseAssistDataForNovatek9667XA(string strMOVFile,int itemNum);    //2020-10-08

    int GetAssistInfoCount() { return (int)m_infoList.size();}
    int GetNode(int idx, AssistInfo_t &node);

    void ClearList();
    AssistInfoList* GetAssistInfoList(void) {return &m_infoList;}



private:
    string m_strAstFile;
    AssistInfoList	m_infoList;
    int ParseGGA(char *gpsinfo, string &strLat, string &strLgt, int &spd);
    int ParseRMC(char *gpsinfo, string &strLat, string &strLgt, int &spd, double &north_angle);
    int ParseLine(char *pszLine, int key);


};


#endif //YEECORECAMVIEWER_ANDROID_ASSISTFILE_H
