//
// Custom mutator for PDF mutations using QPDF
// 
// Author: Erik Viken
//

#include "afl-fuzz.h"
//#include "afl-mutations-modified.h"
//#include "afl-mutations.h"

#include "qpdf_mutator.hh"
#include "mutation-library.h"

#undef R // qpdf/QPDF.hh uses R, so it cannot be defined

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <string>
#include <cstring>

#include <qpdf/QPDF.hh>
#include <qpdf/Pl_Buffer.hh>
#include <qpdf/Constants.h> 
#include <qpdf/BufferInputSource.hh>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "cppcodec/base64_rfc4648.hpp"


void modify_values(my_mutator_t *data, rapidjson::Value& value, rapidjson::Value& parentKey, rapidjson::Document& document);

extern "C" {
void *afl_custom_init(afl_state_t *afl, unsigned int seed) {
  
  srand(seed);
  my_mutator_t *data = static_cast<my_mutator_t*>(calloc(1, sizeof(my_mutator_t)));

  if (!data) {
    perror("afl_custom_init alloc");
    return NULL;
  }

  data->fuzz_buf = (u8 *)malloc(MAX_FILE);
  data->afl_mutate_buf = (u8 *)malloc(MAX_MUTATION_BUF);
  data->duplication_buf = (u8 *)malloc(MAX_DUPLICATION_BUF);
  data->key_mutation = (u8 *)malloc(MAX_KEY_BUF);
  
  if (!data->fuzz_buf || !data->afl_mutate_buf || !data->duplication_buf || !data->key_mutation) {
    perror("afl_custom_init malloc");
    std::cerr << "Failed init " << std::endl;

    // Free initiated
    if (data->fuzz_buf) free(data->fuzz_buf);
    if (data->afl_mutate_buf) free(data->afl_mutate_buf);
    if (data->duplication_buf) free(data->duplication_buf);
    if (data->key_mutation) free(data->key_mutation);
    free(data);

    return NULL;
  }

  data->buf_size = MAX_FILE;
  data->afl_mutate_buf_size = MAX_MUTATION_BUF;
  data->duplication_buf_size = MAX_DUPLICATION_BUF;
  data->key_mutation_size = MAX_KEY_BUF;

  data->afl = afl;

  return data;
}
}

extern "C" {
size_t afl_custom_fuzz(my_mutator_t *data, uint8_t *buf, size_t buf_size,
                       u8 **out_buf, uint8_t *add_buf,
                       size_t add_buf_size,
                       size_t max_size) {

  data->duplication_rate = DUPLICATION_RATE;
  data->replacement_rate = REPLACEMENT_RATE;

  // Prepare a QPDF object and process the memory file
  QPDF qpdf;
  try {
    qpdf.setSuppressWarnings(true);

    qpdf.processMemoryFile("MemoryLoadedPDF", (char const *)buf, buf_size);

    // Set up a buffer pipeline to capture JSON output
    Pl_Buffer json_output("buffer");

    // Choose the decode level and json stream data options
    qpdf_stream_decode_level_e decode_level = qpdf_dl_all;
    qpdf_json_stream_data_e json_stream_data = qpdf_sj_inline;

    // Conversion settings
    std::string file_prefix = ""; // Not needed for memory buffer
    std::set<std::string> wanted_objects;

    // Write JSON data to the buffer
    qpdf.writeJSON(2, &json_output, decode_level, json_stream_data, file_prefix, wanted_objects);

    // Access the JSON data
    Buffer* result_buffer = json_output.getBuffer();
    const unsigned char* json_data = result_buffer->getBuffer();
    size_t json_length = result_buffer->getSize();

    std::string json_string(reinterpret_cast<const char*>(json_data), json_length);

    delete result_buffer;

    // Set up JSON
    rapidjson::Document document;
    document.Parse(json_string.c_str());

    if (document.HasParseError()) {
      // Error parsing JSON
      return 1;
    }

    // Skip the first element of the "qpdf" array
    if (document.HasMember("qpdf") && document["qpdf"].IsArray()) {
      rapidjson::Value& elements = document["qpdf"];

      data->stream_mutation = rand() % 2;

      // Skip first element, as this data should not be mutated.
      for (rapidjson::SizeType i = 1; i < elements.Size(); ++i) { 
        rapidjson::Value& element = elements[i];
        for (rapidjson::Value::MemberIterator itr = element.MemberBegin(); itr != element.MemberEnd(); ++itr) {
          modify_values(data, itr->value, itr->name, document);
        }
      }
    }

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    std::shared_ptr<InputSource> input_source = std::make_shared<BufferInputSource>("json_buffer", buffer.GetString());

    qpdf.createFromJSON(input_source);

    QPDFWriter pdf_writer(qpdf);
    pdf_writer.setOutputMemory();
    pdf_writer.write();
    Buffer* pdf_buffer = pdf_writer.getBuffer();

    const unsigned char* pdf_data = pdf_buffer->getBuffer();
    size_t pdf_length = pdf_buffer->getSize();

    if(pdf_length < max_size){
      if(pdf_length < data->buf_size){
        memcpy(reinterpret_cast<char*>(data->fuzz_buf), (const char *)pdf_data, pdf_length);

        *out_buf = data->fuzz_buf;

        qpdf.closeInputSource();
        delete pdf_buffer;
      
        return pdf_length;
      }
    }

    delete pdf_buffer;

  }catch (std::exception& e) {

    qpdf.closeInputSource();

    // This will be triggered when: two identical keys (example: "/Size") are in the same object (should not be common)

    memcpy(reinterpret_cast<char*>(data->fuzz_buf), (const char *)buf, buf_size);

    *out_buf = data->fuzz_buf;
    return buf_size;
  }

  qpdf.closeInputSource();

  memcpy(reinterpret_cast<char*>(data->fuzz_buf), (const char *)buf, buf_size);

  *out_buf = data->fuzz_buf;
  return buf_size;
}
}

extern "C" {
void afl_custom_deinit(my_mutator_t *data) {

  free(data->fuzz_buf);
  free(data->afl_mutate_buf);
  free(data->duplication_buf);
  free(data->key_mutation);
  free(data);

}
}

//-------------------------------------------------------------
// These methods are here to disable all trimming done by AFL++
// ------------------------------------------------------------

extern "C" {
int afl_custom_init_trim(void *data, unsigned char *buf, size_t buf_size){
  return 0;
}
}

extern "C" {
size_t afl_custom_trim(my_mutator_t *data, unsigned char **out_buf){
  return data->buf_size;
}
}

extern "C" {
int afl_custom_post_trim(void *data, unsigned char success){
  return 0;
}
}

// ----------------------------------------
// Helper functions for modifying JSON data
// ----------------------------------------

void modify_values(my_mutator_t *data, rapidjson::Value& value, rapidjson::Value& parentKey, rapidjson::Document& document) {

  // Don't process things in trailer (breaks QPDF)
  if(is_trailer(parentKey)){
    return;
  }

  // Mutate based on type
  if (value.IsObject()) {
    for (rapidjson::Value::MemberIterator itr = value.MemberBegin(); itr != value.MemberEnd(); ++itr) {      
      modify_values(data, itr->value, itr->name, document);
    }

  } else if (value.IsString()) {
    // Mutate base64 encoded data
    if (is_string(parentKey) && std::string(parentKey.GetString()) == "data") {
      if(data->stream_mutation == 1){
        mutate_stream(data, value, document); 
      }else{
        mutate_stream_default(data, value, document); 
      }
    }
  }
}
