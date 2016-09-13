package cn.net.xyan.jnipp;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;

/**
 * Created by zarra on 16/9/12.
 */
public class JNITest {
    public static class ITest extends InputStream{

        @Override
        public int read() throws IOException {
            return 0;
        }
    }

    public static void main(String[] args){
        System.loadLibrary("jnipp");
        ByteArrayInputStream in = new ByteArrayInputStream(new byte[]{1,2,3,4,5,6,7,8,9,10});
        JNISample jniSample = new JNISample();
        jniSample.read(in,null);
    }
}