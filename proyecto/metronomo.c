
/*
 * metronomo.c
 * Aplicación que procesa un fichero de vídeo y
 * analiza el audio para detectar el tiempo (los pulsos por minuto o beats per minute) 
 * y aplica efectos de vídeo en función de dicho tiempo.
 * 
 * Proyecto GStreamer - Aplicaciones Multimedia (2018-19)
 * Universidad Carlos III de Madrid
 *
 * Equipo (TO DO: rellenar con los datos adecuados):
 * - Alumno 1 (nombre, apellidos y NIA)
 * - Alumno 2 (nombre, apellidos y NIA)
 *
 * Versión implementada (TO DO: eliminar las líneas que no procedan):
 * - versión básica
 * - intervalo 
 * - efectos de vídeo
 *
 */




  // ------------------------------------------------------------
  // Procesar argumentos
  // ------------------------------------------------------------
  
  // REF: https://www.gnu.org/software/libc/manual/html_node/Parsing-Program-Arguments.html#Parsing-Program-Arguments

  /* 
   * Argumentos del programa:
   * -h: presenta la ayuda y termina (con estado 0)
   * -i inicio: instante inicial a partir del cual reproducir y realizar la detección (en nanosegundos)
   * -f fin: instante final en el que detener la reproducción y detección (en nanosegundos)
   * -l límite_inferior: umbral inferior, si el tempo detectado es menor que este umbral se aplicará un efecto de vídeo antiguo.
   * -g límite_superior: umbral superior, si el tempo detectado es mayor que este umblar se aplicará un efecto de detección de bordes
   * fichero_entrada: fichero de vídeo a analizar
   */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <gst/gst.h>


GstElement *queue_aud;
GstElement *queue_img;

int estado = 0;

/* --------------------------------------------------------------------- *
 *  Bus message handler
 * --------------------------------------------------------------------- */
static gboolean
bus_call (GstBus     *bus,
          GstMessage *msg,
          gpointer    data)
{
  GMainLoop *loop = (GMainLoop *) data;
  char *src = GST_MESSAGE_SRC_NAME(msg);
  
  switch (GST_MESSAGE_TYPE (msg)) {

  case GST_MESSAGE_EOS:
    g_print ("..[bus].. (%s) :: End of stream\n", src);
    g_main_loop_quit (loop);
    break;

  case GST_MESSAGE_ERROR: {
    gchar  *debug;
    GError *error;
    
    gst_message_parse_error (msg, &error, &debug);
    g_free (debug);
    
    g_printerr ("..[bus].. (%s) :: Error: %s\n", src, error->message);
    g_error_free (error);

    estado = -1;  // error multimedia
    g_main_loop_quit (loop);
    break;
  }
  case GST_MESSAGE_TAG:{
    GstTagList *tags = NULL;
    gst_message_parse_tag (msg, &tags);
    gdouble value = 0;
    gst_tag_list_get_double (tags, GST_TAG_BEATS_PER_MINUTE, &value);

	if(value != 0.0 ) g_print ("[BPMS]: %lf : \n",value);
  }
  default: {
    g_print(" %lu \n", GST_MESSAGE_TIMESTAMP(msg));
    g_print ("..[bus].. %15s :: %-15s\n", src, GST_MESSAGE_TYPE_NAME(msg));
    break;
  }
  }

  return TRUE;
}



/* --------------------------------------------------------------------- *
 *  Link dynamically created pads
 * --------------------------------------------------------------------- */
static void
on_pad_added (GstElement *element,
              GstPad     *pad,
              gpointer    data)
{
  GstPad *sinkpad;

  gchar *name = gst_pad_get_name(pad);
  char *caps = gst_caps_to_string(gst_pad_get_current_caps(pad));

  g_print ("...Dynamic pad created: %s, capabilities: %s\n", name, caps);

  /* We can now link this pad with the appropriate decoder sink pad */

  if (g_str_has_prefix(caps, "audio")) {
    /* Audio dynamic pad: conect audio pipeline */
    sinkpad = gst_element_get_static_pad (queue_aud, "sink");
    
  } else if  (g_str_has_prefix(caps, "video")) {
    /* Audio dynamic pad: conect audio pipeline */
    sinkpad = gst_element_get_static_pad (queue_img, "sink");
  }

  if (sinkpad != NULL) {
    /* connect pads */
    gst_pad_link (pad, sinkpad);

    /* delete objects */
    gst_object_unref (sinkpad);
  }

}


int main(int argc, char *argv[]) {
/*COMPROBACION DE ARGUMENTOS*/
/*------------------------------------------------------------------------------------------------------------*/
  int c;
  char *filename;
    
  opterr = 0;  // no es necesario declararla, la exporta getopt
  /*con getopt si un argumento (como h) no tiene dos puntos significa que no tiene argumento, si tiene dos puntos
  es porque el argumento es requerido*/
  while ((c = getopt (argc, argv, "hi:f:l:g:")) != -1) {
    g_print("¿ESTA ENTRANDO EN EL BUCLE?\n");
    switch (c)
      {
      case 'h':
	// ayuda
	g_print("Funcionalidad ayuda no implementada\n");
        break;

      case 'i':
	// tiempo inicio intervalo de vídeo a procesar
	g_print("Funcionalidad intervalo no implementada\n");
        break;

      case 'f':
	// tiempo inicio intervalo de vídeo a procesar
	g_print("Funcionalidad intervalo no implementada\n");
        break;

      case 'l':
	// umbral inferior para efectos de vídeo
	g_print("Funcionalidad efectos no implementada\n");
        break;

      case 'g':
	// umbral superior para efectos de vídeo
	g_print("Funcionalidad efectos no implementada\n");
        break;

      case '?':
	// getopt devuelve '?' si encuentra una opción desconocida
	// o si falta el argumento para una opción que lo requiere
	// La opción conflictiva queda almacenada en optopt
        if ((optopt == 'i') ||(optopt == 'f') || (optopt == 'l') ||(optopt == 'g')) {
	  // falta argumento para opción que lo requiere
          fprintf (stderr, "Error: la opción -%c requiere un argumento\n", optopt);
	}
	// error: opción desconocida
        else if (isprint (optopt))
          fprintf (stderr, "Error: argumento `-%c' no válido\n", optopt);
        else
          fprintf (stderr, "Error: argumento `\\x%x' no válido.\n", optopt);
        return 1;
	
      default:
        fprintf (stderr,
                 "Error: argumento %d no válido\n", optind);
        
        return 1;
      }
  }

  filename = argv[optind];
  
  // getopt recoloca los argumentos no procesados al final
  // el primero será el nombre del fichero de entrada (es correcto)
  // si hay algún otro: error argumento desconocido
  for (int index = optind+1; index < argc; index++) {
    printf ("Error: argumento %s no válido\n", argv[index]);
    return 1;
  }

/*FIN COMPROBACION DE ARGUMENTOS*/
/*------------------------------------------------------------------------------------------------------------*/

  GMainLoop *loop;

  /* Check input arguments 
  if (argc != 2) {
    g_printerr ("Usage: %s <Ogg/Theora filename>\n", argv[0]);
    return -1;  // error argumentos
  }*/



  puts("GStreamer test - init!\n"); /* prints GStreamer test! */

  gst_init(&argc, &argv);  /* inits the GStreamer library */
  loop = g_main_loop_new (NULL, FALSE);

  /* 
   * Create pipeline
   */

  /* Create pipeline */
  GstElement *pipeline = gst_pipeline_new ("mi_pipeline");

   /* Create elements */
  GstElement *source = gst_element_factory_make("filesrc", "fuente_fichero");
  GstElement *decode = gst_element_factory_make("decodebin", "decode");
  GstElement *bpm = gst_element_factory_make("bpmdetect", "bpm");
  GstElement *f_colorspace = gst_element_factory_make("videoconvert", "transform_colorspace");
  GstElement *f_audioconv = gst_element_factory_make("audioconvert", "audio_convert");
  GstElement *aud_sink = gst_element_factory_make("alsasink", "audio-output");
  GstElement *img_sink = gst_element_factory_make("ximagesink", "image-output");


  queue_aud = gst_element_factory_make("queue", "queue-aud");
  queue_img = gst_element_factory_make("queue", "queue-img");

  /* Error checking */
  if (!pipeline || !source || !decode || !bpm
       || !queue_aud || !f_audioconv  || !aud_sink
       || !queue_img || !f_colorspace || !img_sink) 
    {
      g_printerr ("One element could not be created. Exiting.\n");
      return -1;   // error multimedia
    }


  /* 
   * Set up the pipeline
   */

  /* set the input filename to the source element */
  g_object_set (G_OBJECT (source), "location", argv[1], NULL);

  /* set the name for the decoder element */
 // g_object_set (G_OBJECT (decode), "decode", "decode", NULL);


  /* Bus message handling */
  GstBus *bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_add_watch (bus, bus_call, loop);
  gst_object_unref (bus);

  

  /* Add elements to pipeline */
  gst_bin_add_many(GST_BIN(pipeline), source, decode, queue_aud, queue_img, f_colorspace, f_audioconv, bpm, aud_sink, img_sink, NULL);

  /* Link elements */

  /* note that the demuxer will be linked to the decoder dynamically.
     The reason is that Ogg may contain various streams (for example
     audio and video). The source pad(s) will be created at run time,
     by the demuxer when it detects the amount and nature of streams.
     Therefore we connect a callback function which will be executed
     when the "pad-added" is emitted.*/

  /* demuxer src pad is created dinamically -> needs to be linked later */
  gst_element_link (source, decode);
  gst_element_link_many(queue_aud, f_audioconv,  bpm, aud_sink, NULL);
  gst_element_link_many(queue_img, f_colorspace, img_sink, NULL);

  /* connect demuxer to the rest of the pipeline when pad is dinamycally added */
  g_signal_connect (decode, "pad-added", G_CALLBACK (on_pad_added), NULL);



  /*
   * Change of State 
   */

  /* Set the pipeline to "playing" state*/
  g_print("Play: %s\n", argv[1]);
  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  /* Iterate */
  g_print ("Run...\n");
  g_main_loop_run (loop);




  /*
   * END: clean up objects
   * Out of the main loop, clean up nicely 
   */

  g_print ("Returned, stopping playback\n");

  /* set state */
  gst_element_set_state (pipeline, GST_STATE_NULL);

  /* 
   * Destroy objects (deallocate memory)
   * elements cannot be directly disposed while still in pipeline (or bin)
   */
  g_print ("Deleting pipeline\n");
  gst_object_unref (GST_OBJECT (pipeline));


  puts("\nGStreamer test - end!"); /* prints GStreamer test! */

  return estado;
}




