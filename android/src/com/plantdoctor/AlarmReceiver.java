package com.plantdoctor;

import android.app.ActivityManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.PowerManager;
import android.util.Log;
import android.os.Handler;
import android.os.Looper;
import androidx.core.app.NotificationCompat;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.NotificationChannel;
import android.app.PendingIntent;
import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;

public class AlarmReceiver extends BroadcastReceiver {
    private static final String TAG = "AlarmReceiver";

    @Override
    public void onReceive(Context context, Intent intent) {
        if ("android.intent.action.BOOT_COMPLETED".equals(intent.getAction())) {
            Log.d(TAG, "Device rebooted - rescheduling alarm");
            AlarmHelper.scheduleExactAlarm(context);
            return;
        }

        if (!"com.plantdoctor.ALARM_TRIGGERED".equals(intent.getAction())) {
            Log.w(TAG, "Received unexpected intent: " + intent.getAction());
            return;
        }

        if (isServiceRunning(context)) {
            Log.d(TAG, "Service already running - skipping new instance");
            return;
        }

        boolean isHuawei = Build.MANUFACTURER.equalsIgnoreCase("huawei");
        PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
        if (pm == null) {
            Log.e(TAG, "PowerManager unavailable");
            return;
        }

        PowerManager.WakeLock wl = pm.newWakeLock(
            PowerManager.PARTIAL_WAKE_LOCK,
            "plantdoctor:alarm_wakelock"
        );

        try {
            wl.acquire(10 * 60 * 1000L /*10 minutes*/);
            processAlarm(context, isHuawei);
        } finally {
            if (wl != null && wl.isHeld()) {
                wl.release();
            }
        }
    }

    private void processAlarm(Context context, boolean isHuawei) {
        // Only start service for Huawei if app is not active
        if (isHuawei && !isQtReady(context)) {
            Intent serviceIntent = new Intent(context, AlarmService.class);
            try {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                    context.startForegroundService(serviceIntent);
                } else {
                    context.startService(serviceIntent);
                }
                Log.d(TAG, "Service started for Huawei device");
            } catch (Exception e) {
                Log.e(TAG, "Failed to start service", e);
            }
        }

        // Execute alarm logic
        executeAlarmLogic(context);
    }

    private void executeAlarmLogic(Context context) {
        try {
            Log.d(TAG, "Executing alarm logic");

            // Always reschedule the next alarm
            AlarmHelper.scheduleExactAlarm(context);

            // Try immediate execution
            if (isQtReady(context)) {
                NativeBridge.invoked();
                Log.d(TAG, "Alarm triggered successfully");
                return;
            }

            // App not active - launch activity
            Log.w(TAG, "Qt not ready - launching app");
            Intent launchIntent = context.getPackageManager().getLaunchIntentForPackage("com.plantdoctor");
            if (launchIntent == null) {
                Log.e(TAG, "Failed to get launch intent for com.plantdoctor");
                showOpenAppNotification(context); //ask the user to manually launch it
                return;
            }
            launchIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_REORDER_TO_FRONT);
            try {
                context.startActivity(launchIntent);
                Log.d(TAG, "Launch intent started for QtActivity");
            } catch (Exception e) {
                Log.e(TAG, "Failed to start activity", e);
                showOpenAppNotification(context);
                return;
            }

            // Retry up to 3 times with increasing delays
            int maxRetries = 3;
            long initialDelay = 5000; // 5 seconds
            Handler handler = new Handler(Looper.getMainLooper());
            AtomicInteger retryCount = new AtomicInteger(0);

            Runnable retryRunnable = new Runnable() {
                @Override
                public void run() {
                    if (isQtReady(context)) {
                        NativeBridge.invoked();
                        Log.d(TAG, "Delayed alarm triggered successfully");
                    } else if (retryCount.incrementAndGet() < maxRetries) {
                        long nextDelay = initialDelay * (retryCount.get() + 1);
                        Log.w(TAG, "Qt not ready - retrying after " + nextDelay + "ms");
                        handler.postDelayed(this, nextDelay);
                    } else {
                        Log.e(TAG, "Qt still not ready after " + maxRetries + " retries");
                        showOpenAppNotification(context);
                    }
                }
            };
            handler.postDelayed(retryRunnable, initialDelay);
        } catch (Exception e) {
            Log.e(TAG, "Alarm processing failed", e);
            showOpenAppNotification(context);
        }
    }

    private boolean isQtReady(Context context) {
        try {
            // Check if the app is in the foreground
            ActivityManager am = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
            if (am == null) return false;

            List<ActivityManager.RunningAppProcessInfo> processes = am.getRunningAppProcesses();
            if (processes != null) {
                for (ActivityManager.RunningAppProcessInfo process : processes) {
                    if (process.processName.equals(context.getPackageName()) &&
                        process.importance == ActivityManager.RunningAppProcessInfo.IMPORTANCE_FOREGROUND) {
                        Log.d(TAG, "App is in foreground, assuming Qt is ready");
                        return true;
                    }
                }
            }
            Log.d(TAG, "App not in foreground");
            return false;
        } catch (Exception e) {
            Log.e(TAG, "Error checking Qt readiness", e);
            return false;
        }
    }

    private void showOpenAppNotification(Context context) {
        NotificationManager nm = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel channel = new NotificationChannel(
                "alarm_notification_channel",
                "Alarm Notifications",
                NotificationManager.IMPORTANCE_HIGH
            );
            nm.createNotificationChannel(channel);
        }

        Intent notificationIntent = context.getPackageManager().getLaunchIntentForPackage("com.plantdoctor");
        if (notificationIntent != null) {
            notificationIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TOP);
            PendingIntent pendingIntent = PendingIntent.getActivity(context, 0, notificationIntent, 
                PendingIntent.FLAG_UPDATE_CURRENT | (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M ? PendingIntent.FLAG_IMMUTABLE : 0));

            // Use Android system icon instead of custom one
            int iconId = android.R.drawable.ic_dialog_info;
            
            Notification notification = new NotificationCompat.Builder(context, "alarm_notification_channel")
                .setContentTitle("PlantDoctor Alarm")
                .setContentText("Please open the app to process the alarm")
                .setSmallIcon(iconId)
                .setPriority(NotificationCompat.PRIORITY_HIGH)
                .setContentIntent(pendingIntent)
                .setAutoCancel(true)
                .build();
            nm.notify(0, notification);
            Log.d(TAG, "Showed notification to open app");
        } else {
            Log.e(TAG, "Failed to create notification intent");
        }
    }

    @SuppressWarnings("deprecation")
    private boolean isServiceRunning(Context context) {
        // Fast in-process check
        if (AlarmService.isServiceRunning()) {
            return true;
        }

        // Robust system-level check
        ActivityManager manager = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        if (manager == null) return false;

        // Suppress deprecation warning - this is still the only way on older APIs
        List<ActivityManager.RunningServiceInfo> services = manager.getRunningServices(Integer.MAX_VALUE);
        for (ActivityManager.RunningServiceInfo service : services) {
            if (AlarmService.class.getName().equals(service.service.getClassName())) {
                return true;
            }
        }
        return false;
    }
}
