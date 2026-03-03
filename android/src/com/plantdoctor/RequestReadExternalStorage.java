package com.plantdoctor;

import android.app.Activity;
import android.content.pm.PackageManager;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import android.Manifest;
import android.util.Log;

public class RequestReadExternalStorage {

    private static final int REQUEST_CODE_STORAGE = 1001;

    public static boolean hasStoragePermission(Activity activity) {
        return ContextCompat.checkSelfPermission(activity,
                Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED;
    }

    public static void requestStoragePermission(Activity activity) {
        if (!hasStoragePermission(activity)) {
            ActivityCompat.requestPermissions(activity,
                    new String[]{Manifest.permission.READ_EXTERNAL_STORAGE},
                    REQUEST_CODE_STORAGE);
        } else {
            Log.d("RequestStorage", "Permission already granted");
        }
    }

    // Optional: handle the result in Qt using a signal via JNI callback
}
