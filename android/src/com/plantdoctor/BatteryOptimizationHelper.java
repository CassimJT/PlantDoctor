package com.plantdoctor;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Build;
import android.os.PowerManager;
import android.provider.Settings;
import android.util.Log;

public class BatteryOptimizationHelper {
    private static final String TAG = "BatteryOptimizationHelper";
    private static final String PREFS_NAME = "SalesMatePrefs";
    private static final String KEY_BATTERY_PROMPT_SHOWN = "battery_prompt_shown";
    private static final String KEY_HUAWEI_AUTOSTART_PROMPT_SHOWN = "huawei_autostart_prompt_shown";

    public static void requestIgnoreBatteryOptimization(Context context) {
        try {
            SharedPreferences prefs = context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE);
            SharedPreferences.Editor editor = prefs.edit();

            // Standard Android battery optimization exemption
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
                String packageName = context.getPackageName();

                if (!pm.isIgnoringBatteryOptimizations(packageName)) {
                    // Only prompt if not previously shown
                    if (!prefs.getBoolean(KEY_BATTERY_PROMPT_SHOWN, false)) {
                        Intent intent = new Intent(Settings.ACTION_REQUEST_IGNORE_BATTERY_OPTIMIZATIONS);
                        intent.setData(Uri.parse("package:" + packageName));
                        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                        context.startActivity(intent);
                        editor.putBoolean(KEY_BATTERY_PROMPT_SHOWN, true);
                        editor.apply();
                        Log.d(TAG, "Prompted user for battery optimization exemption");
                    } else {
                        Log.d(TAG, "Battery optimization prompt already shown");
                    }
                } else {
                    Log.d(TAG, "Battery optimization already exempted");
                    // Reset prompt flag if permission is granted
                    if (prefs.getBoolean(KEY_BATTERY_PROMPT_SHOWN, false)) {
                        editor.putBoolean(KEY_BATTERY_PROMPT_SHOWN, false);
                        editor.apply();
                    }
                }
            }

            // Huawei-specific Auto-start prompt
            if (Build.MANUFACTURER.equalsIgnoreCase("huawei")) {
                // No direct way to check Auto-start status, so rely on prompt flag
                if (!prefs.getBoolean(KEY_HUAWEI_AUTOSTART_PROMPT_SHOWN, false)) {
                    try {
                        Intent intent = new Intent();
                        intent.setClassName("com.huawei.systemmanager",
                                          "com.huawei.systemmanager.startupmgr.ui.StartupNormalAppListActivity");
                        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                        context.startActivity(intent);
                        editor.putBoolean(KEY_HUAWEI_AUTOSTART_PROMPT_SHOWN, true);
                        editor.apply();
                        Log.d(TAG, "Prompted user for Huawei Auto-start settings");
                    } catch (Exception e) {
                        Log.e(TAG, "Failed to open Huawei Auto-start settings", e);
                        // Fallback: Open general app settings
                        Intent fallbackIntent = new Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS);
                        fallbackIntent.setData(Uri.parse("package:" + context.getPackageName()));
                        fallbackIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                        context.startActivity(fallbackIntent);
                        editor.putBoolean(KEY_HUAWEI_AUTOSTART_PROMPT_SHOWN, true);
                        editor.apply();
                        Log.d(TAG, "Opened general app settings as fallback");
                    }
                } else {
                    Log.d(TAG, "Huawei Auto-start prompt already shown");
                }
            }
        } catch (Exception e) {
            Log.e(TAG, "Error requesting battery optimization exemption", e);
        }
    }
}
