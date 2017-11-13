package pappu.com.facedetection.java_jni;

/**
 * Created by pappu on 11/7/17.
 */

public class Facedetection {
    public Facedetection(){
        initialize();
    }
    static {
        System.loadLibrary("facedetection");
    }
    private native void initialize();

    public native boolean deserialize(String openCVXMLPath);
    public native long[] getFaceRectangle();
    public native boolean faceDetect(byte[] imageBuf, long imageWidth, long imageHeight,
                                  int orientation,int coreNumber,boolean applyHistogram);
}
