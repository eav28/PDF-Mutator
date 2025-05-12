#include <qpdf/QPDF.hh>
#include <qpdf/Pl_Buffer.hh>
#include <qpdf/Constants.h>
#include <qpdf/BufferInputSource.hh>

#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <string>
#include <chrono>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

using namespace rapidjson;

#include <qpdf/InputSource.hh>
#include <memory>
#include <string>
#include <cstring>

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

#include <cstdlib>
#include <iostream>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc < 6) {
        std::cerr << "Usage: " << argv[0]
                  << " -i <input_dir> -o <output_dir> [afl-cmin options] -- <program @@>\n";
        return 1;
    }

    std::string input_dir, output_dir;
    std::vector<std::string> afl_args;
    std::string program_args;
    bool found_double_dash = false;

    // Args
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-i" && i + 1 < argc) {
            input_dir = argv[++i];
        } else if (arg == "-o" && i + 1 < argc) {
            output_dir = argv[++i];
        } else if (arg == "--") {
            found_double_dash = true;
            for (int j = i + 1; j < argc; ++j) {
                program_args += std::string(" ") + argv[j];
            }
            break;
        } else {
            afl_args.push_back(arg);
        }
    }

    if (input_dir.empty() || output_dir.empty() || !found_double_dash) {
        std::cerr << "Error: Missing required arguments.\n";
        return 1;
    }

    // Create tmp directory
    if (system("mkdir -p tmp_12345") != 0) {
        std::cerr << "Error creating directory\n";
        return 1;
    }

    int file_amount = 0;
    int valid_files = 0;

    for (const auto& entry : fs::directory_iterator(input_dir)) {
        std::cout << "Opening file: " << entry.path() << std::endl;
        file_amount++;

        try {
            std::ifstream file(entry.path(), std::ios::binary | std::ios::ate);
            if (!file) continue;

            std::streamsize length = file.tellg();
            file.seekg(0, std::ios::beg);
            std::vector<char> buffer(length);
            if (!file.read(buffer.data(), length)) continue;

            QPDF qpdf;
            qpdf.setSuppressWarnings(true);
            qpdf.processMemoryFile("MemoryLoadedPDF", buffer.data(), buffer.size());

            Pl_Buffer json_output("buffer");
            qpdf.writeJSON(2, &json_output, qpdf_dl_all, qpdf_sj_inline, "", {});

            Buffer* result_buffer = json_output.getBuffer();
            std::string json_string(reinterpret_cast<const char*>(result_buffer->getBuffer()),
                                    result_buffer->getSize());

            rapidjson::Document document;
            document.Parse(json_string.c_str());
            if (document.HasParseError()) continue;

            rapidjson::StringBuffer sb;
            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
            document.Accept(writer);

            std::shared_ptr<InputSource> input_source =
                std::make_shared<BufferInputSource>("json_buffer", sb.GetString());
            qpdf.createFromJSON(input_source);

            QPDFWriter pdf_writer(qpdf, ("tmp_12345/" + entry.path().filename().string()).c_str());
            pdf_writer.write();

            valid_files++;
        } catch (const std::exception& e) {
            std::cerr << "Error processing PDF file: " << e.what() << std::endl;
            continue;
        }
    }

    std::cout << "Processed: " << valid_files << "/" << file_amount << std::endl;

    // Construct afl-cmin command
    std::string afl_cmd = "afl-cmin -i tmp_12345 -o " + output_dir;
    for (const auto& arg : afl_args) {
        afl_cmd += " " + arg;
    }
    afl_cmd += " --" + program_args;

    std::cout << "Executing: " << afl_cmd << std::endl;

    if (system(afl_cmd.c_str()) != 0) {
        std::cerr << "Error running afl-cmin\n";
    }

    if (system("rm -r tmp_12345") != 0) {
        std::cerr << "Error deleting tmp directory\n";
        return 1;
    }

    return 0;
}