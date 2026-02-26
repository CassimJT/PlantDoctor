package com.plantdoctor;

import android.util.Log;
import android.content.Context;
import androidx.work.Worker;
import androidx.work.WorkerParameters;
import java.util.concurrent.TimeUnit;


public class ScheduledWorker extends Worker {
    private static final String TAG = "QtWorkManager";
    public ScheduledWorker(Context context,WorkerParameters params){
        super(context,params);
    }

   @Override
    public Result doWork() {
        Log.d(TAG,"Executing periodic work");
        try{
            NativeBridge.invoked();
            return Result.success();
        }catch(Exception e){
            Log.d(TAG,"Worker, C++ Call Failed");
            return Result.failure();
        }

    }
}
