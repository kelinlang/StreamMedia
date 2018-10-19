package com.stream.media.demo.utils;


import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;

import java.util.ArrayList;
import java.util.List;

/**
 * author: ben .
 * date: 2017/04/11 .
 * desc: 6.0权限相关类
 */

public class CheckPermissionUtils {
    private static final String TAG = CheckPermissionUtils.class.getSimpleName();

    /**
     * check this permission is need to request
     *
     * @param context         context
     * @param permissionArray permissionArray
     * @return
     */
    public static String[] getNeededPermission(Context context, String[] permissionArray) {
        if (context == null || permissionArray == null || permissionArray.length == 0) {
            return new String[]{};
        }

        List<String> permissionList = new ArrayList<>();
        for (int i = 0; i < permissionArray.length; i++) {
            if (CheckPermissionUtils.isNeedAddPermission(context, permissionArray[i])) {
                permissionList.add(permissionArray[i]);
            }
        }
        return permissionList.toArray(new String[permissionList.size()]);
    }

    /**
     * check permission is need ?
     *
     * @return true: need permission  false: don't need permission
     */
    public static boolean isNeedAddPermission(Context context, String permission) {
        return ContextCompat.checkSelfPermission(context, permission) != PackageManager.PERMISSION_GRANTED;
    }

    /**
     * check permission is need and request
     *
     * @param context
     * @param permission
     * @param requestCode
     * @return true: need permission  false: don't need permission
     */
    @RequiresApi(api = Build.VERSION_CODES.M)
    public static boolean isAddPermission(Activity context, String permission, int requestCode) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN && ActivityCompat.checkSelfPermission(context, permission)
                != PackageManager.PERMISSION_GRANTED) {
            if (context.shouldShowRequestPermissionRationale(Manifest.permission.READ_CONTACTS)) {
            }
            ActivityCompat.requestPermissions(context, new String[]{permission}, requestCode);
            return false;
        } else {
            return true;
        }
    }
}
