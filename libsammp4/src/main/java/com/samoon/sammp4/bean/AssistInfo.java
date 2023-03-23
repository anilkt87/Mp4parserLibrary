package com.samoon.sammp4.bean;

public
class AssistInfo {

    private float	gsensor_x;
    private float	gsensor_y;
    private float	gsensor_z;
    private String gps_lat;
    private String gps_lgt;
    private int     spd;
    private double  north_angle;

    public AssistInfo(){}

    public AssistInfo(float gsensor_x, float gsensor_y, float gsensor_z, String gps_lat, String gps_lgt, int spd, double north_angle) {
        this.gsensor_x = gsensor_x;
        this.gsensor_y = gsensor_y;
        this.gsensor_z = gsensor_z;
        this.gps_lat = gps_lat;
        this.gps_lgt = gps_lgt;
        this.spd = spd;
        this.north_angle = north_angle;
    }



    public float getGsensor_x() {
        return gsensor_x;
    }

    public float getGsensor_y() {
        return gsensor_y;
    }

    public float getGsensor_z() {
        return gsensor_z;
    }

    public String getGps_lat() {
        return gps_lat;
    }

    public String getGps_lgt() {
        return gps_lgt;
    }

    public int getSpd() {
        return spd;
    }

    public double getNorth_angle() {
        return north_angle;
    }


}
