package cn.net.xyan.javacpp;

import org.bytedeco.javacpp.BytePointer;
import org.bytedeco.javacpp.FunctionPointer;
import org.bytedeco.javacpp.Loader;
import org.bytedeco.javacpp.Pointer;
import org.bytedeco.javacpp.annotation.Cast;
import org.bytedeco.javacpp.annotation.Const;
import org.bytedeco.javacpp.annotation.Name;
import org.bytedeco.javacpp.annotation.Platform;

/**
 * Created by zarra on 16/9/13.
 */
@Platform(include={"<libjpeg/16/jpeglib16.h>"},link = "jpeg16")
public class JPEG16 {

    static { Loader.load(); }

    @Name("jpeg_source_mgr")
    public static class SourceMgr extends Pointer{
        static { Loader.load(); }

//         const JOCTET * next_input_byte; /* => next byte to read from buffer */
//        size_t bytes_in_buffer;  /* # of bytes remaining in buffer */
//
//        JMETHOD(void, init_source, (j_decompress_ptr cinfo));
//        JMETHOD(boolean, fill_input_buffer, (j_decompress_ptr cinfo));
//        JMETHOD(void, skip_input_data, (j_decompress_ptr cinfo, long num_bytes));
//        JMETHOD(boolean, resync_to_restart, (j_decompress_ptr cinfo, int desired));
//        JMETHOD(void, term_source, (j_decompress_ptr cinfo));

        /** Default native constructor. */
        public SourceMgr() { super((Pointer)null); allocate(); }

        /** Native array allocator. Access with {@link Pointer#position(long)}. */
        public SourceMgr(long size) { super((Pointer)null); allocateArray(size); }

        /** Pointer cast constructor. Invokes {@link Pointer#Pointer(Pointer)}. */
        public SourceMgr(Pointer p) { super(p); }

        private native void allocate();

        private native void allocateArray(long size);

        @Override public SourceMgr position(long position) {
            return (SourceMgr)super.position(position);
        }

        public native @Cast("size_t")long bytes_in_buffer();public native SourceMgr bytes_in_buffer(long bytes_in_buffer);

        public native @Cast("JOCTET *")BytePointer next_input_byte();public native SourceMgr next_input_byte(BytePointer next_input_byte);

        public static class InitSource extends FunctionPointer {
            static { Loader.load(); }
            /** Pointer cast constructor. Invokes {@link Pointer#Pointer(Pointer)}. */
            public InitSource(Pointer p) { super(p); }
            protected InitSource() { allocate(); }
            private native void allocate();
            public native void call(JpegDecompress jpegDecompress);
        }
        public native InitSource init_source();public native SourceMgr init_source(InitSource init_source);

        public static class FillInputBuffer extends FunctionPointer {
            static { Loader.load(); }
            /** Pointer cast constructor. Invokes {@link Pointer#Pointer(Pointer)}. */
            public    FillInputBuffer(Pointer p) { super(p); }
            protected FillInputBuffer() { allocate(); }
            private native void allocate();
            public native @Cast("boolean") boolean call(JpegDecompress jpegDecompress);
        }
        public native FillInputBuffer fill_input_buffer();public native SourceMgr fill_input_buffer(FillInputBuffer fill_input_buffer);


        public static class SkipInputData extends FunctionPointer {
            static { Loader.load(); }
            /** Pointer cast constructor. Invokes {@link Pointer#Pointer(Pointer)}. */
            public    SkipInputData(Pointer p) { super(p); }
            protected SkipInputData() { allocate(); }
            private native void allocate();
            public native void call(JpegDecompress jpegDecompress,@Cast("long") long num_bytes);
        }
        public native SkipInputData skip_input_data();public native SourceMgr skip_input_data(SkipInputData skip_input_data);

        public static class ResyncToRestart extends FunctionPointer {
            static { Loader.load(); }
            /** Pointer cast constructor. Invokes {@link Pointer#Pointer(Pointer)}. */
            public    ResyncToRestart(Pointer p) { super(p); }
            protected ResyncToRestart() { allocate(); }
            private native void allocate();
            public native  @Cast("boolean") boolean call(JpegDecompress jpegDecompress,int desired);
        }
        public native ResyncToRestart resync_to_restart();public native SourceMgr resync_to_restart(ResyncToRestart resync_to_restart);

        public static class TermSource extends FunctionPointer {
            static { Loader.load(); }
            /** Pointer cast constructor. Invokes {@link Pointer#Pointer(Pointer)}. */
            public    TermSource(Pointer p) { super(p); }
            protected TermSource() { allocate(); }
            private native void allocate();
            public native void call(JpegDecompress jpegDecompress);
        }
        public native TermSource term_source();public native SourceMgr term_source(TermSource term_source);

    }

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

        public native SourceMgr src();public native JpegDecompress src(SourceMgr src);

        public native @Cast("JDIMENSION") int image_width();public native JpegDecompress image_width(int width);

    }


    public static native void jpeg_create_decompress(JpegDecompress jpegDecompress);
}
