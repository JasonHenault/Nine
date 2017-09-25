#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NTOOLS;

Log::Log() {
    std::cout << "Start logging." << std::endl;
}

Log::~Log() {
    SaveReport();
}

void Log::RegisterInfo (const std::string &info) {
    _info.push_back(info);
}

void Log::RegisterWarning (const std::string &warn, std::string file, int line) {
    WarnAndError wae;
    wae.mes = warn;
    wae.file = file;
    wae.line = line;

    _warning.push_back(wae);
}

void Log::RegisterError (const std::string &error, std::string file, int line) {
    std::cout << file << " (" << line << ") error : \"" << error << "\"" << std::endl;

    WarnAndError wae;
    wae.mes = error;
    wae.file = file;
    wae.line = line;

    _error.push_back(wae);
}

void Log::RegisterPackage (const std::string &path, const std::string type) {
    Package p;
    p.path = path;
    p.type = typeid(type).name();

    _package.push_back(p);
}

void Log::SaveReport() {
    std::cout << _error.size() << " errors, " << _warning.size() << " warnings, " << _package.size() << " packages and " << _info.size() << " informations. Saving ..." << std::endl;

    std::ostringstream html;

    // standart body
    html << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\"> \n \
    <html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" > \n \
       <head> \n \
              <title>Nine report</title> \n \
              <meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\" /> \n \
              </head> \n \
              <body> \n \
              <img src=\"banner.png\" alt=\"Report standart banner\" /> \n \
              <br/> \n \
              <img src=\"information.png\" alt=\"info\" title=\"Informations\"/> (";
              html << _info.size();
              html << ")  \n \
              <img src=\"warning.png\" alt=\"warning\" title=\"Warnings\"/> (";
              html << _warning.size();
              html << ")  \n \
              <img src=\"error.png\" alt=\"error\" title=\"Errors\"/> (";
              html << _error.size();
              html << ")  \n \
              <img src=\"package.png\" alt=\"package\" title=\"Packages\"/> (";
              html << _package.size();
              html << ") \n \
              <br /> \n \
              <textarea name=\"messages\" id=\"messages\" rows=\"25\" cols=\"100\"> \n";
    ////////////////

    html << "Infos : \n\n";

    for(int i = 0; i < _info.size(); i++) {
        html << i+1;
        html << ") ";
        html << _info[i] + "\n";
    }

    html << "</textarea> \n \
            </br> \n \
            <textarea name=\"warning\" id=\"warning\" rows=\"25\" cols=\"100\"> \n";

    html << "Warnings : \n\n";

    for(int i = 0; i < _warning.size(); i++) {
        html << i+1;
        html << ") ";
        html << _warning[i].mes + "\n";
    }

    html << "</textarea> \n \
            </br> \n \
            <textarea name=\"error\" id=\"error\" rows=\"25\" cols=\"100\"> \n";

    html << "Errors :\n\n";

    for(int i = 0; i < _error.size(); i++) {
        html << i+1;
        html << ") ";
        html << _error[i].mes + "\n";
    }

    html << "</textarea> \n \
            </br> \n \
            <textarea name=\"package\" id=\"package\" rows=\"25\" cols=\"100\"> \n";

    html << "Package :\n\n";

    for(int i = 0; i < _package.size(); i++) {
        html << i+1;
        html << ") ";
        html << _package[i].path + "\n";
    }

    html << "</textarea> \n \
            </body> \n \
            </html>";

    std::ofstream file("Report.html", std::ios::out | std::ios::trunc);
    file << html.str();
    file.close();

    std::cout << "Report saved in the execution directory" << std::endl;
}
