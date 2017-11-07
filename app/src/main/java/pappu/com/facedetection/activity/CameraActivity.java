package pappu.com.facedetection.activity;

import android.app.Activity;
import android.content.Context;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.Toast;

import java.util.List;

import pappu.com.facedetection.R;
import pappu.com.facedetection.datamodel.Orientation;
import pappu.com.facedetection.glview.GlSurfaceView;
import pappu.com.facedetection.renderer.ImageRenderer;


/**
 * Created by pappu on 12/5/16.
 */


public class CameraActivity extends Activity implements SensorEventListener, Camera.PreviewCallback,View.OnClickListener{

    private Camera mCamera = null;
    private final static int mCameraId = Camera.CameraInfo.CAMERA_FACING_FRONT;
    private FrameLayout previewLayout;
    private SensorManager senSensorManager;
    private Sensor senAccelerometer;

    int mFrameHeight,mFrameWidth;
    public Orientation orientation = Orientation.PotraitUp;
    SurfaceTexture surfaceTexture;

    private  boolean startVideo = true;
    private Button imageCaptureButton,videoCaptureButton;
    private GlSurfaceView glSurfaceView;
    private ImageRenderer imageRenderer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.camera_activity);
        senSensorManager = (SensorManager) this.getSystemService(
                Context.SENSOR_SERVICE);
        senAccelerometer = senSensorManager
                .getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        senSensorManager.registerListener(this, senAccelerometer,
                SensorManager.SENSOR_DELAY_NORMAL);

        previewLayout = (FrameLayout)findViewById(R.id.preview);
        imageCaptureButton = (Button)findViewById(R.id.capture_image);
        videoCaptureButton = (Button)findViewById(R.id.capture_video);
        imageCaptureButton.setOnClickListener(this);
        videoCaptureButton.setOnClickListener(this);
        setCamera();
        glSurfaceView = new GlSurfaceView(this);
        imageRenderer = new ImageRenderer(this,mFrameWidth,mFrameHeight);
        glSurfaceView.setRenderer(imageRenderer);
        glSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        previewLayout.addView(glSurfaceView);


    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.capture_image:
                Toast.makeText(this,"Saved Image Successfully", Toast.LENGTH_SHORT).show();

                break;
            case R.id.capture_video:
                if(startVideo){
                    videoCaptureButton.setText("StopCapture");
                    startVideo = false;
                }
                else {
                    videoCaptureButton.setText("VideoCapture");
                    startVideo = true;
                }
                break;
        }
    }



    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        imageRenderer.updateYUVBuffers(data);
        glSurfaceView.requestRender();
    }




    void setCamera(){

        mCamera = getCameraInstance();
        mCamera.setDisplayOrientation(90);
        Camera.Parameters params = mCamera.getParameters();
        params.setPreviewFormat(ImageFormat.NV21);
        Camera.Size previewSize = params.getPreviewSize() ;

        List<Camera.Size> prevsizeArray =  params.getSupportedPreviewSizes();

        for(int i=0;i<prevsizeArray.size();i++){

            int width = prevsizeArray.get(i).width,height = prevsizeArray.get(i).height;
            float ratio = (float)width/height;
            if(ratio==(float)16/9 && height<=720){
                previewSize.width = width;
                previewSize.height = height;
                break;
            }

        }
        mFrameHeight = previewSize.height;
        mFrameWidth = previewSize.width;
        params.setPreviewSize(mFrameWidth,mFrameHeight);

        mCamera.setParameters(params);
        try {
            surfaceTexture = new SurfaceTexture(0);
            mCamera.setPreviewTexture(surfaceTexture);
            mCamera.setPreviewCallback(this);
            mCamera.startPreview();

        } catch (Exception error ) {
            error.printStackTrace();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        senSensorManager.registerListener(this, senAccelerometer,
                SensorManager.SENSOR_DELAY_NORMAL);
    }

    @Override
    public void onSensorChanged(SensorEvent sensorEvent) {
        Sensor mySensor = sensorEvent.sensor;

        if (mySensor.getType() == Sensor.TYPE_ACCELEROMETER) {

            if ((sensorEvent.values[0] < 3 && sensorEvent.values[0] > -3)
                    && (sensorEvent.values[1] < 3 && sensorEvent.values[1] > -3)) {
               orientation = Orientation.Flat;

            } else {
                if (Math.abs(sensorEvent.values[1]) > Math
                        .abs(sensorEvent.values[0])) {

                    if (sensorEvent.values[1] < 0) {
                       orientation = Orientation.PotraitDown;

                    } else {
                        orientation = Orientation.PotraitUp;
                    }

                } else {
                    if (sensorEvent.values[0] < 0) {
                        orientation = Orientation.LandscapeDown;

                    } else {
                        orientation = Orientation.LandscapeUp;

                    }

                }
            }
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

    }

    public static Camera getCameraInstance() {
        Camera camera = null;
        try {
            camera = Camera.open(mCameraId);
        } catch (Exception error) {
            error.printStackTrace();
            Log.e("mainactivity", "[ERROR] Camera open failed." + error.getMessage());
        }

        return camera;
    }

    @Override
    protected void onPause() {
        super.onPause();
        senSensorManager.unregisterListener(this);
        videoCaptureButton.setText("VideoCapture");
        startVideo = true;
    }

    @Override
    protected void onDestroy() {
        senSensorManager.unregisterListener(this);
        releaseCamera();
        super.onDestroy();
    }

    public void releaseCamera() {
        if (mCamera != null) {
            try{
                mCamera.setPreviewCallback(null);
                mCamera.stopPreview();
                surfaceTexture.release();
                mCamera.release();
                mCamera = null;
            }catch (Exception e){
                e.printStackTrace();
            }


        }
    }

}
