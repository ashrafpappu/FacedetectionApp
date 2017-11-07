package pappu.com.facedetection.renderer;

import android.content.Context;
import android.opengl.GLES20;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

import pappu.com.facedetection.R;
import pappu.com.facedetection.helper.GLHelper;
import pappu.com.facedetection.helper.GLRenderHelper;


/**
 * Created by pappu on 8/7/17.
 */

public class OnscreenTextureDraw {
    private int shaderProgram;
    short[] indices = new short[] {0,1,2,0,2,3};
    private FloatBuffer onScreenTexCoordBuffer, onScreenVertexBuffer;
    private ShortBuffer onScreenIndexBuffer;
    private int textureLocation;
    private GLRenderHelper renderHelper;
    private float[] vertices = new float[] {
            -1.f, -1.f,
            -1.f, 1.f,
            1.f, 1.f,
            1.f, -1.f
    };
    private float[] texCoords = new float[] {
            0.f, 0.f,
            0.f, 1.f,
            1.f, 1.f,
            1.f, 0.f
    };

    private void setBuffer(){

        ByteBuffer tcbb = ByteBuffer.allocateDirect(texCoords.length * 4);
        tcbb.order(ByteOrder.nativeOrder());
        onScreenTexCoordBuffer = tcbb.asFloatBuffer();
        onScreenTexCoordBuffer.put(texCoords);
        onScreenTexCoordBuffer.position(0);


        ByteBuffer vbb = ByteBuffer.allocateDirect(vertices.length * 4);
        vbb.order(ByteOrder.nativeOrder());
        onScreenVertexBuffer = vbb.asFloatBuffer();
        onScreenVertexBuffer.put(vertices);
        onScreenVertexBuffer.position(0);


        ByteBuffer ibb = ByteBuffer.allocateDirect(indices.length * 2);
        ibb.order(ByteOrder.nativeOrder());
        onScreenIndexBuffer = ibb.asShortBuffer();
        onScreenIndexBuffer.put(indices);
        onScreenIndexBuffer.position(0);
    }

    public void initOnScreenShaderProgramm(Context context){
        setBuffer();
        shaderProgram = GLHelper.getShaderProgramm(context, R.raw.onscreen_vertex_shader, R.raw.onscreen_fragment_shader);
        renderHelper = new GLRenderHelper(shaderProgram,null);
        textureLocation = GLES20.glGetUniformLocation(shaderProgram, "u_texture");
    }

    public void onScreenRender(int texture){
        GLES20.glUseProgram(shaderProgram);
        renderHelper.updateVertexAtrrib(onScreenVertexBuffer,2);
        renderHelper.updateTexCoordAtrrib(onScreenTexCoordBuffer,2);
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0 + 1);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texture);
        GLES20.glUniform1i(textureLocation, 1);
        GLES20.glDrawElements(GLES20.GL_TRIANGLES, indices.length,
                GLES20.GL_UNSIGNED_SHORT, onScreenIndexBuffer);
        renderHelper.disableVertexAttrib();
        renderHelper.disableTexCoordAttrib();
    }

}
