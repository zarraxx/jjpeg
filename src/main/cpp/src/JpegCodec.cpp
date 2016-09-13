//
// Created by 夏申频 on 16/9/11.
//
#include <limits.h>
#include <cassert>
#include <csetjmp>
#include <stdio.h>
extern "C" {
#include <libjpeg/16/jinclude.h>
#include <libjpeg/16/jpeglib.h>
#include <libjpeg/16/jerror.h>
}
#include "JpegCodec.h"

//#define SIZEOF sizeof

struct my_error_mgr {
    struct jpeg_error_mgr pub; /* "public" fields */
    jmp_buf setjmp_buffer;     /* for return to caller */
};
typedef struct my_error_mgr *my_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */
extern "C" {
METHODDEF(void) my_error_exit(j_common_ptr cinfo) {
    /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
    my_error_ptr myerr = (my_error_ptr) cinfo->err;

    /* Always display the message. */
    /* We could postpone this until after returning, if we chose. */
    (*cinfo->err->output_message)(cinfo);

    /* Return control to the setjmp point */
    longjmp(myerr->setjmp_buffer, 1);
}
}

/* Expanded data source object for stdio input */

typedef struct {
    struct jpeg_source_mgr pub;  /* public fields */

    std::istream *infile;    /* source stream */
    JOCTET *buffer;    /* start of buffer */
    boolean start_of_file;  /* have we gotten any data yet? */
} my_source_mgr;

typedef my_source_mgr *my_src_ptr;

#define INPUT_BUF_SIZE  4096  /* choose an efficiently fread'able size */


METHODDEF(void)
init_source(j_decompress_ptr cinfo) {
    my_src_ptr src = (my_src_ptr) cinfo->src;

    /* We reset the empty-input-file flag for each image,
     * but we don't clear the input buffer.
     * This is correct behavior for reading a series of images from one source.
     */
    src->start_of_file = TRUE;
}


/*
 * Fill the input buffer --- called whenever buffer is emptied.
 *
 * In typical applications, this should read fresh data into the buffer
 * (ignoring the current state of next_input_byte & bytes_in_buffer),
 * reset the pointer & count to the start of the buffer, and return TRUE
 * indicating that the buffer has been reloaded.  It is not necessary to
 * fill the buffer entirely, only to obtain at least one more byte.
 *
 * There is no such thing as an EOF return.  If the end of the file has been
 * reached, the routine has a choice of ERREXIT() or inserting fake data into
 * the buffer.  In most cases, generating a warning message and inserting a
 * fake EOI marker is the best course of action --- this will allow the
 * decompressor to output however much of the image is there.  However,
 * the resulting error message is misleading if the real problem is an empty
 * input file, so we handle that case specially.
 *
 * In applications that need to be able to suspend compression due to input
 * not being available yet, a FALSE return indicates that no more data can be
 * obtained right now, but more may be forthcoming later.  In this situation,
 * the decompressor will return to its caller (with an indication of the
 * number of scanlines it has read, if any).  The application should resume
 * decompression after it has loaded more data into the input buffer.  Note
 * that there are substantial restrictions on the use of suspension --- see
 * the documentation.
 *
 * When suspending, the decompressor will back up to a convenient restart point
 * (typically the start of the current MCU). next_input_byte & bytes_in_buffer
 * indicate where the restart point will be if the current call returns FALSE.
 * Data beyond this point must be rescanned after resumption, so move it to
 * the front of the buffer rather than discarding it.
 */

METHODDEF(boolean)
fill_input_buffer(j_decompress_ptr cinfo) {
    my_src_ptr src = (my_src_ptr) cinfo->src;
    size_t nbytes;

    //FIXME FIXME FIXME FIXME FIXME
    //nbytes = JFREAD(src->infile, src->buffer, INPUT_BUF_SIZE);
    std::streampos pos = src->infile->tellg();
    std::streampos end = src->infile->seekg(0, std::ios::end).tellg();
    src->infile->seekg(pos, std::ios::beg);
    //FIXME FIXME FIXME FIXME FIXME
    if (end == pos) {
        /* Start the I/O suspension simply by returning false here: */
        return FALSE;
    }
    if ((end - pos) < INPUT_BUF_SIZE) {
        src->infile->read((char *) src->buffer, (size_t) (end - pos));
    } else {
        src->infile->read((char *) src->buffer, INPUT_BUF_SIZE);
    }

    std::streamsize gcount = src->infile->gcount();
    assert(gcount < INT_MAX);
    nbytes = (size_t) gcount;

    if (nbytes <= 0) {
        if (src->start_of_file)  /* Treat empty input file as fatal error */
            ERREXIT(cinfo, JERR_INPUT_EMPTY);
        WARNMS(cinfo, JWRN_JPEG_EOF);
        /* Insert a fake EOI marker */
        src->buffer[0] = (JOCTET) 0xFF;
        src->buffer[1] = (JOCTET) JPEG_EOI;
        nbytes = 2;
    }

    src->pub.next_input_byte = src->buffer;
    src->pub.bytes_in_buffer = nbytes;
    src->start_of_file = FALSE;

    return TRUE;
}


/*
 * Skip data --- used to skip over a potentially large amount of
 * uninteresting data (such as an APPn marker).
 *
 * Writers of suspendable-input applications must note that skip_input_data
 * is not granted the right to give a suspension return.  If the skip extends
 * beyond the data currently in the buffer, the buffer can be marked empty so
 * that the next read will cause a fill_input_buffer call that can suspend.
 * Arranging for additional bytes to be discarded before reloading the input
 * buffer is the application writer's problem.
 */

METHODDEF(void)
skip_input_data(j_decompress_ptr cinfo, long num_bytes) {
    my_src_ptr src = (my_src_ptr) cinfo->src;

    /* Just a dumb implementation for now.  Could use fseek() except
     * it doesn't work on pipes.  Not clear that being smart is worth
     * any trouble anyway --- large skips are infrequent.
     */
    if (num_bytes > 0) {
        while (num_bytes > (long) src->pub.bytes_in_buffer) {
            num_bytes -= (long) src->pub.bytes_in_buffer;
            (void) fill_input_buffer(cinfo);
            /* note we assume that fill_input_buffer will never return FALSE,
             * so suspension need not be handled.
             */
        }
        src->pub.next_input_byte += (size_t) num_bytes;
        src->pub.bytes_in_buffer -= (size_t) num_bytes;
    }
}


/*
 * An additional method that can be provided by data source modules is the
 * resync_to_restart method for error recovery in the presence of RST markers.
 * For the moment, this source module just uses the default resync method
 * provided by the JPEG library.  That method assumes that no backtracking
 * is possible.
 */


/*
 * Terminate source --- called by jpeg_finish_decompress
 * after all data has been read.  Often a no-op.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */

METHODDEF(void)
term_source(j_decompress_ptr cinfo) {
    (void) cinfo;
    /* no work necessary here */
}


/*
 * Prepare for input from a stdio stream.
 * The caller must have already opened the stream, and is responsible
 * for closing it after finishing decompression.
 */

GLOBAL(void)
jpeg_stdio_src(j_decompress_ptr cinfo, std::istream &infile, bool flag) {
    my_src_ptr src;

    /* The source object and input buffer are made permanent so that a series
     * of JPEG images can be read from the same file by calling jpeg_stdio_src
     * only before the first one.  (If we discarded the buffer at the end of
     * one image, we'd likely lose the start of the next one.)
     * This makes it unsafe to use this manager and a different source
     * manager serially with the same JPEG object.  Caveat programmer.
     */
    if (cinfo->src == NULL) {  /* first time for this JPEG object? */
        cinfo->src = (struct jpeg_source_mgr *)
                (*cinfo->mem->alloc_small)((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                           SIZEOF(my_source_mgr));
        src = (my_src_ptr) cinfo->src;
        src->buffer = (JOCTET *)
                (*cinfo->mem->alloc_small)((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                           INPUT_BUF_SIZE * SIZEOF(JOCTET));
    }

    src = (my_src_ptr) cinfo->src;
    src->pub.init_source = init_source;
    src->pub.fill_input_buffer = fill_input_buffer;
    src->pub.skip_input_data = skip_input_data;
    src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
    src->pub.term_source = term_source;
    src->infile = &infile;
    if (flag) {
        src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
        src->pub.next_input_byte = NULL; /* until buffer loaded */
    }
}


/*
 * Note: see dcmdjpeg +cn option to avoid the YBR => RGB loss
 */
bool DecodeByStreams(std::istream &is, std::ostream &os) {


    jpeg_decompress_struct ccinfo;
    /* This struct contains the JPEG decompression parameters and pointers to
     * working space (which is allocated as needed by the JPEG library).
     */
    jpeg_decompress_struct &cinfo = ccinfo;

    /* We use our private extension JPEG error handler.
     * Note that this struct must live as long as the main JPEG parameter
     * struct, to avoid dangling-pointer problems.
     */
    my_error_mgr jjerr;
    my_error_mgr &jerr = jjerr;
    /* More stuff */
    //FILE * infile;    /* source file */
    JSAMPARRAY buffer;    /* Output row buffer */
    size_t row_stride;    /* physical row width in output buffer */

    //if( Internals->StateSuspension == 0 )
    // {
    // Step 1: allocate and initialize JPEG decompression object
    //
    // We set up the normal JPEG error routines, then override error_exit.
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    // Establish the setjmp return context for my_error_exit to use.
    if (setjmp(jerr.setjmp_buffer)) {
        // If we get here, the JPEG code has signaled an error.
        // We need to clean up the JPEG object, close the input file, and return.
        // But first handle the case IJG does not like:
        if (jerr.pub.msg_code == JERR_BAD_PRECISION /* 18 */ ) {
            //this->BitSample = jerr.pub.msg_parm.i[0];
            //assert( this->BitSample == 8 || this->BitSample == 12 || this->BitSample == 16 );
        }
        jpeg_destroy_decompress(&cinfo);
        // TODO: www.dcm4che.org/jira/secure/attachment/10185/ct-implicit-little.dcm
        // weird Icon Image from GE...
        return false;
    }
    // }

    //if( Internals->StateSuspension == 0 )
    // {
    // Now we can initialize the JPEG decompression object.
    jpeg_create_decompress(&cinfo);

    // Step 2: specify data source (eg, a file)
    jpeg_stdio_src(&cinfo, is, true);
    /*}
    else
    {
        jpeg_stdio_src(&cinfo, is, false);
    }*/

    /* Step 3: read file parameters with jpeg_read_header() */

    //if ( Internals->StateSuspension < 2 )
    // {
    if (jpeg_read_header(&cinfo, TRUE) == JPEG_SUSPENDED) {
        //Internals->StateSuspension = 2;
    }
    // First of all are we using the proper JPEG decoder (correct bit sample):
    if (jerr.pub.num_warnings) {
        // PHILIPS_Gyroscan-12-MONO2-Jpeg_Lossless.dcm
        if (jerr.pub.msg_code == JWRN_MUST_DOWNSCALE) {
            // PHILIPS_Gyroscan-12-Jpeg_Extended_Process_2_4.dcm
            // PHILIPS_Gyroscan-12-MONO2-Jpeg_Lossless.dcm
            // MARCONI_MxTWin-12-MONO2-JpegLossless-ZeroLengthSQ.dcm
            // LJPEG_BuginGDCM12.dcm
            //gdcmDebugMacro( "JWRN_MUST_DOWNSCALE" );
            //this->BitSample = jerr.pub.msg_parm.i[0];
            //assert( cinfo.data_precision == this->BitSample );
            jpeg_destroy_decompress(&cinfo);
            return false;
        } else {
            assert(0);
        }
    }
    // Let's check the color space:
    // JCS_UNKNOWN    -> 0
    // JCS_GRAYSCALE
    // JCS_RGB
    // JCS_YCbCr
    // JCS_CMYK
    // JCS_YCCK

    // Sanity checks:
    const unsigned int dims[] = {1, 2};
//    if (cinfo.image_width != dims[0]
//        || cinfo.image_height != dims[1]) {
//        //gdcmWarningMacro( "dimension mismatch. JPEG is " <<
//        //                                                cinfo.image_width << "," << cinfo.image_height << " while DICOM " << dims[0] <<
//        //                                                 "," << dims[1]  );
//        //this->Dimensions[0] = cinfo.image_width;
//        //this->Dimensions[1] = cinfo.image_height;
//        /*
//         * Long story short, the real issue is that class such as ImageRegionReader expect to read the
//         * image information without ever touching the JPEG codestream...
//         */
//        return false;
//    }
    //assert(cinfo.image_width == dims[0]);
   // assert(cinfo.image_height == dims[1]);

    switch (cinfo.jpeg_color_space) {
        case JCS_GRAYSCALE:
//                if( GetPhotometricInterpretation() != PhotometricInterpretation::MONOCHROME1
//                    && GetPhotometricInterpretation() != PhotometricInterpretation::MONOCHROME2 )
//                {
//                    gdcmWarningMacro( "Wrong PhotometricInterpretation. DICOM says: " <<
//                                                                                      GetPhotometricInterpretation() << " but JPEG says: "
//                                                                                      << (int)cinfo.jpeg_color_space );
//                    //Internals->SetPhotometricInterpretation( PhotometricInterpretation::MONOCHROME2 );
//                    this->PI = PhotometricInterpretation::MONOCHROME2;
//                }
            break;
        case JCS_RGB:
            //assert( GetPhotometricInterpretation() == PhotometricInterpretation::RGB );
            if (cinfo.process == JPROC_LOSSLESS) {
                cinfo.jpeg_color_space = JCS_UNKNOWN;
                cinfo.out_color_space = JCS_UNKNOWN;
            }
//                if( GetPhotometricInterpretation() == PhotometricInterpretation::YBR_RCT
//                    || GetPhotometricInterpretation() == PhotometricInterpretation::YBR_ICT )
//                    this->PI = PhotometricInterpretation::RGB;
            break;
        case JCS_YCbCr:
//                if( GetPhotometricInterpretation() != PhotometricInterpretation::YBR_FULL &&
//                    GetPhotometricInterpretation() != PhotometricInterpretation::YBR_FULL_422 )
//                {
//                    // DermaColorLossLess.dcm (lossless)
//                    // LEADTOOLS_FLOWERS-24-RGB-JpegLossy.dcm (lossy)
//                    gdcmWarningMacro( "Wrong PhotometricInterpretation. DICOM says: " <<
//                                                                                      GetPhotometricInterpretation() << " but JPEG says: "
//                                                                                      << (int)cinfo.jpeg_color_space );
//                    // Here it gets nasty since apparently when this occurs lossless means
//                    // we should not do any color conversion, but we *might* be breaking
//                    // correct DICOM file.
//                    // FIXME FIXME
//                    /* prevent the library from performing any color space conversion */
//                    cinfo.jpeg_color_space = JCS_UNKNOWN;
//                    cinfo.out_color_space = JCS_UNKNOWN;
//                }
            if (cinfo.process == JPROC_LOSSLESS) {
                //cinfo.jpeg_color_space = JCS_UNKNOWN;
                //cinfo.out_color_space = JCS_UNKNOWN;
            }
//                if( GetPhotometricInterpretation() == PhotometricInterpretation::YBR_FULL
//                    || GetPhotometricInterpretation() == PhotometricInterpretation::YBR_FULL_422 )
//                {
//                    cinfo.jpeg_color_space = JCS_UNKNOWN;
//                    cinfo.out_color_space = JCS_UNKNOWN;
//                    //this->PlanarConfiguration = 1;
//                }
            break;
        case JCS_CMYK:
            //assert( GetPhotometricInterpretation() == PhotometricInterpretation::CMYK );
            if (cinfo.process == JPROC_LOSSLESS) {
                cinfo.jpeg_color_space = JCS_UNKNOWN;
                cinfo.out_color_space = JCS_UNKNOWN;
            }
            break;
        case JCS_UNKNOWN:
            if (cinfo.process == JPROC_LOSSLESS) {
                cinfo.jpeg_color_space = JCS_UNKNOWN;
                cinfo.out_color_space = JCS_UNKNOWN;
            }
            break;
        default:
            assert(0);
            return false;
    }
    //assert( cinfo.data_precision == BITS_IN_JSAMPLE );
    //assert( cinfo.data_precision == this->BitSample );

    /* Step 4: set parameters for decompression */
    /* no op */


/* Step 5: Start decompressor */

// if (Internals->StateSuspension < 3 )
// {
    if (
            jpeg_start_decompress(&cinfo)
            == FALSE) {
/* Suspension: jpeg_start_decompress */
//Internals->StateSuspension = 3;
    }

/* We may need to do some setup of our own at this point before reading
 * the data.  After jpeg_start_decompress() we have the correct scaled
 * output image dimensions available, as well as the output colormap
 * if we asked for color quantization.
 * In this example, we need to make an output work buffer of the right size.
 */
/* JSAMPLEs per row in output buffer */
    row_stride = cinfo.output_width * cinfo.output_components;
    row_stride *= sizeof(JSAMPLE);
/* Make a one-row-high sample array that will go away when done with image */
    buffer = (*cinfo.mem->alloc_sarray)
            ((j_common_ptr) &cinfo, JPOOL_IMAGE, (JDIMENSION) row_stride, 1);

/* Save the buffer in case of suspension to be able to reuse it later: */
    // Internals->SampBuffer = buffer;
// }
//    else
//    {
///* JSAMPLEs per row in output buffer */
//        row_stride = cinfo.output_width * cinfo.output_components;
//        row_stride *= sizeof(JSAMPLE);
//
///* Suspension: re-use the buffer: */
//        buffer = (JSAMPARRAY) Internals->SampBuffer;
//    }

/* Step 6: while (scan lines remain to be read) */
/*           jpeg_read_scanlines(...); */

/* Here we use the library's state variable cinfo.output_scanline as the
 * loop counter, so that we don't have to keep track ourselves.
 */
    while (cinfo.output_scanline < cinfo.output_height) {
/* jpeg_read_scanlines expects an array of pointers to scanlines.
 * Here the array is only one element long, but you could ask for
 * more than one scanline at a time if that's more convenient.
 */
        if (jpeg_read_scanlines(&cinfo, buffer, 1) == 0) {
/* Suspension in jpeg_read_scanlines */
            // Internals->
            // StateSuspension = 3;
            return true;
        }
         os.write((char *) buffer[0], row_stride);
    }

/* Step 7: Finish decompression */

    if (jpeg_finish_decompress(&cinfo) == FALSE) {
/* Suspension: jpeg_finish_decompress */
        //Internals->
        //StateSuspension = 4;
        return true;
    }

/* we are done decompressing the file, now is a good time to store the type
   of compression used: lossless or not */
    if (cinfo.process == JPROC_LOSSLESS) {
        //LossyFlag = false;
    } else {
        //LossyFlag = true;
    }

/* Step 8: Release JPEG decompression object */

/* This is an important step since it will release a good deal of memory. */
    jpeg_destroy_decompress(&cinfo);

/* After finish_decompress, we can close the input file.
 * Here we postpone it until after no more JPEG errors are possible,
 * so as to simplify the setjmp error logic above.  (Actually, I don't
 * think that jpeg_destroy can do an error exit, but why assume anything...)
 */
//fclose(infile);

/* At this point you may want to check to see whether any corrupt-data
 * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
 */
/* gdcmData/D_CLUNIE_MR4_JPLY.dcm produces a single warning:
 *  Invalid SOS parameters for sequential JPEG
 * Be nice with this one:
 */
    if (jerr.pub.num_warnings > 1) {
        //gdcmErrorMacro("Too many warning during decompression of JPEG stream: " << jerr.pub.num_warnings);
        return false;
    }
/* In any case make sure the we reset the internal state suspension */
    //Internals->
    //    StateSuspension = 0;

/* And we're done! */
    return true;
}


