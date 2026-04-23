package com.plantdoctor;

import android.util.Log;

public class NativeBridge {
    private static final String TAG = "NativeBridge";
    private static boolean isInitialized = false;

    static {
        try {
            System.loadLibrary("appPlantDoctor_armeabi-v7a");
            isInitialized = true;
            Log.d(TAG, "Library loaded successfully");
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Library load error", e);
        }
    }

    public static void invoked() {
        if (!isInitialized) {
            Log.e(TAG, "Native library not loaded");
            throw new IllegalStateException("Native library not loaded");
        }
        try {
            nativeInvoked();
            Log.d(TAG, "nativeInvoked called successfully");
        } catch (Exception e) {
            Log.e(TAG, "Failed to call nativeInvoked", e);
        }
    }

    private static native void nativeInvoked();
}