package com.plantdoctor;

import android.util.Log;
public class NativeBridge {
    private static boolean isInitialized = false;

    static {
        try {
            System.loadLibrary("appSalesMate_armeabi-v7a");
            isInitialized = true;
            Log.d("NativeBridge", "Library loaded successfully");
        } catch (UnsatisfiedLinkError e) {
            Log.e("NativeBridge", "Library load error", e);
        }
    }

    public static void invoked() {
        if (!isInitialized) {
            Log.e("NativeBridge", "Native library not loaded");
            throw new IllegalStateException("Native library not loaded");
        }
        try {
            nativeInvoked();
            Log.d("NativeBridge", "nativeInvoked called successfully");
        } catch (Exception e) {
            Log.e("NativeBridge", "Failed to call nativeInvoked", e);
        }
    }

    private static native void nativeInvoked();
}
