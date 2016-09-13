package cn.net.xyan.javacpp;

/**
 * Created by zarra on 16/9/14.
 */
public class Jpeg16Test {

   @org.junit.Test
    public void test1(){
       JPEG16.JpegDecompress jpegDecompress = new JPEG16.JpegDecompress();

       JPEG16.jpeg_create_decompress(jpegDecompress);
    }
}
