package pappu.com.facedetection.java_jni;

/**
 * Created by pappu on 11/7/17.
 */

public class Facedetection {
    static {
        System.loadLibrary("facedetection");
    }
    public native int getid();
}
