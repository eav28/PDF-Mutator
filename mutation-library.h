#ifndef MUTATION_LIBRARY  // Include guard to prevent multiple inclusions
#define MUTATION_LIBRARY

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

// rand() % RATE determines 
#define NAME_OBJECT_MUATATION_RATE 16
#define INVALID_NAME_OBJECT_MUTATION 5

#define INTEGER_MUATATION_RATE 16
#define DOUBLE_MUATATION_RATE 16
#define STREAM_MUATATION_RATE 2


#define MUTATION_RATE 16
#define DUPLICATION_RATE 50
#define REPLACEMENT_RATE 70
#define MUTATION_SUB_ADD 40 // ADD OR SUBTRACT ITEMS TO STREAM, RENAME: STREAM_SUB_ADD_MUTATION_RATE
#define MUTATION_RATE_STREAM 4 // RENAME: STREAM_MUTATION_RATE

typedef struct my_mutator {
    
    afl_state_t *afl;

    u8 *fuzz_buf;
    size_t buf_size;
    //int next_mutation;
    int mutation_rate;

    int stream_mutation = 0;

    u8 *json_buf;
    size_t json_len;
    size_t json_size;
    int json_is_set = 0;

    u8 *afl_mutate_buf;
    size_t afl_mutate_buf_size;

    //int next_duplication;
    //int next_replacement;

    int duplication_rate = DUPLICATION_RATE;
    int replacement_rate = REPLACEMENT_RATE;

    u8 *duplication_buf;
    int has_duplicate_data = 0;
    size_t duplication_buf_size;

    u8 *key_mutation;
    size_t key_mutation_size;

} my_mutator_t;


int randomness(my_mutator_t *data, uint32_t mod);
//int next_mutation(my_mutator_t *data);
int mutation_chance(uint32_t mod);
int should_mutate(my_mutator_t *data);
int is_trailer(rapidjson::Value& key);
int is_string(rapidjson::Value& key);
int is_duplicable(rapidjson::Value& key);
int is_key(rapidjson::Value& key);
uint32_t get_interesting_num(my_mutator_t *data);
void mutate_integer(my_mutator_t *data, rapidjson::Value& value, rapidjson::Document& document);
void mutate_double(my_mutator_t *data, rapidjson::Value& value, rapidjson::Document& document);
void mutate_key(my_mutator_t *data, rapidjson::Value& key, rapidjson::Document& document);
int mutate_stream(my_mutator_t *data, rapidjson::Value& value, rapidjson::Document& document);
int mutate_stream_default(my_mutator_t *data, rapidjson::Value& value, rapidjson::Document& document);
int should_duplicate(my_mutator_t *data);
int should_replace(my_mutator_t *data);
int duplicate(my_mutator_t* data, rapidjson::Value& value);
int replace(my_mutator_t* data, rapidjson::Value& value, rapidjson::Document& document);
int set_new_value(my_mutator_t* data, rapidjson::Value& value, rapidjson::Document& document);
int add_array_element(my_mutator_t* data, rapidjson::Value& array, rapidjson::Document& document);
int remove_array_element(my_mutator_t* data, rapidjson::Value& array, rapidjson::Document& document);
int swap_value(my_mutator_t* data, rapidjson::Value& value, rapidjson::Document& document);
int is_value_replaceable(rapidjson::Value& key);
void add_random_entry(my_mutator_t *data, rapidjson::Value &object, rapidjson::Document &document);
void remove_random_entry(my_mutator_t *data, rapidjson::Value &object);

//
// pdf_keys taken from AFLplusplus/dictionaries/pdf.dict
//
// Created by Ben Nagy <ben@iagu.net>
//

extern std::string pdf_keys[];
extern std::string stream_dict[];

#endif // MUTATOR_H