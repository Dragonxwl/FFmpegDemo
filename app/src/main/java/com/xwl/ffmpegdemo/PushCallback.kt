package com.xwl.ffmpegdemo

interface PushCallback {
    fun videoCallback(pts:Long , dts:Long,duration:Long,index:Long)
}