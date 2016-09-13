package cn.net.xyan.jnipp;

import java.io.InputStream;

/**
 * Created by zarra on 16/9/12.
 */
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Created by zarra on 16/9/12.
 */
public class JNISample {
    public native void  read(InputStream in, OutputStream out);
}