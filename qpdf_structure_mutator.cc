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
#include <qpdf/Constants.h> // For decoding levels and other constants
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
                       size_t add_buf_size,  // add_buf can be NULL
                       size_t max_size) {

  //data->next_mutation = next_mutation(data);
  //data->next_duplication = rand() % 3;
  
  data->duplication_rate = DUPLICATION_RATE;
  data->replacement_rate = REPLACEMENT_RATE;

  //std::cout << "BUF: " << buf << std::endl;

  // Prepare a QPDF object and process the memory file
  QPDF qpdf;
  try {

    //std::cout << "WAY BEFORE" << std::endl;
    qpdf.setSuppressWarnings(true);

    qpdf.processMemoryFile("MemoryLoadedPDF", (char const *)buf, buf_size);

    // Set up a buffer pipeline to capture JSON output
    Pl_Buffer json_output("buffer");

    // Choose the decode level and json stream data options
    qpdf_stream_decode_level_e decode_level = qpdf_dl_all;
    qpdf_json_stream_data_e json_stream_data = qpdf_sj_inline;

    // Conversion settings
    std::string file_prefix = ""; // Not needed for memory buffer
    std::set<std::string> wanted_objects; // Empty to convert all objects

    // Write JSON data to the buffer
    qpdf.writeJSON(2, &json_output, decode_level, json_stream_data, file_prefix, wanted_objects);

    // Access the JSON data
    Buffer* result_buffer = json_output.getBuffer();
    const unsigned char* json_data = result_buffer->getBuffer();
    size_t json_length = result_buffer->getSize();

    //std::cout << "BEFORE: " << json_data << std::endl;

    std::string json_string(reinterpret_cast<const char*>(json_data), json_length);

    delete result_buffer;

    // Set up JSON
    rapidjson::Document document;
    document.Parse(json_string.c_str());

    if (document.HasParseError()) {
      // Error parsing JSON
      //std::cout << "ERROR BEGINS...." << std::endl;

      return 1;
    }

    //std::cout << "BEFORE" << std::endl;

    // Skip the first element of the "qpdf" array
    if (document.HasMember("qpdf") && document["qpdf"].IsArray()) {
      rapidjson::Value& elements = document["qpdf"];

      data->stream_mutation = rand() % 2;

      // skip the first element, as this data should not be mutated.
      for (rapidjson::SizeType i = 1; i < elements.Size(); ++i) { 
        data->mutation_rate = (rand() % 30) + 3; // Gives every mutation a different mutation rate
        rapidjson::Value& element = elements[i];
        for (rapidjson::Value::MemberIterator itr = element.MemberBegin(); itr != element.MemberEnd(); ++itr) {
          modify_values(data, itr->value, itr->name, document);
        }
      }
    }

    //std::cout << "AFTER" << std::endl;

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    //std::cout << "AFTER: " << buffer.GetString() << std::endl;

    std::shared_ptr<InputSource> input_source = std::make_shared<BufferInputSource>("json_buffer", buffer.GetString());

    // Premature end of input...
    qpdf.createFromJSON(input_source);

    QPDFWriter pdf_writer(qpdf);
    pdf_writer.setOutputMemory();
    pdf_writer.write();
    Buffer* pdf_buffer = pdf_writer.getBuffer();

    const unsigned char* pdf_data = pdf_buffer->getBuffer();
    size_t pdf_length = pdf_buffer->getSize();

    //std::cout << "WAY AFTER" << std::endl;

    if(pdf_length < max_size){
      if(pdf_length < data->buf_size){
        memcpy(reinterpret_cast<char*>(data->fuzz_buf), (const char *)pdf_data, pdf_length);

        *out_buf = data->fuzz_buf;

        //std::cout << "AFTER: " << data->fuzz_buf << std::endl;

        qpdf.closeInputSource();
        delete pdf_buffer;

        //std::cout << "SUCSESS! ------------------------" << std::endl;
      
        return pdf_length;
      }
    }

    delete pdf_buffer;

  }catch (std::exception& e) {

    //std::cout << "FAILED....." << e.what() << std::endl;
    //std::cout << data->afl->stage_cur << " : " << data->afl->stage_max  << std::endl;
    //std::cout << "HERE: " << data->fuzz_buf << std::endl;

    // triggered if two keys are in same object (uncommon)
    qpdf.closeInputSource();
    //*out_buf = buf;
    memcpy(reinterpret_cast<char*>(data->fuzz_buf), (const char *)buf, buf_size);


    *out_buf = data->fuzz_buf;
    return buf_size;

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

  // key mutation
  if(is_key(parentKey)){
    if(should_mutate(data)){
      //std::cout << "KEY MUTATION" << std::endl;
      mutate_key(data, parentKey, document);
    }
  }

  // Replace value type
  if(should_mutate(data)) {
    if(is_value_replaceable(parentKey)){
      swap_value(data, value, document);
      return;
    }
  }

  // replace value
  //if(data->has_duplicate_data && should_replace(data)){
  if(data->has_duplicate_data && mutation_chance(data->replacement_rate)){
    if(is_duplicable(parentKey)){
      if(is_value_replaceable(parentKey)){
        //std::cout << "REPLACE" << data->replacement_rate << std::endl;
        replace(data, value, document);
        return;
      }
    }
  }

  // duplicate value
  //if(should_duplicate(data)){
  if(mutation_chance(data->duplication_rate)){
    if(is_value_replaceable(parentKey)){
    //if(is_duplicable(parentKey)){
      //std::cout << "DUPLICATE" << std::endl;
      duplicate(data, value);
    }
  }

  // Mutate based on type
  if (value.IsObject()) {
    for (rapidjson::Value::MemberIterator itr = value.MemberBegin(); itr != value.MemberEnd(); ++itr) {      
      modify_values(data, itr->value, itr->name, document);
    }

    // Add/remove object here
    if(should_mutate(data)){
      if(rand() % 2 == 0){
        add_random_entry(data, value, document);
      }else{
        remove_random_entry(data, value);
      }
    }

  } else if (value.IsArray()) {

    // Add/Remove values to array
    if(should_mutate(data)) {
      if(rand() % 2 == 0){
        //std::cout << "ADD ARRAY MUTATION" << std::endl;
        add_array_element(data, value, document);
      }else{
        //std::cout << "REMOVE ARRAY MUTATION" << std::endl;
        remove_array_element(data, value, document);
      }
    }

    for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
      //std::cout << "ARRAY MUTATION" << std::endl;
      rapidjson::Value dummy;
      modify_values(data, value[i], dummy, document);
    }

  } else if (value.IsInt()) {

    // Change the int values randomly in positive and negative directions
    if (should_mutate(data)) {
      //std::cout << "INT MUTATION" << data->mutation_rate << std::endl;
      mutate_integer(data, value, document);
    }

  } else if (value.IsString()) {

    // Mutate stream data
    /*if (is_string(parentKey) && std::string(parentKey.GetString()) == "data") {
      if(randomness(data, 2) > 0){
        //std::cout << "CONTENT STREAM MUTATION" << std::endl;
        //mutate_stream(data, value, document);
        if(data->stream_mutation == 1){
          mutate_stream(data, value, document); 
        }else{
          mutate_stream_default(data, value, document); 
        }
      }
    }*/

    // Mutate name objects
    if(is_key(value)){
      if(should_mutate(data)){
        //std::cout << "KEY VALUE MUTATION" << std::endl;
        mutate_key(data, value, document);
      }
    }

  } else if (value.IsNumber()) {
    if (value.IsDouble()) {
      //std::cout << "DOUBLE MUTATION" << std::endl;
      mutate_double(data, value, document);
    }
  }
}
