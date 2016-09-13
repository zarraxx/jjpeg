package cn.net.xyan.javacpp;

import org.bytedeco.javacpp.Loader;
import org.bytedeco.javacpp.Pointer;
import org.bytedeco.javacpp.annotation.Name;
import org.bytedeco.javacpp.annotation.Platform;

/**
 * Created by zarra on 16/9/13.
 */
@Platform(include={"<libjpeg/16/jpeglib16.h>"},link = "jpeg16")
public class JPEG16 {

    static { Loader.load(); }

    @Name("jpeg_decompress_struct")
    public static class JpegDecompress extends Pointer{

        static { Loader.load(); }

        /** Default native constructor. */
        public JpegDecompress() { super((Pointer)null); allocate(); }

        /** Native array allocator. Access with {@link Pointer#position(long)}. */
        public JpegDecompress(long size) { super((Pointer)null); allocateArray(size); }

        /** Pointer cast constructor. Invokes {@link Pointer#Pointer(Pointer)}. */
        public JpegDecompress(Pointer p) { super(p); }

        private native void allocate();

        private native void allocateArray(long size);

        @Override public JpegDecompress position(long position) {
            return (JpegDecompress)super.position(position);
        }

    }


    public static native void jpeg_create_decompress(JpegDecompress jpegDecompress);
}
