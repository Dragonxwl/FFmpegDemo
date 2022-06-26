package com.xwl.ffmpegdemo

import android.Manifest
import android.content.Intent
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_main.*
import pub.devrel.easypermissions.EasyPermissions


class MainActivity : AppCompatActivity() {
    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        val perms = arrayOf(
            Manifest.permission.READ_CALENDAR,
            Manifest.permission.WRITE_CALENDAR,
            Manifest.permission.READ_EXTERNAL_STORAGE
        )
        if (!EasyPermissions.hasPermissions(this, *perms)) {
            EasyPermissions.requestPermissions(this, "写权限", 10086, *perms)
        }
        btn_protocol.setOnClickListener {
            tv_info!!.text = urlprotocolinfo()
        }
        btn_codec.setOnClickListener {
            tv_info!!.text = avcodecinfo()
        }
        btn_filter.setOnClickListener {
            tv_info!!.text = avfilterinfo()
        }
        btn_format.setOnClickListener {
            tv_info!!.text = avformatinfo()
        }

        btn_set_callback.setOnClickListener {
            setCallback(object : PushCallback {
                override fun videoCallback(pts: Long, dts: Long, duration: Long, index: Long) {
                    val sb = StringBuilder()
                    sb.append("pts: ").append(pts).append("\n")
                    sb.append("dts: ").append(dts).append("\n")
                    sb.append("duration: ").append(duration).append("\n")
                    sb.append("index: ").append(index).append("\n")
                    tv_info!!.text = sb.toString()
                }

            })
        }
        btn_get_file.setOnClickListener {
            chooseFile()
        }
        btn_push.setOnClickListener {
            object : Thread() {
                override fun run() {
                    super.run()
                    tv_info!!.text = "result ${pushRtspFile(savePath)}"
                }
            }.start()
        }
    }

    private val CHOOSE_FILE = 10
    /**
     * 选择文件
     */
    private fun chooseFile() {
        val intent = Intent()
        intent.type = "video/*"
        intent.action = Intent.ACTION_GET_CONTENT
        intent.addCategory(Intent.CATEGORY_OPENABLE)
        startActivityForResult(intent, CHOOSE_FILE)
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        when (requestCode) {
            CHOOSE_FILE -> if (resultCode == RESULT_OK) {
                savePath = UriUtils.getPath(this@MainActivity, data?.data)
                tv_info!!.text = savePath
            }
        }
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        EasyPermissions.onRequestPermissionsResult(requestCode,permissions,grantResults,this)
    }

    var savePath = ""

    external fun stringFromJNI(): String?
    private external fun urlprotocolinfo(): String?
    private external fun avformatinfo(): String?
    private external fun avcodecinfo(): String?
    private external fun avfilterinfo(): String?
    private external fun pushRtspFile(path:String): Int
    private external fun setCallback(pushCallback:PushCallback): Int
}