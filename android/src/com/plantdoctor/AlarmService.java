package com.plantdoctor;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Intent;
import android.os.Build;
import android.os.IBinder;
import androidx.annotation.Nullable;
import androidx.core.app.NotificationCompat;
import android.util.Log;

public class AlarmService extends Service {
    private static final String TAG = "AlarmService";
    private static boolean isRunning = false;
    private static final int NOTIFICATION_ID = 123;
    private static final String CHANNEL_ID = "alarm_service_channel";

    @Override
    public void onCreate() {
        super.onCreate();
        isRunning = true;
        startForegroundNotification();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(TAG, "AlarmService started");
        // Stop the service immediately after starting
        stopSelf();
        return START_NOT_STICKY;
    }

    private void startForegroundNotification() {
        createNotificationChannel();
        Notification notification = new NotificationCompat.Builder(this, CHANNEL_ID)
            .setContentTitle("PlantDoctor Alarm")
            .setContentText("Processing alarms in background")
            .setSmallIcon(android.R.drawable.ic_dialog_info)
            .setPriority(NotificationCompat.PRIORITY_LOW)
            .build();
        startForeground(NOTIFICATION_ID, notification);
    }

    private void createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel channel = new NotificationChannel(
                CHANNEL_ID,
                "Alarm Service",
                NotificationManager.IMPORTANCE_LOW
            );
            NotificationManager manager = getSystemService(NotificationManager.class);
            if (manager != null) {
                manager.createNotificationChannel(channel);
            }
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        isRunning = false;
        Log.d(TAG, "AlarmService destroyed");
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    public static boolean isServiceRunning() {
        return isRunning;
    }
}