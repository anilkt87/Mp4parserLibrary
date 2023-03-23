//
// Created by administrator on 2021/7/15.
//

#include "AssistFile.h"
#include "log.h"

#define MAX_PATH                 1024

AssistFile::AssistFile()
{

}

AssistFile::~AssistFile(void)
{
    m_infoList.clear();
}

void decryt(char *szline, int key)
{
    int len = strlen(szline);
    int pos = 0;
    char *p = szline;

    while(len) {
        if(p[pos] == 0x0A || p[pos] == 0x0D) break;
        p[pos] = p[pos] - key;
        pos ++;
        len--;
    }
    //USES_CONVERSION;
    //LOGI(_T("        Decrypted: %s"), A2T(szline));
}

void parse_coordinate(char *coord, char dir, string &strCoord)
{
    int flag = 1;
    if(dir == 'W' || dir == 'S') flag = -1;
    int len = strlen(coord);
    char * dotPtr = strchr(coord, '.');

    if(dotPtr) {
        int dddmm = 0;
        int ddd   = 0;
        float ssss = 0.0f;
        float mmssss = 0.0f;
        int dotPos = dotPtr - coord;
        int degree_of_data = len - dotPos;

        coord[dotPos] = '\0';

        dddmm = atof(coord);
        if(dotPos + 1 < len - 1) {
            ssss  = atof(&coord[dotPos+1]);
        } else {
            ssss  = 0.0f;
        }
        //2231.0760 = 22 + 0.310760 * 100 / 60 = 22.5179333333
        ddd = dddmm / 100;
/*
        if(degree_of_data == 5){
            mmssss = dddmm % 100 * 10000 + ssss;
            strCoord.Format(_T("%d.%06d"),  ddd * flag, (int)mmssss * 100 / 60);
        }else if(degree_of_data == 6){
            mmssss = dddmm % 100 * 100000 + ssss;
            strCoord.Format(_T("%d.%07d"),  ddd * flag, (int)mmssss * 100 / 60);
        }
*/
        char value[20];
        if(degree_of_data == 5){
            mmssss = dddmm % 100 * 10000 + ssss;

            if(flag < 0) {
                //  strCoord.Format(_T("-%d.%06d"),  ddd, (int)mmssss * 100 / 60);
                sprintf(value,"-%d.%06d",ddd,(int)mmssss * 100 / 60);
            } else {
//                strCoord.Format(_T("%d.%06d"),  ddd, (int)mmssss * 100 / 60);
                sprintf(value,"%d.%06d",ddd,(int)mmssss * 100 / 60);
            }
        }else if(degree_of_data == 6){
            mmssss = dddmm % 100 * 100000 + ssss;
            if(flag < 0) {
                // strCoord.Format(_T("-%d.%07d"),  ddd, (int)mmssss * 100 / 60);
                sprintf(value,"-%d.%07d",ddd,(int)mmssss * 100 / 60);
            } else {
                sprintf(value,"%d.%07d",ddd,(int)mmssss * 100 / 60);
            }
        }
        strCoord=value;
    } else {
        strCoord = "0";
    }
}

int AssistFile::ParseRMC(char *gpsinfo, string &strLat, string &strLgt, int &spd, double &north_angle)
{
    //$GPRMC,053014.000,A,2237.4177,N,11403.4075,E,1.90,235.83,050510,,,A*6B
    //$GNRMC,021442.504,V,,,,,,,081020,0.1,E,N*32
    char *p = NULL;
    char lat[32] = {0};
    char lgt[32] = {0};
    char speed[32] = {0};
    char nangle[32] = {0};
    char lat_dir = 'N';
    char lgt_dir = 'E';

    do {

        p = strtok(gpsinfo, ",");

        if(!p) break;

        if(strcmp(p, "$GPRMC") != 0  && strcmp(p, "$GLRMC") != 0 && strcmp(p, "$GNRMC") != 0) {
            break;
        }

        p = strtok(NULL, ",");		//<1> UTC Time
        if(!p) break;

        p = strtok(NULL, ",");		//<2> Valid flag  A: Valid  V: Invalid
        if(!p) break;

        p = strtok(NULL, ",");		//<3> lat
        if(!p || strlen(p) > 32) break;
        sprintf(lgt, "%s", p);

        p = strtok(NULL, ",");		//<4> lat dir
        if(!p) break;
        lgt_dir = p[0];

        p = strtok(NULL, ",");		//<5> lgt
        if(!p || strlen(p) > 32) break;
        sprintf(lat, "%s", p);

        p = strtok(NULL, ",");		//<6> lgt dir
        if(!p) break;
        lat_dir = p[0];

        p = strtok(NULL, ",");		//<7> speed
        if(!p || strlen(p) > 32) break;
        sprintf(speed, "%s", p);

        p = strtok(NULL, ",");		//<8> north angle
        if(!p || strlen(p) > 32) break;
        sprintf(nangle, "%s", p);

        p = strtok(NULL, ",");		//<9> UTC Date

        string str;

        parse_coordinate(lgt, lgt_dir, str);
        strLgt = str;

        parse_coordinate(lat, lat_dir, str);
        strLat = str;

        spd = (int)(atof(speed) * 1.85);
        north_angle = atof(nangle);
        if(north_angle > 360) north_angle = 0;
    }while(0);

    return 0;
}

int AssistFile::ParseGGA(char *gpsinfo, string &strLat, string &strLgt, int &spd)
{
    //$GPGGA,071458.000,0000.0000,N,00000.0000,E,0,06,2.7,39.8,M,-2.7,M,,0000*7F
    char *p = NULL;
    char lat[32] = {0};
    char lgt[32] = {0};
    char lat_dir = 'N';
    char lgt_dir = 'E';

    do {
        p = strtok(gpsinfo, ",");
        if(!p) break;
        if(strcmp(p, "$GPGGA") != 0) {
            break;
        }

        p = strtok(NULL, ",");		//Time?
        if(!p) break;

        p = strtok(NULL, ",");		//lgt
        if(!p || strlen(p) > 32) break;
        sprintf(lgt, "%s", p);

        p = strtok(NULL, ",");		//lgt dir
        if(!p) break;
        lgt_dir = p[0];

        p = strtok(NULL, ",");		//lat
        if(!p || strlen(p) > 32) break;
        sprintf(lat, "%s", p);

        p = strtok(NULL, ",");		//lat dir
        if(!p) break;
        lat_dir = p[0];

        string str;
        parse_coordinate(lgt, lgt_dir, str);
        strLgt = str;
        parse_coordinate(lat, lat_dir, str);
        strLat = str;

    }while(0);

    return 0;
}

extern bool get_A12_flags(void);


int AssistFile::ParseLine(char *pszLine, int key)
{
    int				x, y, z;
    char gpsinfo[1024] = {0};

    AssistInfo_t	node;
    node.gsensor_x = 0.0f;
    node.gsensor_y = 0.0f;
    node.gsensor_z = 0.0f;
    node.north_angle = 0.0f;
    node.spd = 0;

    x = y = z = 0;
    if(!pszLine) return -1;

//LOGI("Line:%s\n", pszLine);

    //if (false == get_A12_flags()) {

    if ((pszLine[1] == '\t') || (pszLine[2] == '\t') || (pszLine[3] == '\t') || (pszLine[4] == '\t') || (pszLine[5] == '\t'))
        ;
    else
        decryt(pszLine, key);

    //decryt(pszLine, key);
    //}
//	LOGI("    :%s\n", pszLine);

    sscanf(pszLine, "%d\t%d\t%d\t%s", &x, &y, &z, gpsinfo);


    node.gsensor_x = x / 1000.0f;
    node.gsensor_y = y / 1000.0f;
    node.gsensor_z = z / 1000.0f;

//	LOGI("===> gsensor_x:  %f\n", node.gsensor_x);
//	LOGI("===> gsensor_y:  %f\n", node.gsensor_y);
//	LOGI("===> gsensor_z:  %f\n", node.gsensor_z);


//LOGI("===> gpsinfo:  %s\n", gpsinfo);
    //if(gpsinfo[0] == '$' && gpsinfo[1] == 'G' && gpsinfo[2] == 'P' && gpsinfo[3] == 'G' && gpsinfo[4] == 'G' && gpsinfo[5] == 'A') {
    //	ParseGGA(gpsinfo, node.gps_lat, node.gps_lgt, node.spd);		//GPS info
    //} else {
    ParseRMC(gpsinfo, node.gps_lat, node.gps_lgt, node.spd, node.north_angle);
    //}

//	LOGI("2 gpsinfo:  %s\n", gpsinfo);
    //LOGI(_T("%s,%s\n"), node.gps_lat, node.gps_lgt);

    m_infoList.push_back(node);

#if 0//Ä£ÄâÎÄ¼þÒÔ1Hz²ÉÑù,Êµ¼ÊÊÇÓÃ10Hz²ÉÑù
    m_infoList.AddTail(node);
    m_infoList.AddTail(node);
    m_infoList.AddTail(node);
    m_infoList.AddTail(node);
    m_infoList.AddTail(node);
    m_infoList.AddTail(node);
    m_infoList.AddTail(node);
    m_infoList.AddTail(node);
    m_infoList.AddTail(node);
#endif


    do {

    }while(0);

    return m_infoList.size();;
}


int AssistFile::ParseAssistFile(string strAstFile)
{
    char	tFile[MAX_PATH]	 = {0};
    char	szFile[MAX_PATH] = {0};
    char	szLine[1024]		 = {0};
    FILE	*fd				 = NULL;



    //Convert TCHAR to CHAR

#if defined(Q_OS_WIN64)  || defined(Q_OS_WIN32)
    sprintf(tFile, "%s", strAstFile.toLocal8Bit().data());
#elif defined(Q_OS_MAC64)
    sprintf(tFile, "%s", strAstFile.toStdString().data());
#endif


    sprintf(szFile, "%s", tFile);

    //Clear list first
    m_infoList.clear();

    if(strAstFile.length() < 4) return -1;

    //Open file
    fd = fopen(szFile, "rb");
    if(!fd) {
        return -1;
    }

    int key = 0;
    if(fgets(szLine, 1024, fd)) {		//get first line (KEY)
        char ch;
        ch = szLine[0];
        if(atoi(&ch) == 0) goto end;
        key += atoi(&ch);
        ch = szLine[1];
        if(atoi(&ch) == 0) goto end;
        key += atoi(&ch);
        ch = szLine[2];
        if(atoi(&ch) == 0) goto end;
        key += atoi(&ch);
        ch = szLine[3];
        if(atoi(&ch) == 0) goto end;
        key += atoi(&ch);
        ch = szLine[4];
        if(atoi(&ch) == 0) goto end;
        key += atoi(&ch);
        ch = szLine[4];
        if(atoi(&ch) == 0) goto end;
        key += atoi(&ch);
        ch = szLine[5];
        if(atoi(&ch) == 0) goto end;
        key += atoi(&ch);
        ch = szLine[6];
        if(atoi(&ch) == 0) goto end;
        key += atoi(&ch);
        ch = szLine[7];
        if(atoi(&ch) == 0) goto end;
        key += atoi(&ch);
        ch = szLine[7];
        key += atoi(&ch);
        while(fgets(szLine, 1024, fd)) {
            ParseLine(szLine, key);
        }
    }

    end:
    fclose(fd);
    fd = NULL;

    return m_infoList.size();
}

#define POS(x)  (7+x)
int AssistFile::ParseAssistDataForSunplus(string strMOVFile)
{
    char	tFile[MAX_PATH]	 = {0};
    char	szFile[MAX_PATH] = {0};
    char	szLine[1024]		 = {0};
    FILE	*fd				 = NULL;
    char	dat_hdr_flag[17] = {'\0'};



#if defined(Q_OS_WIN64)  || defined(Q_OS_WIN32)
    sprintf(tFile, "%s", strMOVFile.toLocal8Bit().data());
#elif defined(Q_OS_MAC64)
    sprintf(tFile, "%s", strMOVFile.toStdString().data());
#endif


    sprintf(szFile, "%s", tFile);
    //Clear list first
    m_infoList.clear();

    if(strMOVFile.length() < 4) return -1;

    //Open file
    fd = fopen(szFile, "rb");
    if(fd == NULL) {
        return -1;
    }

    /****************sunplus mov file struct ***************************************
       [ftypSize][ftyp]
           [mdatSize][mdat]......     --->ËùÓÐÒôÊÓÆµÊý¾Ý
           [moovSize][moov]......     --->ÎÄ¼þÊý¾ÝÍ·¼°Ë÷Òý
           [udatSize][udat]......     --->ÓÃ»§Êý¾Ý
           [flagSize][icat6330]       --->sunplus¼ÓµÄÎÄ¼þ±êÊ¶
   ********************************************************************************/

    unsigned int ftypsize = 0, mdatsize = 0, moov_addr = 0, moovsize = 0, udat_addr = 0, udatsize = 0;
    unsigned char b[4] = {0};
    int found_flag = 0;

    fread((unsigned char*)b, 1, 4, fd);		//get mdat address(ftyp size)
    ftypsize = b[0] << 24 | b[1] << 16 | b[2] << 8 | b[3];
    if(ftypsize > 0) {
        fseek(fd, ftypsize, SEEK_SET);
        LOGI("mdat_addr: 0x%08X\n", ftypsize);

        fread((unsigned char*)b, 1, 4, fd); //get mdat size
        mdatsize = b[0] << 24 | b[1] << 16 | b[2] << 8 | b[3];
        if(mdatsize > 0) {
            moov_addr = mdatsize + ftypsize;
            fseek(fd, moov_addr, SEEK_SET);
            LOGI("moov_addr: 0x%08X\n", moov_addr);
            fread((unsigned char*)b, 1, 4, fd); //get moov size
            moovsize = b[0] << 24 | b[1] << 16 | b[2] << 8 | b[3];
            if(moovsize > 0) {
                udat_addr = moov_addr + moovsize;
                fseek(fd, udat_addr, SEEK_SET);
                LOGI("udat_addr: 0x%08X, moovsize:0x%08X\n", udat_addr, moovsize);

                fread((unsigned char*)b, 1, 4, fd); //get udat size
                udatsize = b[0] << 24 | b[1] << 16 | b[2] << 8 | b[3];
                if(udatsize > 0) {
                    LOGI("udatSize: 0x%08X\n", udatsize);
                    //fseek(fd, 4, SEEK_CUR);		//skip 'udat'
                    if(fread(dat_hdr_flag, 1, 4, fd) == 4) {
                        if(dat_hdr_flag[0] == 'f' && dat_hdr_flag[1] == 'r' && dat_hdr_flag[2] == 'e' && dat_hdr_flag[3] == 'e') {
                            udat_addr += udatsize;
                            fseek(fd, udat_addr + 8, SEEK_SET); //8: udat flag + udat size
                        }
                        if(fread(dat_hdr_flag, 1, 16, fd) == 16) {
                            dat_hdr_flag[15] = '\0';
                            LOGI("data:%s\n", dat_hdr_flag);
                            if(dat_hdr_flag[0] == 's' && dat_hdr_flag[1] == 'a' && dat_hdr_flag[2] == 'm' && dat_hdr_flag[3] == 'o'
                               && dat_hdr_flag[4] == 'o' && dat_hdr_flag[5] == 'n' && dat_hdr_flag[6] == '1')
                            {		//"samoon1"
                                //if(strcmp(dat_hdr_flag, "samoon01") == 0) {
                                found_flag = 1;
                            }
                        }
                    }
                }
            }
        }
    }

    if(found_flag)
    {
        sunplus_gps_mov_data_t gps_dat;
        unsigned char key = 0;
        char ch;

        //value = Matrix[0]+Matrix[1]+Matrix[2]+Matrix[3]+Matrix[4]+ Matrix[4]+Matrix[5]+Matrix[6]+Matrix[7]+Matrix[7];
        key = dat_hdr_flag[POS(0)] + dat_hdr_flag[POS(1)] + dat_hdr_flag[POS(2)] + dat_hdr_flag[POS(3)] + dat_hdr_flag[POS(4)] + dat_hdr_flag[POS(4)] \
            + dat_hdr_flag[POS(5)] + dat_hdr_flag[POS(6)] + dat_hdr_flag[POS(7)] + dat_hdr_flag[POS(7)];
        //LOGI("hdr:%s, key:%d \n", dat_hdr_flag, key);

        while(!feof(fd)) {
            if(fread((char*)&gps_dat, 1, sizeof(sunplus_gps_mov_data_t), fd) != sizeof(sunplus_gps_mov_data_t)) {
                break;
            }

            { //decrypt gps data
                gps_dat.gsensor_x -= key;
                gps_dat.gsensor_y -= key;
                gps_dat.gsensor_z -= key;

                int i;
                for(i = 0; i < 126; i++) {
                    gps_dat.gps_info_rmc[i] -= key;
                }
                dat_hdr_flag[16] = '\0';
                LOGI("Flag:%s KEY:%d x:%d y:%d z:%d rmc:%s \n", dat_hdr_flag, key, gps_dat.gsensor_x, gps_dat.gsensor_y, gps_dat.gsensor_z, gps_dat.gps_info_rmc);
            }

            AssistInfo_t	node;
            node.gsensor_x = gps_dat.gsensor_x / 1000.0f;
            node.gsensor_y = gps_dat.gsensor_y / 1000.0f;
            node.gsensor_z = gps_dat.gsensor_z / 1000.0f;
            node.north_angle = 0.0f;
            node.spd = 0;

            ParseRMC(gps_dat.gps_info_rmc, node.gps_lat, node.gps_lgt, node.spd, node.north_angle);

            m_infoList.push_back(node);

#if 0	//Ä£ÄâÎÄ¼þÒÔ1Hz²ÉÑù,Êµ¼ÊÊÇÓÃ10Hz²ÉÑù
            m_infoList.AddTail(node);
            m_infoList.AddTail(node);
            m_infoList.AddTail(node);
            m_infoList.AddTail(node);
            m_infoList.AddTail(node);
            m_infoList.AddTail(node);
            m_infoList.AddTail(node);
            m_infoList.AddTail(node);
            m_infoList.AddTail(node);
#endif

        }
    }

    end:
    fclose(fd);
    fd = NULL;

    return m_infoList.size();
}

#if 1

int AssistFile::ParseAssistDataForNovatek(string strMOVFile)
{
    char	tFile[MAX_PATH] = { 0 };
    char	szFile[MAX_PATH] = { 0 };
    FILE	*fd = NULL;
    //char	szBuf[1024] = { 0 };
    char	file[MAX_PATH] = { 0 };
    int		flag = 0;

    unsigned int _duration = 0, _timescale = 0;



    //Convert TCHAR to CHAR




#if defined(Q_OS_WIN64)  || defined(Q_OS_WIN32)
    sprintf(tFile, "%s", strMOVFile.toLocal8Bit().data());
#elif defined(Q_OS_MAC64)
    sprintf(tFile, "%s", strMOVFile.toStdString().data());
#endif


    sprintf(szFile, "%s", tFile);

    //Clear list first
    m_infoList.clear();

    if (strMOVFile.length() < 4) return -1;

    //Open file
    fd = fopen(szFile, "rb");
    if (fd == NULL) {
        return -1;
    }

    //moov
    //idx1
    //1. Get MOOV offset
    //2. Get audio tracker offset
    //3.    Get 01wb offset from idx1
    //4.    Get GS_data in moov by 01wb offset + 1 sdcard cluster size(0x10000)

    do {

        unsigned int ftypsize = 0, mdatsize = 0, mdat_addr = 0, moov_addr = 0, mvhd_addr = 0;
        unsigned int strm1 = 0, strm2 = 0, strm3 = 0;
        unsigned int tkhd = 0, edts = 0, mdia = 0;
        unsigned int mdhd = 0, dinf = 0, stbl = 0;
        unsigned int stsd = 0, stts = 0, stsc = 0, stsz = 0, stco = 0;
        unsigned int size = 0, pos = 0, curpos = 0;
        unsigned char chSize[8] = { 0 };

        char chFlag[8] = { 0 };
        char rbuf[128] = { 0 };
        char date_s[16] = { 0 };
        char time_s[16] = { 0 };
        char gps_lat[16] = { 0 };
        char gps_lgt[16] = { 0 };
        char gps_spd[16] = { 0 };
        int i, check_flag = 0;

        //Step1. Find MOOV
        check_flag = 0;
        while (!feof(fd)) {

            if (fread(chSize, 1, 4, fd) != 4) break;		//get address
            size = ENDIAN_CONV(chSize);					//convert size
            if (fread(chFlag, 1, 4, fd) != 4) break;

            pos += size;
            LOGI("1 [0x%08X]%s - Pos:0x%08X\n", *((unsigned int*)chSize), chFlag, pos);

            if (strcmp(chFlag, "mdat") == 0) {

                mdat_addr = ftell(fd);
            }

            if (strcmp(chFlag, "moov") == 0) {

                check_flag = 1;
                curpos = ftell(fd);
                break;
            }

            fseek(fd, size - 8, SEEK_CUR);
        }

        if (check_flag == 0) break;

        //Step2. Get Tracker
        int track_id = 0;
        check_flag = 0;
        while (!feof(fd)) {

            if (fread(chSize, 1, 4, fd) != 4) break;		//get address
            size = ENDIAN_CONV(chSize);					//convert size
            if (fread(chFlag, 1, 4, fd) != 4) break;

            pos += size;
            LOGI("2         [0x%08X]%s - Pos:0x%08X\n", *((unsigned int*)chSize), chFlag, pos);

            if (strcmp(chFlag, "trak") == 0) {	//only 1 track

                curpos = ftell(fd);
                track_id++;

                //if(track_id == 2) {		//track1: video, track2: audio
                check_flag = 1;
                //}
            }

#if 0
            if (track_id > 0 && strcmp(chFlag, "trak") != 0) {	//only 1 track
                check_flag = 1;
                fseek(fd, curpos, SEEK_SET);
            }

            if (strcmp(chFlag, "trak") == 0) {
                curpos = ftell(fd);
                track_id++;
                if (track_id == 2) {		//track1: video, track2: audio
                    check_flag = 1;
                }
            }
#endif

            if (check_flag == 1) {
                //Step3. Check Tracker
                do {

                    //Step4. Get mdia from trak
                    int check_flag2 = 0;
                    while (ftell(fd) <= pos) {

                        if (fread(chSize, 1, 4, fd) != 4) break;		//get address
                        size = ENDIAN_CONV(chSize);					//convert size
                        if (fread(chFlag, 1, 4, fd) != 4) break;

                        LOGI("3                [0x%08X]%s - Pos:0x%08X\n", *((unsigned int*)chSize), chFlag, pos);
                        if (strcmp(chFlag, "mdia") == 0) {
                            check_flag2 = 1;
                            break;
                        }

                        fseek(fd, size - 8, SEEK_CUR);
                    }
                    if (check_flag2 == 0) break;

                    //Step5. Get minf from mdia
                    check_flag2 = 0;
                    while (ftell(fd) <= pos) {

                        if (fread(chSize, 1, 4, fd) != 4) break;		//get address
                        size = ENDIAN_CONV(chSize);					//convert size
                        if (fread(chFlag, 1, 4, fd) != 4) break;

                        LOGI("4                        [0x%08X]%s - Pos:0x%08X\n", *((unsigned int*)chSize), chFlag, pos);


#if 1
                        if (strcmp(chFlag, "mdhd") == 0) {

                            char bufFPS[8] = { 0 };
                            unsigned int AllFPSCunts = 0;
                            int64_t stbladdr = ftell(fd);
                            Rdata rdata_rd;

                            fseek(fd, 12, SEEK_CUR);

                            memset(bufFPS, 0, 8);
                            if (fread(bufFPS, 1, 4, fd) != 4) break;

                            rdata_rd.b[3] = bufFPS[0];
                            rdata_rd.b[2] = bufFPS[1];
                            rdata_rd.b[1] = bufFPS[2];
                            rdata_rd.b[0] = bufFPS[3];

                            _timescale = rdata_rd.a;

                            memset(bufFPS, 0, 8);
                            if (fread(bufFPS, 1, 4, fd) != 4) break;

                            rdata_rd.b[3] = bufFPS[0];
                            rdata_rd.b[2] = bufFPS[1];
                            rdata_rd.b[1] = bufFPS[2];
                            rdata_rd.b[0] = bufFPS[3];

                            _duration = rdata_rd.a;

                            //    LOGI("######################## timescale: %d  duration:  %d \n", _timescale, _duration);
                            fseek(fd, stbladdr, SEEK_SET);
                        }
#endif

                        if(strcmp(chFlag, "minf") == 0)
                        {
                            check_flag2 = 1;
                            break;
                        }

                        fseek(fd, size - 8, SEEK_CUR);
                    }
                    if (check_flag2 == 0) break;

                    //Step6. Get stbl from minf
                    check_flag2 = 0;
                    while (ftell(fd) <= pos) {

                        if (fread(chSize, 1, 4, fd) != 4) break;		//get address
                        size = ENDIAN_CONV(chSize);					//convert size
                        if (fread(chFlag, 1, 4, fd) != 4) break;

                        LOGI("5                                [0x%08X]%s - Pos:0x%08X\n", *((unsigned int*)chSize), chFlag, pos);
                        if (strcmp(chFlag, "stbl") == 0) {
                            check_flag2 = 1;
                            break;
                        }

                        fseek(fd, size - 8, SEEK_CUR);
                    }

                    if (check_flag2 == 0) break;

                    //@liangfu
                    int64_t stbladdr = ftell(fd);


                    //Step7. Get stco from stbl
                    check_flag2 = 0;
                    while (ftell(fd) <= pos) {

                        if (fread(chSize, 1, 4, fd) != 4) break;		//get address
                        if (fread(chFlag, 1, 4, fd) != 4) break;

                        size = ENDIAN_CONV(chSize);					//convert size
                        LOGI("                                     [0x%08X]% 4d-%s - Pos:0x%08X\n", *((unsigned int*)chSize), size, chFlag, pos);

                        if (strcmp(chFlag, "stsz") == 0) {

                            char bufFPS[8] = { 0 };
                            unsigned int AllFPSCunts = 0;

                            int64_t stbladdr = ftell(fd);
                            Rdata rdata_rd;

                            fseek(fd, 8, SEEK_CUR);
                            if (fread(bufFPS, 1, 4, fd) != 4) break;

                            rdata_rd.b[3] = bufFPS[0];
                            rdata_rd.b[2] = bufFPS[1];
                            rdata_rd.b[1] = bufFPS[2];
                            rdata_rd.b[0] = bufFPS[3];


                            AllFPSCunts = rdata_rd.a;

                            //VideoFPS = AllFPSCunts / (_duration / _timescale);
                            VideoFPS = (AllFPSCunts*_timescale)/_duration;
                            fseek(fd, stbladdr, SEEK_SET);
                        }

                        if (strcmp(chFlag, "stco") == 0) {
                            check_flag2 = 1;
                            break;
                        }
#if 0
                        if ((track_id == 1 && strcmp(chFlag, "stsz") == 0)
                            || ((track_id > 1 && strcmp(chFlag, "stco") == 0))) {
                            check_flag2 = 1;
                            break;
                        }
#endif
                        fseek(fd, size - 8, SEEK_CUR);

                    }

                    if (check_flag2 == 0) break;

                    unsigned int stsz_addr = ftell(fd);
                    unsigned int stsz_size = size;
                    unsigned int addr = 0;
                    int gps_rec_cnt = 0;
                    unsigned int *alladdr;
                    unsigned int *allaframesize;

                    //@liangfu 20160804.
                    {
                        size = (size - 8) / 4;					  //4bytes for every frame size length
                        if (size > 0) {
                            //gps_rec_cnt = size / 30 + 1;
                            gps_rec_cnt = size / VideoFPS + 1;

                            alladdr = (unsigned int *)malloc(gps_rec_cnt * sizeof(unsigned int));
                            allaframesize = (unsigned int *)malloc(gps_rec_cnt * sizeof(unsigned int));
                        }
                        else {

                            break;
                        }

                        LOGI("6	Founded %d stsz indexs! mdat_addr:0x%08X, curr:0x%08X\n", size, mdat_addr, ftell(fd));

                        //Step8. Read video frame size from STSZ
                        //fseek(fd, 8, SEEK_CUR);	//Skip the first 8 bytes
                        fseek(fd, 4, SEEK_CUR);	//Skip the first 8 bytes
                        for (i = 0; i < size; i++) {

                            unsigned int addr = 0, rval = 0, idx;

                            memset(chSize, 0, 8);
                            fread(chSize, 1, 4, fd);
                            addr = ENDIAN_CONV(chSize);

                            //LOGI("STCO    idx%d: 0x%08X \n", i, addr);

                            //idx = (i + 1) / 30;
                            //if (i > 0 && (i % 30) == 0) {			//Read video frame size by every 30fps(at the 31frame)

                            idx = (i + 1) / VideoFPS;
                            if (i > 0 && (i % VideoFPS) == 0) {			//Read video frame size by every 30fps(at the 31frame)


                                alladdr[idx - 1] = addr;			//video frame size

                                int64_t stbladdr = ftell(fd);
                                fseek(fd, alladdr[idx - 1], SEEK_SET);

                                memset(chSize, 0, 8);
                                fread(chSize, 1, 4, fd);
                                allaframesize[idx - 1] = ENDIAN_CONV(chSize) + 4;

                                fseek(fd, stbladdr, SEEK_SET);

                                //    LOGI("\tSTCO    idx%d: 0x%08X  size:  0x%08X \n", idx, addr, allaframesize[idx - 1]);
                            }
                        }

                    }


#if 0
                    if (track_id == 1) {	//only video tracker

                        size = (size - 8) / 4;					  //4bytes for every frame size length
                        if (size > 0) {
                            gps_rec_cnt = size / 30 + 1;
                            alladdr = (unsigned int *)malloc(gps_rec_cnt * sizeof(unsigned int));
                        }
                        else {
                            break;
                        }
                        LOGI("6	Founded %d stsz indexs! mdat_addr:0x%08X, curr:0x%08X\n", size, mdat_addr, ftell(fd));

                        //Step8. Read video frame size from STSZ
                        fseek(fd, 8, SEEK_CUR);	//Skip the first 8 bytes
                        for (i = 0; i < size; i++) {

                            unsigned int addr = 0, rval = 0, idx;
                            memset(chSize, 0, 8);
                            fread(chSize, 1, 4, fd);
                            addr = ENDIAN_CONV(chSize);
                            //LOGI("STSZ    idx%d: 0x%08X\n", i, addr);

                            idx = (i + 1) / 30;
                            if (i > 0 && (i % 30) == 0) {			//Read video frame size by every 30fps(at the 31frame)
                                alladdr[idx - 1] = addr;			//video frame size
                                LOGI("\tSTSZ    idx%d: 0x%08X\n", idx, addr);
                            }
                        }

                        //Read video frame offset from STCO
                        fseek(fd, stsz_addr + stsz_size - 8, SEEK_SET);

                        if (fread(chSize, 1, 4, fd) != 4) break;   //[4] size
                        if (fread(chFlag, 1, 4, fd) != 4) break;   //[4] flag

                        size = ENDIAN_CONV(chSize);
                        size = (size - 8) / 4;
                        if (size > 0) {

                            LOGI("	Founded %d %s indexs(0x%08X)! mdat_addr:0x%08X, curr:0x%08X\n", size, chFlag, stsz_addr, mdat_addr, ftell(fd));
                            //Read video frame size from STCO
                            fseek(fd, 8, SEEK_CUR);	//Skip the first 8 bytes
                            for (i = 0; i < size; i++) {

                                unsigned int addr = 0, rval = 0, idx;
                                memset(chSize, 0, 8);
                                fread(chSize, 1, 4, fd);
                                addr = ENDIAN_CONV(chSize);
                                //LOGI("STCO    idx%d: 0x%08X\n", i, addr);

                                idx = (i + 1) / 30;
                                if (i > 0 && ((i + 1) % 30) == 0 && idx <= gps_rec_cnt) {	//Read video frame offset by every 30fps(at the 30frame!!!! Fuck!!!!)

                                    addr = alladdr[idx - 1] + addr; //Video frame size + Video frame offset
                                    addr = ((addr + 0x7fff) & 0xFFFF8000);		//Aligned to 0x8000(For wirte cluster size)
                                    alladdr[idx - 1] = addr;
                                    LOGI("\tSTCO    idx%d: 0x%08X\n", idx, addr);
                                }
                            }
                        }
                    }
                    else {

                        //Step8. Get index from stco
                        if (fread(chSize, 1, 4, fd) != 4) break;   //[4]
                        if (fread(chSize, 1, 4, fd) != 4) break;   //[4] index size

                        size = ENDIAN_CONV(chSize);
                        if (size > 0) {
                            alladdr = (unsigned int *)malloc((size + 1) * sizeof(unsigned int));
                        }

                        LOGI("	Founded %d indexs! mdat_addr:0x%08X, curr:0x%08X\n", size, mdat_addr, ftell(fd));

                        for (i = 0; i < size; i++) {

                            unsigned int addr = 0, rval = 0;

                            memset(chSize, 0, 8);
                            fread(chSize, 1, 4, fd);
                            addr = ENDIAN_CONV(chSize);

                            //alladdr[i] = addr; //Skip 0x00 0x35
                            //alladdr[i] = addr + 0x20000; //0x10000: cluster size: 128K   Á¢ÌåÉù: 0x20000
                            alladdr[i] = addr + 0x10000; //cluster size: 128K   µ¥ÉùµÀ: 0x10000

                            LOGI("7    idx%d: 0x%08X  0x%08X\n", i, addr, alladdr[i]);
                        }

                        gps_rec_cnt = size;
                    }
#endif



#if 1
                    int found_key = 0;
                    int key = 0;
                    char cache[20][128] = { 0 };

                    long tellFileSize = 0;

                    for (i = 0; i < gps_rec_cnt; i++) {

                        int len;
                        int64_t gpsseek = 0;

                        RMCINFO gsdata;
                        memset(&gsdata, 0, sizeof(RMCINFO));

                        //fseek(fd, alladdr[i] + 48/*72 Fuck!!!*/, SEEK_SET);
                        //fseek(fd, alladdr[i] + 48/*72 Fuck!!!*/, SEEK_SET);
                        //fseek(fd, alladdr[i]+48, SEEK_SET);

                        gpsseek = alladdr[i] + allaframesize[i] + 48;
                        fseek(fd, gpsseek, SEEK_SET);

                        //tellFileSize = ftell(fd);

                        //fseek(fd, alladdr[i] + 72, SEEK_SET);


                        fread((char*)&gsdata, 1, sizeof(RMCINFO), fd);

                        char num[2] = { 0 };
                        key = 0;
                        num[0] = gsdata.key[(0)]; key += atoi(num);
                        num[0] = gsdata.key[(1)]; key += atoi(num);
                        num[0] = gsdata.key[(2)]; key += atoi(num);
                        num[0] = gsdata.key[(3)]; key += atoi(num);
                        num[0] = gsdata.key[(4)]; key += atoi(num);
                        num[0] = gsdata.key[(4)]; key += atoi(num);
                        num[0] = gsdata.key[(5)]; key += atoi(num);
                        num[0] = gsdata.key[(6)]; key += atoi(num);
                        num[0] = gsdata.key[(7)]; key += atoi(num);
                        num[0] = gsdata.key[(7)]; key += atoi(num);
                        //LOGI("  key:%d\n", key);

                        {	//Decrypt

#if 0
                            int pos = 0, len = 128; //10 = sizeof(key)
                            char *p = gsdata.Gps_str;

                            while (len) {
                                if (len > 1 && p[pos] == 0x0D && p[pos + 1] == 0x0A) {
                                    break;
                                }
                                p[pos] = p[pos] - key;
                                pos++;
                                len--;
                            }
#endif

                            LOGI("[%d] dectyped: x:%d, y:%d, z:%d, gps:%s\n", i, gsdata.Xacc, gsdata.Yacc, gsdata.Zacc, gsdata.Gps_str);

                            AssistInfo_t	node;
                            node.north_angle = 0.0f;
                            node.spd = 0;
                            if (((gsdata.Xacc>10000) || (gsdata.Xacc<-10000)) || ((gsdata.Yacc>10000) || (gsdata.Yacc<-10000)) || ((gsdata.Zacc>10000) || (gsdata.Zacc<-10000)))//MI@20141121  Update Time Lapse Video Issue
                            {
                                gsdata.Xacc = 0;
                                gsdata.Yacc = 0;
                                gsdata.Zacc = 0;
                            }

                            if (i >= gps_rec_cnt - 2) {		//Adam for NBDVR
                                gsdata.Xacc = 823;
                                gsdata.Yacc = 150;
                                gsdata.Zacc = 96;
                            }
                            node.gsensor_x = (gsdata.Xacc - 800) / 1000.0f;// * 64;	//Adam, For NBDVR
                            node.gsensor_y = gsdata.Yacc / 1000.0f;// * 64;
                            node.gsensor_z = gsdata.Zacc / 1000.0f;// * 64;

                            if (gsdata.Gps_str[1] == '$' && gsdata.Gps_str[2] == 'G' && gsdata.Gps_str[3] == 'P' && gsdata.Gps_str[4] == 'G' && gsdata.Gps_str[5] == 'G' && gsdata.Gps_str[6] == 'A') {
                                ParseGGA(&gsdata.Gps_str[1], node.gps_lat, node.gps_lgt, node.spd);		//GPS info
                            }
                            else {
                                ParseRMC(&gsdata.Gps_str[1], node.gps_lat, node.gps_lgt, node.spd, node.north_angle);
                            }

                            //LOGI("[%d] dectyped: x:%d, y:%d, z:%d, gps:%s\n", i, gsdata.Xacc, gsdata.Yacc, gsdata.Zacc, gsdata.Gps_str);

                            m_infoList.push_back(node);
                        }

                    }
#endif
                    if (alladdr != NULL) {
                        free(alladdr);
                        alladdr = NULL;
                    }
                    break;
                } while (0);

                fseek(fd, curpos, SEEK_SET);
                break;
            }

            fseek(fd, size - 8, SEEK_CUR);
        }
    } while (0);

    end:
    fclose(fd);
    fd = NULL;

    return m_infoList.size();
}


int AssistFile::ParseAssistDataForNovatek65x(string strMOVFile)
{
    char	tFile[MAX_PATH] = { 0 };
    char	szFile[MAX_PATH] = { 0 };
    FILE	*fd = NULL;
//	char	szBuf[1024] = { 0 };
    char	file[MAX_PATH] = { 0 };
    int		flag = 0;

    unsigned int _duration = 0, _timescale = 0;



    //Convert TCHAR to CHAR



#if defined(Q_OS_WIN64)  || defined(Q_OS_WIN32)
    sprintf(tFile, "%s", strMOVFile.toLocal8Bit().data());
#elif defined(Q_OS_MAC64)
    sprintf(tFile, "%s", strMOVFile.toStdString().data());
#endif

    sprintf(szFile, "%s", tFile);
    //Clear list first
    m_infoList.clear();

    if (strMOVFile.size() < 4) return -1;

    //Open file
    fd = fopen(szFile, "rb");
    if (fd == NULL) {
        return -1;
    }

    //moov
    //idx1
    //1. Get MOOV offset
    //2. Get audio tracker offset
    //3.    Get 01wb offset from idx1
    //4.    Get GS_data in moov by 01wb offset + 1 sdcard cluster size(0x10000)

    do {
        unsigned int ftypsize = 0, mdatsize = 0, mdat_addr = 0, moov_addr = 0, mvhd_addr = 0;
        unsigned int strm1 = 0, strm2 = 0, strm3 = 0;
        unsigned int tkhd = 0, edts = 0, mdia = 0;
        unsigned int mdhd = 0, dinf = 0, stbl = 0;
        unsigned int stsd = 0, stts = 0, stsc = 0, stsz = 0, stco = 0;
        unsigned int size = 0, pos = 0, curpos = 0;
        unsigned char chSize[8] = { 0 };
        char chFlag[8] = { 0 };
        char rbuf[128] = { 0 };
        char date_s[16] = { 0 };
        char time_s[16] = { 0 };
        char gps_lat[16] = { 0 };
        char gps_lgt[16] = { 0 };
        char gps_spd[16] = { 0 };
        int i, check_flag = 0;

        //Step1. Find MOOV
        check_flag = 0;
        while (!feof(fd)) {

            if (fread(chSize, 1, 4, fd) != 4) break;		//get address
            size = ENDIAN_CONV(chSize);		//convert size
            if (fread(chFlag, 1, 4, fd) != 4) break;

            pos += size;
            LOGI(" [0x%08X]%s - Pos:0x%08X\n", *((unsigned int*)chSize), chFlag, pos);

            if (strcmp(chFlag, "mdat") == 0) {
                mdat_addr = ftell(fd);
            }
            if (strcmp(chFlag, "moov") == 0) {
                check_flag = 1;
                curpos = ftell(fd);
                break;
            }

            fseek(fd, size - 8, SEEK_CUR);
        }
        if (check_flag == 0) break;

        //Step2. Get Tracker
        int track_id = 0;
        check_flag = 0;
        while (!feof(fd)) {
            if (fread(chSize, 1, 4, fd) != 4) break;		//get address
            size = ENDIAN_CONV(chSize);		//convert size
            if (fread(chFlag, 1, 4, fd) != 4) break;

            pos += size;
            LOGI("         [0x%08X]%s - Pos:0x%08X\n", *((unsigned int*)chSize), chFlag, pos);

            if (track_id > 0 && strcmp(chFlag, "trak") != 0) {	//only 1 track
                check_flag = 1;
                fseek(fd, curpos, SEEK_SET);
            }
            if (strcmp(chFlag, "trak") == 0) {
                curpos = ftell(fd);
                track_id++;
                if (track_id == 2) {		//track1: video, track2: audio
                    check_flag = 1;
                }
            }

            if (check_flag == 1) {
                //Step3. Check Tracker
                do {
                    //Step4. Get mdia from trak
                    int check_flag2 = 0;
                    while (ftell(fd) <= pos) {
                        if (fread(chSize, 1, 4, fd) != 4) break;		//get address
                        size = ENDIAN_CONV(chSize);		//convert size
                        if (fread(chFlag, 1, 4, fd) != 4) break;

                        LOGI("                [0x%08X]%s - Pos:0x%08X\n", *((unsigned int*)chSize), chFlag, pos);
                        if (strcmp(chFlag, "mdia") == 0) {
                            check_flag2 = 1;
                            break;
                        }
                        fseek(fd, size - 8, SEEK_CUR);
                    }
                    if (check_flag2 == 0) break;

                    //Step5. Get minf from mdia
                    check_flag2 = 0;
                    while (ftell(fd) <= pos) {
                        if (fread(chSize, 1, 4, fd) != 4) break;		//get address
                        size = ENDIAN_CONV(chSize);		//convert size
                        if (fread(chFlag, 1, 4, fd) != 4) break;

                        LOGI("                        [0x%08X]%s - Pos:0x%08X\n", *((unsigned int*)chSize), chFlag, pos);

#if 1
                        if (strcmp(chFlag, "mdhd") == 0) {

                            char bufFPS[8] = { 0 };
                            unsigned int AllFPSCunts = 0;
                            int64_t stbladdr = ftell(fd);
                            Rdata rdata_rd;

                            fseek(fd, 12, SEEK_CUR);

                            memset(bufFPS, 0, 8);
                            if (fread(bufFPS, 1, 4, fd) != 4) break;

                            rdata_rd.b[3] = bufFPS[0];
                            rdata_rd.b[2] = bufFPS[1];
                            rdata_rd.b[1] = bufFPS[2];
                            rdata_rd.b[0] = bufFPS[3];

                            _timescale = rdata_rd.a;

                            memset(bufFPS, 0, 8);
                            if (fread(bufFPS, 1, 4, fd) != 4) break;

                            rdata_rd.b[3] = bufFPS[0];
                            rdata_rd.b[2] = bufFPS[1];
                            rdata_rd.b[1] = bufFPS[2];
                            rdata_rd.b[0] = bufFPS[3];

                            _duration = rdata_rd.a;

                            //   LOGI("######################## timescale: %d  duration:  %d \n", _timescale, _duration);
                            fseek(fd, stbladdr, SEEK_SET);
                        }
#endif

                        if (strcmp(chFlag, "minf") == 0) {
                            check_flag2 = 1;
                            break;
                        }
                        fseek(fd, size - 8, SEEK_CUR);
                    }
                    if (check_flag2 == 0) break;

                    //Step6. Get stbl from minf
                    check_flag2 = 0;
                    while (ftell(fd) <= pos) {
                        if (fread(chSize, 1, 4, fd) != 4) break;		//get address
                        size = ENDIAN_CONV(chSize);		//convert size
                        if (fread(chFlag, 1, 4, fd) != 4) break;

                        LOGI("                                [0x%08X]%s - Pos:0x%08X\n", *((unsigned int*)chSize), chFlag, pos);
                        if (strcmp(chFlag, "stbl") == 0) {
                            check_flag2 = 1;
                            break;
                        }
                        fseek(fd, size - 8, SEEK_CUR);
                    }
                    if (check_flag2 == 0) break;

                    //Step7. Get stsz from stbl
                    check_flag2 = 0;
                    while (ftell(fd) <= pos) {
                        if (fread(chSize, 1, 4, fd) != 4) break;		//get address
                        if (fread(chFlag, 1, 4, fd) != 4) break;
                        size = ENDIAN_CONV(chSize);		//convert size
                        LOGI("                                        [0x%08X]% 4d-%s - Pos:0x%08X\n", *((unsigned int*)chSize), size, chFlag, pos);


                        if (strcmp(chFlag, "stsz") == 0) {

                            char bufFPS[8] = { 0 };
                            unsigned int AllFPSCunts = 0;

                            int64_t stbladdr = ftell(fd);
                            Rdata rdata_rd;

                            fseek(fd, 8, SEEK_CUR);
                            if (fread(bufFPS, 1, 4, fd) != 4) break;

                            rdata_rd.b[3] = bufFPS[0];
                            rdata_rd.b[2] = bufFPS[1];
                            rdata_rd.b[1] = bufFPS[2];
                            rdata_rd.b[0] = bufFPS[3];


                            AllFPSCunts = rdata_rd.a;

                            //VideoFPS = AllFPSCunts / (_duration / _timescale);
                            VideoFPS = (AllFPSCunts*_timescale)/_duration;
                            //   LOGI("######################## VideoFPS: %d \n", VideoFPS);
                            fseek(fd, stbladdr, SEEK_SET);
                        }

                        if ((track_id == 1 && strcmp(chFlag, "stsz") == 0)
                            || ((track_id > 1 && strcmp(chFlag, "stco") == 0))) {
                            check_flag2 = 1;
                            break;
                        }
                        fseek(fd, size - 8, SEEK_CUR);
                    }
                    if (check_flag2 == 0) break;

                    unsigned int stsz_addr = ftell(fd);
                    unsigned int stsz_size = size;
                    unsigned int addr = 0;
                    unsigned int *alladdr;
                    int gps_rec_cnt = 0;

                    if (track_id == 1) {	//only video tracker
                        size = (size - 8) / 4;					  //4bytes for every frame size length
                        if (size > 0) {

                            //gps_rec_cnt = size / 30 + 1;
                            gps_rec_cnt = size / VideoFPS + 1;

                            alladdr = (unsigned int *)malloc(gps_rec_cnt * sizeof(unsigned int));
                        }
                        else {
                            break;
                        }
                        LOGI("	Founded %d stsz indexs! mdat_addr:0x%08X, curr:0x%08X\n", size, mdat_addr, ftell(fd));

                        //Step8. Read video frame size from STSZ
                        fseek(fd, 8, SEEK_CUR);	//Skip the first 8 bytes
                        for (i = 0; i < size; i++) {
                            unsigned int addr = 0, rval = 0, idx;
                            memset(chSize, 0, 8);
                            fread(chSize, 1, 4, fd);
                            addr = ENDIAN_CONV(chSize);
                            //LOGI("STSZ    idx%d: 0x%08X\n", i, addr);

//							idx = (i + 1) / 30;
//							if (i > 0 && (i % 30) == 0) {	//Read video frame size by every 30fps(at the 31frame)

                            idx = (i + 1) / VideoFPS;
                            if (i > 0 && (i % VideoFPS) == 0) {			//Read video frame size by every 30fps(at the 31frame)

                                alladdr[idx - 1] = addr;		//video frame size
                                LOGI("\tSTSZ    idx%d: 0x%08X\n", idx, addr);
                            }
                        }

                        //Read video frame offset from STCO
                        fseek(fd, stsz_addr + stsz_size - 8, SEEK_SET);
                        if (fread(chSize, 1, 4, fd) != 4) break;   //[4] size
                        if (fread(chFlag, 1, 4, fd) != 4) break;   //[4] flag
                        size = ENDIAN_CONV(chSize);
                        size = (size - 8) / 4;
                        if (size > 0) {
                            LOGI("	Founded %d %s indexs(0x%08X)! mdat_addr:0x%08X, curr:0x%08X\n", size, chFlag, stsz_addr, mdat_addr, ftell(fd));
                            //Read video frame size from STCO
                            fseek(fd, 8, SEEK_CUR);	//Skip the first 8 bytes
                            for (i = 0; i < size; i++) {
                                unsigned int addr = 0, rval = 0, idx;
                                memset(chSize, 0, 8);
                                fread(chSize, 1, 4, fd);
                                addr = ENDIAN_CONV(chSize);
                                //LOGI("STCO    idx%d: 0x%08X\n", i, addr);

                                idx = (i + 1) / 30;
                                if (i > 0 && ((i + 1) % 30) == 0 && idx <= gps_rec_cnt) {	//Read video frame offset by every 30fps(at the 30frame!!!! Fuck!!!!)
                                    addr = alladdr[idx - 1] + addr; //Video frame size + Video frame offset
                                    addr = ((addr + 0x7fff) & 0xFFFF8000);		//Aligned to 0x8000(For wirte cluster size)
                                    alladdr[idx - 1] = addr;
                                    //  LOGI("\tSTCO    idx%d: 0x%08X\n", idx, addr);
                                }
                            }
                        }
                    }
                    else {
                        //Step8. Get index from stco
                        if (fread(chSize, 1, 4, fd) != 4) break;   //[4]
                        if (fread(chSize, 1, 4, fd) != 4) break;   //[4] index size
                        size = ENDIAN_CONV(chSize);
                        if (size > 0) {
                            alladdr = (unsigned int *)malloc((size + 1) * sizeof(unsigned int));
                        }
                        LOGI("	Founded %d indexs! mdat_addr:0x%08X, curr:0x%08X\n", size, mdat_addr, ftell(fd));
                        for (i = 0; i < size; i++) {
                            unsigned int addr = 0, rval = 0;
                            memset(chSize, 0, 8);
                            fread(chSize, 1, 4, fd);
                            addr = ENDIAN_CONV(chSize);
                            //alladdr[i] = addr; //Skip 0x00 0x35
                            //alladdr[i] = addr + 0x20000; //0x10000: cluster size: 128K   Á¢ÌåÉù: 0x20000
                            alladdr[i] = addr + 0x10000; //cluster size: 128K   µ¥ÉùµÀ: 0x10000
                            LOGI("    idx%d: 0x%08X\n", i, addr);
                        }
                        gps_rec_cnt = size;
                    }

#if 1
                    int found_key = 0;
                    int key = 0;
                    char cache[20][128] = { 0 };

                    long tellFileSize = 0;

                    for (i = 0; i < gps_rec_cnt; i++) {
                        int len;

                        RMCINFO gsdata;
                        memset(&gsdata, 0, sizeof(RMCINFO));

                        fseek(fd, alladdr[i] + 48/*72 Fuck!!!*/, SEEK_SET);
                        //fseek(fd, alladdr[i] + 48/*72 Fuck!!!*/, SEEK_SET);

                        tellFileSize = ftell(fd);

                        //fseek(fd, alladdr[i] + 72, SEEK_SET);


                        fread((char*)&gsdata, 1, sizeof(RMCINFO), fd);

                        char num[2] = { 0 };
                        key = 0;
                        num[0] = gsdata.key[(0)]; key += atoi(num);
                        num[0] = gsdata.key[(1)]; key += atoi(num);
                        num[0] = gsdata.key[(2)]; key += atoi(num);
                        num[0] = gsdata.key[(3)]; key += atoi(num);
                        num[0] = gsdata.key[(4)]; key += atoi(num);
                        num[0] = gsdata.key[(4)]; key += atoi(num);
                        num[0] = gsdata.key[(5)]; key += atoi(num);
                        num[0] = gsdata.key[(6)]; key += atoi(num);
                        num[0] = gsdata.key[(7)]; key += atoi(num);
                        num[0] = gsdata.key[(7)]; key += atoi(num);

                        LOGI("  key:%d\n", key);

                        {	//Decrypt

#if 1
                            int pos = 0, len = 128; //10 = sizeof(key)
                            char *p = gsdata.Gps_str;

                            //@liangfu 20170228 Èç¹û¿ÉÒÔ¶àÐ´Ò»´Î ÔòÏÈÅÐ¶ÏµÚÒ»¸ö×Ö½ÚÊÇ·ñÊÇ¿Õ¸ñ£¬Èç¹û²»ÊÇ¾ÍÒª¼õÈ¥Á½´Îkey¡£
                            if((p[0] - key) != ' ')
                                key = 2 * key;

                            while (len) {
                                if (len > 1 && p[pos] == 0x0D && p[pos + 1] == 0x0A) {
                                    break;
                                }
                                //p[pos] = p[pos] - key - key;

                                p[pos] = p[pos] - key;
                                pos++;
                                len--;
                            }
#endif

                            LOGI("[%d] dectyped: x:%d, y:%d, z:%d, gps:%s\n", i, gsdata.Xacc, gsdata.Yacc, gsdata.Zacc, gsdata.Gps_str);

                            AssistInfo_t	node;
                            node.north_angle = 0.0f;
                            node.spd = 0;
                            if (((gsdata.Xacc>10000) || (gsdata.Xacc<-10000)) || ((gsdata.Yacc>10000) || (gsdata.Yacc<-10000)) || ((gsdata.Zacc>10000) || (gsdata.Zacc<-10000)))//MI@20141121  Update Time Lapse Video Issue
                            {
                                gsdata.Xacc = 0;
                                gsdata.Yacc = 0;
                                gsdata.Zacc = 0;
                            }

                            node.gsensor_x = gsdata.Xacc / 1000.0f;// * 64;
                            node.gsensor_y = gsdata.Yacc / 1000.0f;// * 64;
                            node.gsensor_z = gsdata.Zacc / 1000.0f;// * 64;

                            if (gsdata.Gps_str[1] == '$' && gsdata.Gps_str[2] == 'G' && gsdata.Gps_str[3] == 'P' && gsdata.Gps_str[4] == 'G' && gsdata.Gps_str[5] == 'G' && gsdata.Gps_str[6] == 'A') {
                                ParseGGA(&gsdata.Gps_str[1], node.gps_lat, node.gps_lgt, node.spd);		//GPS info
                            }
                            else {
                                ParseRMC(&gsdata.Gps_str[1], node.gps_lat, node.gps_lgt, node.spd, node.north_angle);
                            }

                            //LOGI("[%d] dectyped: x:%d, y:%d, z:%d, gps:%s\n", i, gsdata.Xacc, gsdata.Yacc, gsdata.Zacc, gsdata.Gps_str);

                            m_infoList.push_back(node);


#if 0	//Ä£ÄâÎÄ¼þÒÔ1Hz²ÉÑù,Êµ¼ÊÊÇÓÃ10Hz²ÉÑù
                            m_infoList.AddTail(node);
                            m_infoList.AddTail(node);
                            m_infoList.AddTail(node);
                            m_infoList.AddTail(node);
                            m_infoList.AddTail(node);
                            m_infoList.AddTail(node);
                            m_infoList.AddTail(node);
                            m_infoList.AddTail(node);
                            m_infoList.AddTail(node);
#endif
                        }

                    }
#endif
                    if (alladdr != NULL) {
                        free(alladdr);
                        alladdr = NULL;
                    }
                    break;
                } while (0);

                fseek(fd, curpos, SEEK_SET);

                break;
            }
            fseek(fd, size - 8, SEEK_CUR);
        }
    } while (0);

    end:
    fclose(fd);
    fd = NULL;

    return m_infoList.size();
}

#if 0

int AssistFile::ParseAssistDataForNovatek66x(string strMOVFile)
{
    TCHAR	tFile[MAX_PATH]	 = {0};
    char	szFile[MAX_PATH] = {0};
    FILE	*fd				 = NULL;
//	char	szBuf[1024] = {0};
    char	file[MAX_PATH] = {0};
    int		flag	= 0;

    unsigned int _duration = 0, _timescale = 0;

    USES_CONVERSION;

    //Convert TCHAR to CHAR
    wsprintf(tFile, _T("%s"), strMOVFile);
    sprintf(szFile, "%s", T2A(tFile));

    //Clear list first
    m_infoList.RemoveAll();

    if(strMOVFile.GetLength() < 4) return -1;

    //Open file
    fd = fopen(szFile, "rb");
    if(fd == NULL) {
        return -1;
    }

    //moov
    //idx1
    //1. Get MOOV offset
    //2. Get audio tracker offset
    //3.    Get 01wb offset from idx1
    //4.    Get GS_data in moov by 01wb offset + 1 sdcard cluster size(0x10000)

    do {

        unsigned int ftypsize = 0, mdatsize = 0, mdat_addr = 0, moov_addr = 0, mvhd_addr = 0;
        unsigned int strm1 = 0, strm2 = 0, strm3 = 0;
        unsigned int tkhd = 0, edts = 0, mdia = 0;
        unsigned int mdhd = 0, dinf = 0, stbl = 0;
        unsigned int stsd = 0, stts = 0, stsc = 0, stsz = 0, stco = 0;
        unsigned int size = 0, pos = 0, curpos = 0;
        unsigned char chSize[12] = {0};

        char chFlag[8] = {0};
        char rbuf[128] = {0};
        char date_s[16] = {0};
        char time_s[16] = {0};
        char gps_lat[16] = {0};
        char gps_lgt[16] = {0};
        char gps_spd[16] = {0};
        int i, check_flag = 0;

        //Step1. Find MOOV
        check_flag = 0;
        while(!feof(fd)) {

            if(fread(chSize, 1, 4, fd) != 4) break;		//get address
            size = ENDIAN_CONV(chSize);					//convert size
            if(fread(chFlag, 1, 4, fd) != 4) break;

            pos += size;
            LOGI("1 [0x%08X]%s - Pos:0x%08X   file locat: 0x%x \n", *((unsigned int*)chSize), chFlag, pos, ftell(fd));

            if(strcmp(chFlag, "mdat") == 0) {

                mdat_addr = ftell(fd);
            }

            if(strcmp(chFlag, "moov") == 0) {

                check_flag = 1;
                curpos = ftell(fd);
                break;
            }

            fseek(fd, size-8, SEEK_CUR);
        }

        if(check_flag == 0) break;


        //Step2. Get Tracker
        int track_id = 0;
        check_flag = 0;

        curpos = ftell(fd);
        while(!feof(fd)) {

            if(fread(chSize, 1, 4, fd) != 4) break; 	//get address
            size = ENDIAN_CONV(chSize); 	//convert size
            if(fread(chFlag, 1, 4, fd) != 4) break;

            pos += size;
            LOGI(" 		[0x%08X]%s - Pos:0x%08X\n", *((unsigned int*)chSize), chFlag, pos);

            if(strcmp(chFlag, "udta") == 0) {

                fseek(fd, -8, SEEK_CUR);
                if(fread(chSize, 1, 4, fd) != 4) break;
                size = ENDIAN_CONV(chSize); 	//convert size

                fseek(fd, size, SEEK_CUR);  //gps flags

/*
                memset(rbuf, 0, 64);
                if(fread(chSize, 1, 4, fd) != 4) break; 	//get address

                size = ENDIAN_CONV(chSize); 	//convert size
                if(size > 64) size = 64;
*/
                //Step3. Check Tracker
                do {

            //		if(fread(rbuf, 1, size, fd) != size) break;


                    unsigned int stsz_addr = ftell(fd);
                    unsigned int stsz_size = size;
                    unsigned int addr = 0;
                    int gps_rec_cnt = 0;
                    unsigned int *alladdr;
                    unsigned int *allaframesize;

                    LOGI("1 gps flags : 0x%x  \n",  ftell(fd));


                    fseek(fd, 8, SEEK_CUR);
                    if(fread(chSize, 1, 4, fd) != 4) break;
                    gps_rec_cnt = ENDIAN_CONV(chSize);

                    if (gps_rec_cnt > 500) gps_rec_cnt = 200;
                    LOGI("2 gps flags : 0x%x  gps_rec_cnt: %d  \n",	ftell(fd), gps_rec_cnt);


                    alladdr = (unsigned int *)malloc((gps_rec_cnt+1) * sizeof(unsigned int));
                    allaframesize = (unsigned int *)malloc((gps_rec_cnt+1) * sizeof(unsigned int));

                    unsigned int idx = 0;

                    for(i = 0; i < gps_rec_cnt; i++) {

                        unsigned int addr = 0, rval = 0;

                        memset(chSize, '\0', 8);

                        if(fread(chSize, 1, 4, fd) != 4) break;
                        addr = ENDIAN_CONV(chSize);
                        alladdr[idx] = addr;

                        if(fread(chSize, 1, 4, fd) != 4) break;
                        allaframesize[idx] = 0;

                        LOGI("\t 1STCO    idx%d: 0x%08X  size:  0x%08X \n", idx, addr, allaframesize[idx]);
                        idx++;
                    }

#if 1

                    int key ;
                    long tellFileSize = 0;

                    for(i = 0; i < gps_rec_cnt; i++) {

                        int len;
                        __int64 gpsseek = 0;

                        RMCINFO gsdata;
                        memset(&gsdata, 0, sizeof(RMCINFO));

                        gpsseek = alladdr[i] + allaframesize[i] + 48;// 16;//48;
                        fseek(fd, gpsseek, SEEK_SET);


                        //tellFileSize = ftell(fd);
                        //fseek(fd, alladdr[i] + 72, SEEK_SET);
                        LOGI("%d  alladdr: 0x%x \n", i, alladdr[i]);
                        LOGI("%d  allaframesize: 0x%x \n", i, allaframesize[i]);
                        LOGI("gpsseek: 0x%x \n", gpsseek);

                        fread((char*)&gsdata, 1, sizeof(RMCINFO), fd);

                        {	//Decrypt

                            char num[2] = {0};
                            key = 0;
                            num[0] = gsdata.key[(0)]; key += atoi(num);
                            num[0] = gsdata.key[(1)]; key += atoi(num);
                            num[0] = gsdata.key[(2)]; key += atoi(num);
                            num[0] = gsdata.key[(3)]; key += atoi(num);
                            num[0] = gsdata.key[(4)]; key += atoi(num);
                            num[0] = gsdata.key[(4)]; key += atoi(num);
                            num[0] = gsdata.key[(5)]; key += atoi(num);
                            num[0] = gsdata.key[(6)]; key += atoi(num);
                            num[0] = gsdata.key[(7)]; key += atoi(num);
                            num[0] = gsdata.key[(7)]; key += atoi(num);
                            //LOGI("  key:%d\n", key);


//LOGI("[%d] dectyped: x:%d, y:%d, z:%d, gps:%s\n", i, gsdata.Xacc, gsdata.Yacc, gsdata.Zacc, gsdata.Gps_str);
#if 0

                            int pos = 0, len = 128; //10 = sizeof(key)
                            char *p = gsdata.Gps_str;

                            while(len) {
                                if(len > 1 && p[pos] == 0x0D && p[pos+1] == 0x0A) {
                                    break;
                                }
                                p[pos] = p[pos] - key;
                                pos ++;
                                len--;
                            }
#endif

//LOGI("[%d] dectyped: x:%d, y:%d, z:%d, gps:%s\n", i, gsdata.Xacc, gsdata.Yacc, gsdata.Zacc, gsdata.Gps_str);

                            AssistInfo_t	node;
                            node.north_angle = 0.0f;
                            node.spd = 0;
                            if(((gsdata.Xacc>10000)||(gsdata.Xacc<-10000))||((gsdata.Yacc>10000)||(gsdata.Yacc<-10000))||((gsdata.Zacc>10000)||(gsdata.Zacc<-10000)))//MI@20141121  Update Time Lapse Video Issue
                            {
                                gsdata.Xacc = 0;
                                gsdata.Yacc = 0;
                                gsdata.Zacc = 0;
                            }

                            node.gsensor_x =(gsdata.Xacc - 800) / 1000.0f;// * 64;	//Adam, For NBDVR
                            node.gsensor_y =gsdata.Yacc / 1000.0f;// * 64;
                            node.gsensor_z =gsdata.Zacc / 1000.0f;// * 64;

                            if(gsdata.Gps_str[0] == '$' && gsdata.Gps_str[1] == 'G' && gsdata.Gps_str[2] == 'P' && gsdata.Gps_str[3] == 'G' && gsdata.Gps_str[4] == 'G' && gsdata.Gps_str[5] == 'A') {
                                ParseGGA(&gsdata.Gps_str[1], node.gps_lat, node.gps_lgt, node.spd);		//GPS info
                            } else {
                                ParseRMC(&gsdata.Gps_str[1], node.gps_lat, node.gps_lgt, node.spd, node.north_angle);
                            }

LOGI("[%d] dectyped: lat: %s  lng: %s  spd: %d  angle: %f \n", i, node.gps_lat, node.gps_lgt, node.spd, node.north_angle);

                            m_infoList.AddTail(node);
                        }

                    }
#endif
                    if(alladdr != NULL) {
                        free(alladdr);
                        alladdr = NULL;
                    }
                    break;
                }while(0);

                fseek(fd, curpos, SEEK_SET);
                break;
            }

            fseek(fd, size-8, SEEK_CUR);
        }
    }while(0);

end:
    fclose(fd);
    fd = NULL;

    return m_infoList.GetCount();
}

#else

int AssistFile::ParseAssistDataForNovatek66x(string strMOVFile)
{
    char	tFile[MAX_PATH]	 = {0};
    char	szFile[MAX_PATH] = {0};
    FILE	*fd				 = NULL;
//	char	szBuf[1024] = {0};
    char	file[MAX_PATH] = {0};
    int		flag	= 0;

    int _duration = 0, _timescale = 0;



    //Convert TCHAR to CHAR


#if defined(Q_OS_WIN64)  || defined(Q_OS_WIN32)
    sprintf(tFile, "%s", strMOVFile.toLocal8Bit().data());
#elif defined(Q_OS_MAC64)
    sprintf(tFile, "%s", strMOVFile.toStdString().data());
#endif


    sprintf(szFile, "%s", tFile);

    //Clear list first
    m_infoList.clear();

    if(strMOVFile.size() < 4) return -1;

    //Open file
    fd = fopen(szFile, "rb");
    if(fd == NULL) {
        return -1;
    }

    //moov
    //idx1
    //1. Get MOOV offset
    //2. Get audio tracker offset
    //3.    Get 01wb offset from idx1
    //4.    Get GS_data in moov by 01wb offset + 1 sdcard cluster size(0x10000)

    do {

        unsigned int ftypsize = 0, mdatsize = 0, mdat_addr = 0, moov_addr = 0, mvhd_addr = 0;
        unsigned int strm1 = 0, strm2 = 0, strm3 = 0;
        unsigned int tkhd = 0, edts = 0, mdia = 0;
        unsigned int mdhd = 0, dinf = 0, stbl = 0;
        unsigned int stsd = 0, stts = 0, stsc = 0, stsz = 0, stco = 0;
        unsigned int size = 0, pos = 0, curpos = 0;
        unsigned char chSize[12] = {0};

        char chFlag[8] = {0};
        char rbuf[128] = {0};
        char date_s[16] = {0};
        char time_s[16] = {0};
        char gps_lat[16] = {0};
        char gps_lgt[16] = {0};
        char gps_spd[16] = {0};
        int i, check_flag = 0;

        //Step1. Find MOOV
        check_flag = 0;
        while(!feof(fd)) {

            if(fread(chSize, 1, 4, fd) != 4) break;		//get address
            size = ENDIAN_CONV(chSize);					//convert size
            if(fread(chFlag, 1, 4, fd) != 4) break;

            pos += size;
            LOGI("1 [0x%08X]%s - Pos:0x%08X\n", *((unsigned int*)chSize), chFlag, pos);

            if(strcmp(chFlag, "mdat") == 0) {
                mdat_addr = ftell(fd);
            }

            if(strcmp(chFlag, "moov") == 0) {
                check_flag = 1;
                curpos = ftell(fd);
                break;
            }
            fseek(fd, size-8, SEEK_CUR);
        }

        if(check_flag == 0) break;

        //Step2. Get Tracker
        int track_id = 0;
        check_flag = 0;
        while(!feof(fd)) {

            if(fread(chSize, 1, 4, fd) != 4) break;		//get address
            size = ENDIAN_CONV(chSize);					//convert size
            if(fread(chFlag, 1, 4, fd) != 4) break;

            pos += size;
            LOGI("2         [0x%08X]%s - Pos:0x%08X\n", *((unsigned int*)chSize), chFlag, pos);

            if(strcmp(chFlag, "trak") == 0) {	//only 1 track

                curpos = ftell(fd);
                track_id ++;

                //if(track_id == 2) {		//track1: video, track2: audio
                check_flag = 1;
                //}
            }

#if 0
            if(track_id > 0 && strcmp(chFlag, "trak") != 0) {	//only 1 track
                check_flag = 1;
                fseek(fd, curpos, SEEK_SET);
            }

            if(strcmp(chFlag, "trak") == 0) {
                curpos = ftell(fd);
                track_id ++;
                if(track_id == 2) {		//track1: video, track2: audio
                    check_flag = 1;
                }
            }
#endif

            if(check_flag == 1) {
                //Step3. Check Tracker
                do {

                    //Step4. Get mdia from trak
                    int check_flag2 = 0;
                    while(ftell(fd) <= pos) {

                        if(fread(chSize, 1, 4, fd) != 4) break;		//get address
                        size = ENDIAN_CONV(chSize);					//convert size
                        if(fread(chFlag, 1, 4, fd) != 4) break;

                        LOGI("3                [0x%08X]%s - Pos:0x%08X\n", *((unsigned int*)chSize), chFlag, pos);
                        if(strcmp(chFlag, "mdia") == 0) {
                            check_flag2 = 1;
                            break;
                        }

                        fseek(fd, size-8, SEEK_CUR);
                    }
                    if(check_flag2 == 0) break;

                    //Step5. Get minf from mdia
                    check_flag2 = 0;
                    while(ftell(fd) <= pos) {

                        if(fread(chSize, 1, 4, fd) != 4) break;		//get address
                        size = ENDIAN_CONV(chSize);					//convert size
                        if(fread(chFlag, 1, 4, fd) != 4) break;

                        LOGI("4                        [0x%08X]%s - Pos:0x%08X\n", *((unsigned int*)chSize), chFlag, pos);

#if 1
                        if (strcmp(chFlag, "mdhd") == 0) {

                            char bufFPS[24] = { 0 };
                            unsigned int AllFPSCunts = 0;
                            int64_t stbladdr = ftell(fd);
                            Rdata rdata_rd;


                            LOGI("mdhd  Pos:0x%08X\n", stbladdr);


                            fseek(fd, 12, SEEK_CUR);
                            //fread(bufFPS, 1, 12, fd);


                            memset(bufFPS, 0, 24);
                            if (fread(bufFPS, 1, 4, fd) != 4) break;

                            for(int i=0; i<4; i++)



                                rdata_rd.b[3] = bufFPS[0];
                            rdata_rd.b[2] = bufFPS[1];
                            rdata_rd.b[1] = bufFPS[2];
                            rdata_rd.b[0] = bufFPS[3];

                            _timescale = rdata_rd.a;

                            memset(bufFPS, 0, 24);
                            if (fread(bufFPS, 1, 4, fd) != 4) break;

                            for (int i = 0; i<4; i++)


                                rdata_rd.b[3] = bufFPS[0];
                            rdata_rd.b[2] = bufFPS[1];
                            rdata_rd.b[1] = bufFPS[2];
                            rdata_rd.b[0] = bufFPS[3];
                            _duration = rdata_rd.a;
                            //   LOGI("######################## timescale: %d  duration:  %d \n", _timescale, _duration);
                            fseek(fd, stbladdr, SEEK_SET);
                        }
#endif
                        if(strcmp(chFlag, "minf") == 0) {
                            check_flag2 = 1;
                            break;
                        }

                        fseek(fd, size-8, SEEK_CUR);
                    }
                    if(check_flag2 == 0) break;

                    //Step6. Get stbl from minf
                    check_flag2 = 0;
                    while(ftell(fd) <= pos) {

                        if(fread(chSize, 1, 4, fd) != 4) break;		//get address
                        size = ENDIAN_CONV(chSize);					//convert size
                        if(fread(chFlag, 1, 4, fd) != 4) break;

                        LOGI("5                                [0x%08X]%s - Pos:0x%08X\n", *((unsigned int*)chSize), chFlag, pos);
                        if(strcmp(chFlag, "stbl") == 0) {
                            check_flag2 = 1;
                            break;
                        }

                        fseek(fd, size-8, SEEK_CUR);
                    }

                    if(check_flag2 == 0) break;

                    //@liangfu
                    int64_t stbladdr = ftell(fd);


                    //Step7. Get stco from stbl
                    check_flag2 = 0;
                    while(ftell(fd) <= pos) {

                        if(fread(chSize, 1, 4, fd) != 4) break;		//get address
                        if(fread(chFlag, 1, 4, fd) != 4) break;

                        size = ENDIAN_CONV(chSize);					//convert size
                        LOGI("                                     [0x%08X]% 4d-%s - Pos:0x%08X\n", *((unsigned int*)chSize), size, chFlag, pos);

                        if (strcmp(chFlag, "stsz") == 0) {

                            char bufFPS[8] = { 0 };
                            int AllFPSCunts = 0;

                            int64_t stbladdr = ftell(fd);
                            Rdata rdata_rd;

                            fseek(fd, 8, SEEK_CUR);
                            if (fread(bufFPS, 1, 4, fd) != 4) break;

                            rdata_rd.b[3] = bufFPS[0];
                            rdata_rd.b[2] = bufFPS[1];
                            rdata_rd.b[1] = bufFPS[2];
                            rdata_rd.b[0] = bufFPS[3];

                            AllFPSCunts = rdata_rd.a;
                            // VideoFPS = AllFPSCunts / (_duration / _timescale);
                            VideoFPS = (AllFPSCunts*_timescale)/_duration;
                            //   LOGI("######################## VideoFPS: %d \n", VideoFPS);
                            fseek(fd, stbladdr, SEEK_SET);
                        }

                        if(strcmp(chFlag, "stco") == 0) {
                            check_flag2 = 1;
                            break;
                        }
#if 0
                        if((track_id == 1 && strcmp(chFlag, "stsz") == 0)
                            || ((track_id > 1 && strcmp(chFlag, "stco") == 0))) {
                            check_flag2 = 1;
                            break;
                        }
#endif
                        fseek(fd, size-8, SEEK_CUR);
                    }

                    if(check_flag2 == 0) break;

                    unsigned int stsz_addr = ftell(fd);
                    unsigned int stsz_size = size;
                    unsigned int addr = 0;
                    int gps_rec_cnt = 0;
                    unsigned int *alladdr;
                    unsigned int *allaframesize;

                    //@liangfu 20160804.
                    {
                        size = (size - 8) / 4;					  //4bytes for every frame size length
                        if(size > 0) {

                            //gps_rec_cnt = size / 30 + 1;
                            gps_rec_cnt = size / VideoFPS + 1;

                            alladdr = (unsigned int *)malloc(gps_rec_cnt * sizeof(unsigned int));
                            allaframesize = (unsigned int *)malloc(gps_rec_cnt * sizeof(unsigned int));
                        } else {

                            break;
                        }

                        LOGI("6	Founded %d stsz indexs! mdat_addr:0x%08X, curr:0x%08X\n", size, mdat_addr, ftell(fd));

                        //Step8. Read video frame size from STSZ
                        //fseek(fd, 8, SEEK_CUR);	//Skip the first 8 bytes
                        fseek(fd, 4, SEEK_CUR);	//Skip the first 8 bytes
                        for(i = 0; i < size; i++) {

                            unsigned int addr = 0, rval = 0, idx;

                            memset(chSize, 0, 8);
                            fread(chSize, 1, 4, fd);
                            addr = ENDIAN_CONV(chSize);

                            //LOGI("STCO    idx%d: 0x%08X \n", i, addr);

                            //idx = (i + 1) / 30;
                            //if(i > 0 && (i % 30) == 0) {			//Read video frame size by every 30fps(at the 31frame)

                            idx = (i + 1) / VideoFPS;
                            if (i > 0 && (i % VideoFPS) == 0) {			//Read video frame size by every 30fps(at the 31frame)


                                alladdr[idx - 1] = addr;			//video frame size

                                int64_t stbladdr = ftell(fd);
                                fseek(fd, alladdr[idx - 1], SEEK_SET);

                                memset(chSize, 0, 8);
                                fread(chSize, 1, 4, fd);
                                allaframesize[idx - 1] = ENDIAN_CONV(chSize) + 4;

                                fseek(fd, stbladdr, SEEK_SET);

                                //  LOGI("\tSTCO    idx%d: 0x%08X  size:  0x%08X \n", idx, addr, allaframesize[idx - 1]);
                            }
                        }
                    }


#if 0
                    if(track_id == 1) {	//only video tracker

                        size = (size - 8) / 4;					  //4bytes for every frame size length
                        if(size > 0) {
                            gps_rec_cnt = size / 30 + 1;
                            alladdr = (unsigned int *)malloc(gps_rec_cnt * sizeof(unsigned int));
                        } else {
                            break;
                        }
                        LOGI("6	Founded %d stsz indexs! mdat_addr:0x%08X, curr:0x%08X\n", size, mdat_addr, ftell(fd));

                        //Step8. Read video frame size from STSZ
                        fseek(fd, 8, SEEK_CUR);	//Skip the first 8 bytes
                        for(i = 0; i < size; i++) {

                            unsigned int addr = 0, rval = 0, idx;
                            memset(chSize, 0, 8);
                            fread(chSize, 1, 4, fd);
                            addr = ENDIAN_CONV(chSize);
                            //LOGI("STSZ    idx%d: 0x%08X\n", i, addr);

                            idx = (i + 1) / 30;
                            if(i > 0 && (i % 30) == 0) {			//Read video frame size by every 30fps(at the 31frame)
                                alladdr[idx - 1] = addr;			//video frame size
                                LOGI("\tSTSZ    idx%d: 0x%08X\n", idx, addr);
                            }
                        }

                        //Read video frame offset from STCO
                        fseek(fd, stsz_addr + stsz_size - 8, SEEK_SET);

                        if(fread(chSize, 1, 4, fd) != 4) break;   //[4] size
                        if(fread(chFlag, 1, 4, fd) != 4) break;   //[4] flag

                        size = ENDIAN_CONV(chSize);
                        size = (size - 8) / 4;
                        if(size > 0) {

                            LOGI("	Founded %d %s indexs(0x%08X)! mdat_addr:0x%08X, curr:0x%08X\n", size, chFlag, stsz_addr, mdat_addr, ftell(fd));
                            //Read video frame size from STCO
                            fseek(fd, 8, SEEK_CUR);	//Skip the first 8 bytes
                            for(i = 0; i < size; i++) {

                                unsigned int addr = 0, rval = 0, idx;
                                memset(chSize, 0, 8);
                                fread(chSize, 1, 4, fd);
                                addr = ENDIAN_CONV(chSize);
                                //LOGI("STCO    idx%d: 0x%08X\n", i, addr);

                                idx = (i + 1) / 30;
                                if(i > 0 && ((i + 1) % 30) == 0 && idx <= gps_rec_cnt) {	//Read video frame offset by every 30fps(at the 30frame!!!! Fuck!!!!)

                                    addr = alladdr[idx-1] + addr; //Video frame size + Video frame offset
                                    addr = ((addr + 0x7fff) & 0xFFFF8000);		//Aligned to 0x8000(For wirte cluster size)
                                    alladdr[idx-1] = addr;
                                    LOGI("\tSTCO    idx%d: 0x%08X\n", idx, addr);
                                }
                            }
                        }
                    } else {

                        //Step8. Get index from stco
                        if(fread(chSize, 1, 4, fd) != 4) break;   //[4]
                        if(fread(chSize, 1, 4, fd) != 4) break;   //[4] index size

                        size = ENDIAN_CONV(chSize);
                        if(size > 0) {
                            alladdr = (unsigned int *)malloc((size + 1)*sizeof(unsigned int));
                        }

                        LOGI("	Founded %d indexs! mdat_addr:0x%08X, curr:0x%08X\n", size, mdat_addr, ftell(fd));

                        for(i = 0; i < size; i++) {

                            unsigned int addr = 0, rval = 0;

                            memset(chSize, 0, 8);
                            fread(chSize, 1, 4, fd);
                            addr = ENDIAN_CONV(chSize);

                            //alladdr[i] = addr; //Skip 0x00 0x35
                            //alladdr[i] = addr + 0x20000; //0x10000: cluster size: 128K   Á¢ÌåÉù: 0x20000
                            alladdr[i] = addr + 0x10000; //cluster size: 128K   µ¥ÉùµÀ: 0x10000

                            LOGI("7    idx%d: 0x%08X  0x%08X\n", i, addr,alladdr[i]);
                        }

                        gps_rec_cnt = size;
                    }
#endif



#if 1
                    int found_key = 0;
                    int key = 0;
                    char cache[20][128] = {0};

                    long tellFileSize = 0;

                    for(i = 0; i < gps_rec_cnt; i++) {

                        int len;
                        int64_t gpsseek = 0;

                        RMCINFO gsdata;
                        memset(&gsdata, 0, sizeof(RMCINFO));

                        //fseek(fd, alladdr[i] + 48/*72 Fuck!!!*/, SEEK_SET);
                        //fseek(fd, alladdr[i] + 48/*72 Fuck!!!*/, SEEK_SET);
                        //fseek(fd, alladdr[i]+48, SEEK_SET);



                        gpsseek = alladdr[i]+allaframesize[i]+48;

                        // gpsseek = alladdr[i] + allaframesize[i] + 16;  //CDV16.

                        // LOGI("gpsseek: %d \n", gpsseek);

                        fseek(fd, gpsseek, SEEK_SET);

                        //tellFileSize = ftell(fd);

                        //fseek(fd, alladdr[i] + 72, SEEK_SET);


                        fread((char*)&gsdata, 1, sizeof(RMCINFO), fd);


//解密
#if 1
                        char num[2] = {0};
                        key = 0;
                        num[0] = gsdata.key[(0)]; key += atoi(num);
                        num[0] = gsdata.key[(1)]; key += atoi(num);
                        num[0] = gsdata.key[(2)]; key += atoi(num);
                        num[0] = gsdata.key[(3)]; key += atoi(num);
                        num[0] = gsdata.key[(4)]; key += atoi(num);
                        num[0] = gsdata.key[(4)]; key += atoi(num);
                        num[0] = gsdata.key[(5)]; key += atoi(num);
                        num[0] = gsdata.key[(6)]; key += atoi(num);
                        num[0] = gsdata.key[(7)]; key += atoi(num);
                        num[0] = gsdata.key[(7)]; key += atoi(num);
                        //     LOGI("=======  key:%d  %s\n", key, gsdata.key);
#endif

                        {	//Decrypt

#if 0
                            int pos = 0, len = 128; //10 = sizeof(key)
                            char *p = gsdata.Gps_str;

                            while(len) {
                                if(len > 1 && p[pos] == 0x0D && p[pos+1] == 0x0A) {
                                    break;
                                }
                                p[pos] = p[pos] - key;
                                pos ++;
                                len--;
                            }
#endif

                            //CDV16

#if 0
                            // if (strncmp(gsdata.Gps_str, " $GPRMC", 7) || strncmp(gsdata.Gps_str, " $GNRMC", 7)  || strncmp(gsdata.Gps_str, " $GLRMC", 7) )
                            if(NULL == strstr(gsdata.Gps_str, "RMC"))
                            {

                                gpsseek = alladdr[i] + allaframesize[i] + 16;
                                fseek(fd, gpsseek, SEEK_SET);
                                fread((char*)&gsdata, 1, sizeof(RMCINFO), fd);

                                //解密
                                char num[2] = {0};
                                key = 0;
                                num[0] = gsdata.key[(0)]; key += atoi(num);
                                num[0] = gsdata.key[(1)]; key += atoi(num);
                                num[0] = gsdata.key[(2)]; key += atoi(num);
                                num[0] = gsdata.key[(3)]; key += atoi(num);
                                num[0] = gsdata.key[(4)]; key += atoi(num);
                                num[0] = gsdata.key[(4)]; key += atoi(num);
                                num[0] = gsdata.key[(5)]; key += atoi(num);
                                num[0] = gsdata.key[(6)]; key += atoi(num);
                                num[0] = gsdata.key[(7)]; key += atoi(num);
                                num[0] = gsdata.key[(7)]; key += atoi(num);
                           //     LOGI("=======  key:%d  %s\n", key, gsdata.key);



                                int pos = 0, len = 128; //10 = sizeof(key)
                                char *p = gsdata.Gps_str;

                                while(len) {
                                    if(len > 1 && p[pos] == 0x0D && p[pos+1] == 0x0A) {
                                        break;
                                    }
                                    p[pos] = p[pos] - key;
                                    pos ++;
                                    len--;
                                }

                            }
#endif

                            //LOGI("#################[%d] dectyped: x:%d, y:%d, z:%d, gps:%s\n", i, gsdata.Xacc, gsdata.Yacc, gsdata.Zacc,gsdata.Gps_str);

                            AssistInfo_t	node;
                            node.north_angle = 0.0f;
                            node.spd = 0;
                            if(((gsdata.Xacc>10000)||(gsdata.Xacc<-10000))||((gsdata.Yacc>10000)||(gsdata.Yacc<-10000))||((gsdata.Zacc>10000)||(gsdata.Zacc<-10000)))//MI@20141121  Update Time Lapse Video Issue
                            {
                                gsdata.Xacc = 0;
                                gsdata.Yacc = 0;
                                gsdata.Zacc = 0;
                            }

                            if (i >= gps_rec_cnt - 2)
                            {		//Adam for NBDVR
                                gsdata.Xacc = 823;
                                gsdata.Yacc = 150;
                                gsdata.Zacc = 96;
                            }
                            node.gsensor_x =(gsdata.Xacc - 800) / 1000.0f;// * 64;	//Adam, For NBDVR
                            node.gsensor_y =gsdata.Yacc / 1000.0f;// * 64;
                            node.gsensor_z =gsdata.Zacc / 1000.0f;// * 64;



                            if(gsdata.Gps_str[1] == '$' && gsdata.Gps_str[2] == 'G' && gsdata.Gps_str[3] == 'P' && gsdata.Gps_str[4] == 'G' && gsdata.Gps_str[5] == 'G' && gsdata.Gps_str[6] == 'A') {
                                ParseGGA(&gsdata.Gps_str[1], node.gps_lat, node.gps_lgt, node.spd);		//GPS info
                            } else {
                                ParseRMC(&gsdata.Gps_str[1], node.gps_lat, node.gps_lgt, node.spd, node.north_angle);
                            }


                            // LOGI()<<"gsdata.Gps_str:"<<gsdata.Gps_str;


                            // LOGI("[%d] #### dectyped: x:%d, y:%d, z:%d, gps:%s,lat:%s\n", i, gsdata.Xacc, gsdata.Yacc, gsdata.Zacc, gsdata.Gps_str,node.gps_lat.toStdString().data());

                            m_infoList.push_back(node);
                        }
                    }
#endif
                    if(alladdr != NULL) {
                        free(alladdr);
                        alladdr = NULL;
                    }
                    break;
                }while(0);

                fseek(fd, curpos, SEEK_SET);
                break;
            }

            fseek(fd, size-8, SEEK_CUR);
        }
    }while(0);

    end:
    fclose(fd);
    fd = NULL;

    return m_infoList.size();
}
#endif


#endif

int char2Int(char* buf,int offset)
{
    int num;
    num = buf[offset+3] & 0xFF;
    // "|="按位或赋值。
    num |= ((buf[offset + 2] << 8) & 0xFF00);
    num |= ((buf[offset + 1] << 16) & 0xFF0000);
    num |= ((buf[offset] << 24) & 0xFF000000);
    return num;
}


int AssistFile::ParseAssistDataForNovatek9667XA(string strMOVFile,int itemNum)
{
    char	tFile[MAX_PATH]	 = {0};
    char	szFile[MAX_PATH] = {0};
    FILE	*fd				 = NULL;

    sprintf(tFile, "%s", strMOVFile.data());

    sprintf(szFile, "%s", tFile);

    //Clear list first
    m_infoList.clear();
    fd = fopen(szFile, "rb");

    fseek(fd,0,SEEK_END);
    int flen=ftell(fd); /* 得到文件大小 */
    int countSize = itemNum*8+8; //itemNum 表示多少条,每条8位 加上8 是前面的数据的头(前面4个字节表示->版本号;后面4个字节表示条目 )
    char* buf = (char*)malloc(countSize);
    //文件总长度减去countSize(GPS标识的总长度),就是GPS标识的当前的位置,因为数据在文件末尾所以采取这样的方式比较简单
    fseek(fd,flen-countSize,SEEK_SET);
    int rc = fread(buf,sizeof(char),countSize,fd);


    //前面8个字节的数据 表示版本号和长度
    char* hearBuf = (char*)malloc(8);
    memcpy(hearBuf,buf,8);


    int dataLen = countSize-8;
    //拷贝的数据前面4个字节表示偏移量 后4个字节表示长度
    char* contentBuf = (char*)malloc(dataLen);
    memset(contentBuf,0,dataLen);
    memcpy(contentBuf,buf+8,dataLen);

    int num = dataLen/8;
    char* tmpData = (char*)malloc(8);

    char* tmpOffset = (char*)malloc(4);
    char* tmpLenth = (char*)malloc(4);


    for(int i=0;i<num;i++)
    {
        memset(tmpData,0,8);
        memcpy(tmpData,contentBuf+i*8,8);
        //   fwrite(tmpData,8,1,WFile);
        //从tmpData里面拷贝出偏移量和长度
        memset(tmpOffset,0,4);
        memset(tmpLenth,0,4);

        memcpy(tmpOffset,tmpData,4);
        memcpy(tmpLenth,tmpData+4,4);


        //偏移位
        int offsetInt = char2Int(tmpOffset,0);
        //长度
        int lenInt = char2Int(tmpLenth,0);

        char* gpsbuf = (char*)malloc(lenInt);

        fseek(fd,offsetInt,SEEK_SET);
        //5120是自定义的数据可以去掉
        fread(gpsbuf,sizeof(char),lenInt-5120,fd);

        NTGPSInfo_s *track = (NTGPSInfo_s*)gpsbuf;
        track->rmcinfo.Gps_str;
        AssistInfo_t	node;
        node.north_angle = 0.0f;
        node.spd = 0;
        if(((track->rmcinfo.Xacc>10000)||(track->rmcinfo.Xacc<-10000))||((track->rmcinfo.Yacc>10000)||(track->rmcinfo.Yacc<-10000))||((track->rmcinfo.Zacc>10000)||(track->rmcinfo.Zacc<-10000)))//MI@20141121  Update Time Lapse Video Issue
        {
            track->rmcinfo.Xacc = 0;
            track->rmcinfo.Yacc = 0;
            track->rmcinfo.Zacc = 0;
        }


        node.gsensor_x =(track->rmcinfo.Xacc) / 1024.0f;// * 64;	//Adam, For NBDVR
        node.gsensor_y =track->rmcinfo.Yacc / 1024.0f;// * 64;
        node.gsensor_z =track->rmcinfo.Zacc / 1024.0f;// * 64;


        if(track->rmcinfo.Gps_str[1] == '$' && track->rmcinfo.Gps_str[2] == 'G' && track->rmcinfo.Gps_str[3] == 'P' && track->rmcinfo.Gps_str[4] == 'G' && track->rmcinfo.Gps_str[5] == 'G' && track->rmcinfo.Gps_str[6] == 'A') {

            ParseGGA(&track->rmcinfo.Gps_str[1], node.gps_lat, node.gps_lgt, node.spd);		//GPS info
        } else {

            ParseRMC(&track->rmcinfo.Gps_str[1], node.gps_lat, node.gps_lgt, node.spd, node.north_angle);
        }
        m_infoList.push_back(node);

        // LOGI("lat:%s,gps_lgt:%s",node.gps_lat.data(),node.gps_lgt.data());

        // fwrite(track->rmcinfo.Gps_str,sizeof(track->rmcinfo.Gps_str),1,WFile);
        free(gpsbuf);
        gpsbuf = NULL;
    }

    free(hearBuf);
    free(contentBuf);
    free(tmpData);
    free(tmpOffset);
    free(tmpLenth);

    hearBuf = NULL;
    contentBuf = NULL;
    tmpData = NULL;
    tmpOffset = NULL;
    tmpLenth = NULL;


    //fwrite(buf,488,1,WFile);
//        fwrite(contentBuf,dataLen,1,WFile);
    //  fclose(WFile);

    return m_infoList.size();
}




//@20130627
int AssistFile::ParseMOVSubtitle(string strMOVFile)
{
    char	tFile[MAX_PATH]	 = {0};
    char	szFile[MAX_PATH] = {0};
    FILE	*fd				 = NULL;
//	char	szBuf[1024] = {0};
    char	file[MAX_PATH] = {0};
    int		flag	= 0;

    //Convert TCHAR to CHAR

#if defined(Q_OS_WIN64)  || defined(Q_OS_WIN32)
    sprintf(tFile, "%s", strMOVFile.toLocal8Bit().data());
#elif defined(Q_OS_MAC64)
    sprintf(tFile, "%s", strMOVFile.toStdString().data());
#endif

    sprintf(szFile, "%s", tFile);
    //Clear list first
    m_infoList.clear();

    if(strMOVFile.length() < 4) return -1;

    //Open file
    fd = fopen(szFile, "rb");
    if(fd == NULL) {
        return -1;
    }

    //moov
    //   mvhd
    //   trak (video)
    //   trak (audio)
    //   trak (text)
    //       tkhd
    //       edts
    //       mdia
    //           mdhd
    //           hdlr
    //           minf
    //               nmhd
    //               dinf
    //               stbl
    //                   stsd
    //                   stts
    //                   stsc
    //                   stsz
    //                   stco
    //                       subtitle idx(4bytes address)
    //1. Get MOOV
    //2. Get Strm Text
    //3. Get STCO

    do {
        unsigned int ftypsize = 0, mdatsize = 0, moov_addr = 0, mvhd_addr = 0;
        unsigned int strm1 = 0, strm2 = 0, strm3 = 0;
        unsigned int tkhd = 0, edts = 0, mdia = 0;
        unsigned int mdhd = 0, dinf = 0, stbl = 0;
        unsigned int stsd = 0, stts = 0, stsc = 0, stsz = 0, stco = 0;
        unsigned int size = 0, pos = 0, curpos = 0;
        unsigned char chSize[8] = {0};
        char chFlag[8] = {0};
        char rbuf[5*128] = {0};
        char date_s[16] = {0};
        char time_s[16] = {0};
        char gps_lat[16] = {0};
        char gps_lgt[16] = {0};
        char gps_spd[16] = {0};
        int i, check_flag = 0;

        //Step1. Find MOOV
        check_flag = 0;
        while(!feof(fd))
        {
            if(fread(chSize, 1, 4, fd) != 4) break;		//get address
            size = ENDIAN_CONV(chSize);		//convert size
            if(fread(chFlag, 1, 4, fd) != 4) break;

            pos += size;
            LOGI(" [0x%08X]%s - Pos:0x%08X\n", *((unsigned int*)chSize), chFlag, pos);

            if(strcmp(chFlag, "moov") == 0) {
                check_flag = 1;
                curpos = ftell(fd);
                break;
            }

            fseek(fd, size-8, SEEK_CUR);
        }
        if(check_flag == 0) break;

        //Step2. Get Tracker
        int track_id = 0;
        check_flag = 0;
        while(!feof(fd)) {
            if(fread(chSize, 1, 4, fd) != 4) break;		//get address
            size = ENDIAN_CONV(chSize);		//convert size
            if(fread(chFlag, 1, 4, fd) != 4) break;

            pos += size;
            LOGI("         [0x%08X]%s - Pos:0x%08X\n", *((unsigned int*)chSize), chFlag, pos);

            if(strcmp(chFlag, "trak") == 0) {
                curpos = ftell(fd);
                track_id ++;
                if(track_id == 3) {
                    check_flag = 1;
                    //Step3. Check Tracker
                    do {
                        //Step4. Get mdia from trak
                        int check_flag2 = 0;
                        while(ftell(fd) <= pos) {
                            if(fread(chSize, 1, 4, fd) != 4) break;		//get address
                            size = ENDIAN_CONV(chSize);		//convert size
                            if(fread(chFlag, 1, 4, fd) != 4) break;

                            LOGI("                [0x%08X]%s - Pos:0x%08X\n", *((unsigned int*)chSize), chFlag, pos);
                            if(strcmp(chFlag, "mdia") == 0) {
                                check_flag2 = 1;
                                break;
                            }
                            fseek(fd, size-8, SEEK_CUR);
                        }
                        if(check_flag2 == 0) break;

                        //Step5. Get minf from mdia
                        check_flag2 = 0;
                        while(ftell(fd) <= pos) {
                            if(fread(chSize, 1, 4, fd) != 4) break;		//get address
                            size = ENDIAN_CONV(chSize);		//convert size
                            if(fread(chFlag, 1, 4, fd) != 4) break;

                            LOGI("                        [0x%08X]%s - Pos:0x%08X\n", *((unsigned int*)chSize), chFlag, pos);
                            if(strcmp(chFlag, "minf") == 0) {
                                check_flag2 = 1;
                                break;
                            }
                            fseek(fd, size-8, SEEK_CUR);
                        }
                        if(check_flag2 == 0) break;

                        //Step6. Get stbl from minf
                        check_flag2 = 0;
                        while(ftell(fd) <= pos) {
                            if(fread(chSize, 1, 4, fd) != 4) break;		//get address
                            size = ENDIAN_CONV(chSize);		//convert size
                            if(fread(chFlag, 1, 4, fd) != 4) break;

                            LOGI("                                [0x%08X]%s - Pos:0x%08X\n", *((unsigned int*)chSize), chFlag, pos);
                            if(strcmp(chFlag, "stbl") == 0) {
                                check_flag2 = 1;
                                break;
                            }
                            fseek(fd, size-8, SEEK_CUR);
                        }
                        if(check_flag2 == 0) break;

                        //Step7. Get stco from stbl
                        check_flag2 = 0;
                        while(ftell(fd) <= pos) {
                            if(fread(chSize, 1, 4, fd) != 4) break;		//get address
                            size = ENDIAN_CONV(chSize);		//convert size
                            if(fread(chFlag, 1, 4, fd) != 4) break;

                            LOGI("                                        [0x%08X]%s - Pos:0x%08X\n", *((unsigned int*)chSize), chFlag, pos);
                            if(strcmp(chFlag, "stco") == 0) {
                                check_flag2 = 1;
                                break;
                            }
                            fseek(fd, size-8, SEEK_CUR);
                        }
                        if(check_flag2 == 0) break;

                        //Step8. Get index from stco
                        unsigned int addr = 0;
                        unsigned int *alladdr = NULL;
                        if(fread(chSize, 1, 4, fd) != 4) break;   //[4]
                        if(fread(chSize, 1, 4, fd) != 4) break;   //[4] index size

                        size = ENDIAN_CONV(chSize);

                        if(size > 0) {
                            alladdr = (unsigned int *)malloc((size + 1)*sizeof(unsigned int));
                        }

                        for(i = 0; i < size; i++) {

                            unsigned int addr = 0, rval = 0;
                            memset(chSize, 0, 8);
                            fread(chSize, 1, 4, fd);
                            addr = ENDIAN_CONV(chSize);
                            alladdr[i] = addr; //Skip 0x00 0x35
                        }

                        LOGI(" founded %d indexs!\n", size);
                        int found_key = 0;
                        int key = 0;
                        char cache[32][512] = {0};

                        for(i = 0; i < size; i++) {
                            int len;

                            memset(rbuf, 0, 5*128);
                            //fseek(fd, alladdr[i], SEEK_SET);
                            fseek(fd, alladdr[i], SEEK_SET);
                            fread(chSize, 1, 2, fd);				//[Size:2][GPS String:Size]
                            len = (chSize[0] << 8) | chSize[1];
                            fread(rbuf, 1, len, fd);
                            rbuf[0] = 0x20;
                            LOGI("#%d, Addr:0x%08X, len:%d, data:%s", i, alladdr[i], len, rbuf);

                            //A12 ²»Òª¼ÓÃÜ.
                            if ((rbuf[1] == '\t') || (rbuf[2] == '\t') || (rbuf[3] == '\t') || (rbuf[4] == '\t') || (rbuf[5] == '\t'))
                                found_key = 1;

                            if(found_key == 0) {
                                if(len == 11) {
                                    char num[2] = {0};
                                    found_key = 1;

                                    //value = Matrix[0]+Matrix[1]+Matrix[2]+Matrix[3]+Matrix[4]+ Matrix[4]+Matrix[5]
                                    //        +Matrix[6]+Matrix[7]+Matrix[7];%
#define P(x)  (1+x)
                                    num[0] = rbuf[P(0)]; key += atoi(num);
                                    num[0] = rbuf[P(1)]; key += atoi(num);
                                    num[0] = rbuf[P(2)]; key += atoi(num);
                                    num[0] = rbuf[P(3)]; key += atoi(num);
                                    num[0] = rbuf[P(4)]; key += atoi(num);
                                    num[0] = rbuf[P(4)]; key += atoi(num);
                                    num[0] = rbuf[P(5)]; key += atoi(num);
                                    num[0] = rbuf[P(6)]; key += atoi(num);
                                    num[0] = rbuf[P(7)]; key += atoi(num);
                                    num[0] = rbuf[P(7)]; key += atoi(num);

                                    LOGI("    Founded key: %s, %d, 0x%08X,0x%08X,0x%08X,0x%08X,0x%08X,0x%08X,0x%08X,0x%08X\n", rbuf, key,
                                           rbuf[P(0)],rbuf[P(1)],rbuf[P(2)],rbuf[P(3)],rbuf[P(4)],rbuf[P(5)],rbuf[P(6)],rbuf[P(7)]);
                                    LOGI("\n");

                                    int j;
                                    for(j = 0; j < i; j++)
                                    {
                                        ParseLine(&cache[j][1], key);

                                        LOGI("%s \n", &cache[j][1]);
                                    }
                                } else {
                                    if(i < 20) memcpy(cache[i], rbuf, len);
                                }
                            } else {

                                ParseLine(&rbuf[1], key);
                                LOGI("%s \n", &rbuf[1]);
                            }
                        }
                        if(alladdr != NULL) {
                            free(alladdr);
                            alladdr = NULL;
                        }
                        break;
                    }while(0);

                    fseek(fd, curpos, SEEK_SET);
                }
            }

            if(check_flag == 1) break;
            fseek(fd, size-8, SEEK_CUR);
        }
    }while(0);

    end:
    fclose(fd);
    fd = NULL;

    return m_infoList.size();
}

int AssistFile::GetNode(int idx, AssistInfo_t &node)
{
    int	pos =idx;//= m_infoList.FindIndex(idx);

    if(pos) {
        node = m_infoList[pos];
        return 0;
    } else {

        node.gps_lat = "0";
        node.gps_lgt = "0";
        node.gsensor_x = 0;
        node.gsensor_y = 0;
        node.gsensor_z = 0;
        node.spd       = 0;
        return -1;
    }
}

void AssistFile::ClearList()
{
    m_infoList.clear();
}
