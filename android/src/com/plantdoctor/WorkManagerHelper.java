package com.plantdoctor;

import android.util.Log;
import androidx.work.WorkManager;
import androidx.work.OneTimeWorkRequest;
import android.content.Context;
import androidx.work.PeriodicWorkRequest;
import java.util.concurrent.TimeUnit;
import androidx.work.ExistingPeriodicWorkPolicy;

public class WorkManagerHelper {
    public static void scheduleWork(Context context) {
        OneTimeWorkRequest workRequest = new OneTimeWorkRequest.Builder(ScheduledWorker.class).build();
        WorkManager.getInstance(context).enqueue(workRequest);
    }
    
    //for periodic tasks
    public static void schedulePeriodicWork(Context context) {
        PeriodicWorkRequest periodicWork = new PeriodicWorkRequest.Builder(
            ScheduledWorker.class, //the class containing the doWork function
            15,//time interval
            TimeUnit.MINUTES
        ).setInitialDelay(
            1,
            TimeUnit.MINUTES)
            .build();

        // Use CANCEL_AND_REENQUEUE instead of deprecated REPLACE
        WorkManager.getInstance(context).enqueueUniquePeriodicWork(
            "qt_periodic_work",
            ExistingPeriodicWorkPolicy.CANCEL_AND_REENQUEUE, // Replaces the existing work
            periodicWork
        );
    }
    
    //cancel the work
    public static void cancelWork(Context context) {
        WorkManager.getInstance(context).cancelUniqueWork("qt_periodic_work");
    }
}
