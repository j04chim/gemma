// MEOW: comments, tests, doc
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/types.h>
#include "logger.h"

#include "gemma.h"

Gemma::Gemma( std::string s ) { open(s); };

bool Gemma::open( std::string path ) {

    this->_path = path;

    std::fstream file(
        path, std::fstream::in | std::fstream::binary
    );
    if ( !file.is_open() ) {
        logger(3, ("Can't open file at " + path).c_str());
        file.close();
        return false;
    }

    // Gemma always start with ᓚᘏᗢ
    std::string fcat(9, '\0');
    std::string rcat = "ᓚᘏᗢ";
    file.read(fcat.data(), 9);
    if ( !file ) {
        logger(3, "Error reading header symboles");
        return false;
    }

    // version number
    u_int64_t version = 0;
    file.read(reinterpret_cast<char*>(&version), 8);
    if ( !file ) {
        logger(3, "Error reading version bytes in header");
        return false;
    }

    if (version != VERSION_NUMBER) {
        logger(3, ("Wrong version number " + std::to_string(version)).c_str());
        return false;
    }

    this->_is_open = true;

    return _readMetadata();

}

bool Gemma::_readMetadata() {

    if (!this->_is_open) {
        logger(3, "Tried to read metadata when no file was open");
        return false;
    }

    std::fstream file(
        this->_path, std::ios_base::in | std::ios_base::binary
    );
    if ( !file.is_open() ) {
        logger(3, ("Can't open file at " + this->_path + " to read metadata").c_str());
        file.close();
        return false;
    }

    // skip header
    file.seekg(17);

    u_int32_t meta_size = 0;
    u_int32_t total_size = 8 + 7 * 4 + 1; // meta size x 2
    file.read(reinterpret_cast<char*>(&meta_size), 4);
    if ( !file ) {
        logger(3, "Error reading metadata size at SOM");
        return false;
    }

    // -- Owner --
    u_int32_t owner_size;
    file.read(reinterpret_cast<char*>(&owner_size), 4);
    if ( !file ) {
        logger(3, "Error reading owner size");
        return false;
    }
    total_size += owner_size;

    this->_owner = std::string(owner_size, '\0');
    file.read(this->_owner.data(), owner_size);
    if ( !file ) {
        logger(3, "Error reading owner data");
        return false;
    }

    // -- Title --
    u_int32_t title_size;
    file.read(reinterpret_cast<char*>(&title_size), 4);
    if ( !file ) {
        logger(3, "Error reading title size");
        return false;
    }
    total_size += title_size;

    this->_title = std::string(title_size, '\0');
    file.read(this->_title.data(), title_size);
    if ( !file ) {
        logger(3, "Error reading title data");
        return false;
    }

    // -- Description --
    u_int32_t desc_size;
    file.read(reinterpret_cast<char*>(&desc_size), 4);
    if ( !file ) {
        logger(3, "Error reading description size");
        return false;
    }
    total_size += desc_size;

    this->_description = std::string(desc_size, '\0');
    file.read(this->_description.data(), desc_size);
    if ( !file ) {
        logger(3, "Error reading description data");
        return false;
    }

    // -- URL --
    u_int32_t url_size;
    file.read(reinterpret_cast<char*>(&url_size), 4);
    if ( !file ) {
        logger(3, "Error reading url size");
        return false;
    }
    total_size += url_size;

    this->_url = std::string(url_size, '\0');
    file.read(this->_url.data(), url_size);
    if ( !file ) {
        logger(3, "Error reading url data");
        return false;
    }

    // -- Cover --
    u_int32_t cover_size;
    file.read(reinterpret_cast<char*>(&cover_size), 4);
    if ( !file ) {
        logger(3, "Error reading cover size");
        return false;
    }
    total_size += cover_size;

    this->_cover = std::string(cover_size, '\0');
    file.read(this->_cover.data(), cover_size);
    if ( !file ) {
        logger(3, "Error reading cover data");
        return false;
    }

    // -- Fields --
    u_int32_t fields_size;
    file.read(reinterpret_cast<char*>(&fields_size), 4);
    if ( !file ) {
        logger(3, "Error reading fields size");
        return false;
    }
    total_size += fields_size;

    std::string fields = std::string(fields_size, '\0');
    file.read(fields.data(), fields_size);
    if ( !file ) {
        logger(3, "Error reading fields data");
        return false;
    }

    this->_fields.clear();
    size_t pos = 0;
    std::string token;
    while ((pos = fields.find_first_of(';')) != std::string::npos) {
        token = fields.substr(0, pos);
        this->_fields.push_back(token);
        fields.erase(0, pos + 1);
    }
    this->_fields.push_back(fields);

    // -- Creation date --
    file.read(reinterpret_cast<char*>(&this->_creation), 4);
    if ( !file ) {
        logger(3, "Error reading creation date");
        return false;
    }

    // -- Compression --
    file.read(reinterpret_cast<char*>(&this->_compression), 1);
    if ( !file ) {
        logger(3, "Error reading compression type");
        return false;
    }


    // Size check
    if ( meta_size == total_size ) return true;
    logger(3, ("Wrong metadata size, SOM: " + std::to_string(meta_size) +
                ", Total size: " + std::to_string(total_size)).c_str());
    return false;

}

bool Gemma::createFile() {

    if (!this->_is_open) {
        logger(3, "Tried to create file when no file was open");
        return false;
    }

    std::fstream file(
        this->_path, std::fstream::out | std::fstream::binary | std::fstream::trunc
    );
    if ( !file.is_open() ) {
        logger(3, ("Can't open file at " + this->_path + " for creation").c_str());
        file.close();
        return false;
    }
    logger(0, ("Opened file " + this->_path + " for creation").c_str());

    // -- Header --
    file.write("ᓚᘏᗢ", 9);
    u_int64_t v = VERSION_NUMBER;
    file.write(reinterpret_cast<char*>(&v), 8);
    logger(0, "Wrote header");

    // -- Size --
    u_int32_t meta_size = 8 + 7 * 4 + 1;
    u_int32_t owner_size = this->_owner.size();
    logger(0, ("Owner size: " + std::to_string(owner_size)).c_str());
    u_int32_t title_size = this->_title.size();
    logger(0, ("Title size: " + std::to_string(title_size)).c_str());
    u_int32_t desc_size = this->_description.size();
    logger(0, ("Desc size: " + std::to_string(desc_size)).c_str());
    u_int32_t url_size = this->_url.size();
    logger(0, ("URL size: " + std::to_string(url_size)).c_str());
    u_int32_t cover_size = this->_cover.size();
    logger(0, ("Cover size: " + std::to_string(cover_size)).c_str());

    std::string fields = getFields();
    u_int32_t fields_size = fields.size();
    logger(0, ("Fields size: " + std::to_string(fields_size)).c_str());

    meta_size += owner_size + title_size + desc_size + url_size + cover_size +
                    fields_size;
    logger(0, ("Metadata size: " + std::to_string(meta_size)).c_str());

    file.write(reinterpret_cast<char*>(&meta_size), 4);
    logger(0, "Wrote metadata size at SOM");

    // -- Owner --
    file.write(reinterpret_cast<char*>(&owner_size), 4);
    file.write(this->_owner.data(), owner_size);
    logger(0, "Wrote owner");

    // -- Title --
    file.write(reinterpret_cast<char*>(&title_size), 4);
    file.write(this->_title.data(), title_size);
    logger(0, "Wrote title");

    // -- Description --
    file.write(reinterpret_cast<char*>(&desc_size), 4);
    file.write(this->_description.data(), desc_size);
    logger(0, "Wrote desc");

    // -- URL --
    file.write(reinterpret_cast<char*>(&url_size), 4);
    file.write(this->_url.data(), url_size);
    logger(0, "Wrote url");

    // -- Cover --
    file.write(reinterpret_cast<char*>(&cover_size), 4);
    file.write(this->_cover.data(), cover_size);
    logger(0, "Wrote cover");

    // -- Fields --
    file.write(reinterpret_cast<char*>(&fields_size), 4);
    file.write(fields.data(), fields_size);
    logger(0, "Wrote fields");

    // -- Creation --
    file.write(reinterpret_cast<char*>(&this->_creation), 4);
    logger(0, "Wrote creation");

    // -- Compression --
    file.write(reinterpret_cast<char*>(&this->_compression), 1);
    logger(0, "Wrote compression");

    return true;

}

void Gemma::setOwner( std::string s ) {
    this->_owner = s;
}

void Gemma::setCreation( int i ) {
    this->_creation = i;
}

void Gemma::setCompression( u_int8_t i ) {
    this->_compression = i;
}

void Gemma::setCover( std::string s ) {
    this->_cover = s;
}

void Gemma::setDescription( std::string s ) {
    this->_description = s;
}

void Gemma::setUrl( std::string s ) {
    this->_url = s;
}

void Gemma::addField( std::string s ) {
    this->_fields.push_back(s);
}

void Gemma::setTitle( std::string s ) {
    this->_title = s;
}

std::string Gemma::getFields() {
    std::string fields;
    for (std::string s: this->_fields) {
        fields += s + ';';
        printf("%s\n", s.c_str());
    }
    fields.erase(fields.size() - 1, 1);
    return fields;
}

std::string Gemma::toString() {
    return "# " + this->_title + "\n" +
           "- Owner: " + this->_owner + "\n" +
           "- Title: " + this->_title + "\n" +
           "- Description: " + this->_description + "\n" +
           "- URL: " + this->_url + "\n" +
           "- Cover: " + this->_cover + "\n" +
           "- Fields: " + this->getFields() + "\n" +
           "- Compression: " + std::to_string(this->_compression) + "\n";
}

bool Gemma::appendEntry( std::vector<std::string> e ) {

    if (!this->_is_open) {
        logger(3, "Tried to append to file when no file was open");
        return false;
    }

    if ( e.size() > this->_fields.size() ) {
        logger(3, "Tried to insert more data than there is field");
        return false;
    }

    std::fstream file(
        this->_path, std::fstream::out | std::fstream::binary | std::fstream::app
    );
    if ( !file.is_open() ) {
        logger(3, ("Can't open file at " + this->_path + " to append entry").c_str());
        file.close();
        return false;
    }
    logger(0, ("Opened file " + this->_path + " to appended entry").c_str());

    // -- Calculate size --
    u_int32_t entry_size = 4;
    for (std::string s: e) entry_size += s.size() + 4;

    file.write(reinterpret_cast<char*>(&entry_size), 4);
    logger(0, "Wrote entry size");

    // -- Insert data --
    int i = 0;
    for (std::string s: e) {
        u_int32_t data_size = s.size();
        file.write(reinterpret_cast<char*>(&data_size), 4);
        file.write(s.data(), data_size);
        logger(0, ("Wrote data for field " + this->_fields.at(i)).c_str());
        ++i;
    }

    return true;

}

std::vector<std::string> Gemma::getEntryAtInd( int i ) {

    return this->getEntryAtAdress( this->_getAdressOfId( i ) );

}

std::vector<std::string> Gemma::getEntryAtAdress( u_int64_t i ) {

    std::vector<std::string> result;

    std::fstream file(
        this->_path, std::fstream::out | std::fstream::binary | std::fstream::app
    );
    if ( !file.is_open() ) {
        logger(3, ("Can't open file at " + this->_path + " to get entry").c_str());
        file.close();
        return result;
    }
    logger(0, ("Opened file " + this->_path + " to get entry").c_str());

    // -- goto entry --
    file.seekg(i);

    // -- Get size --
    u_int32_t entry_size = 0;
    file.read(reinterpret_cast<char*>(&entry_size), 4);
    if ( !file ) {
        logger(3, ("Error reading size of entry " + std::to_string(i)).c_str());
        return result;
    }

    // -- Read attributes --
    int ind = 0;
    while ( entry_size > 0 ) {
        u_int32_t attribute_size = 0;
        file.read(reinterpret_cast<char*>(&attribute_size), 4);
        if ( !file ) {
            logger(3, ("Error reading size of att " + this->_fields.at(ind) + " for entry " + std::to_string(i)).c_str());
            return result;
        }
        entry_size -= attribute_size;

        std::string attribute = std::string(attribute_size, '\0');
        file.read(attribute.data(), attribute_size);
        if ( !file ) {
            logger(3, ("Error reading att " + this->_fields.at(ind) + " for entry " + std::to_string(i)).c_str());
            return result;
        }

        result.push_back(attribute);

        ++ind;
    }

    return result;

}

bool Gemma::buildMap() {

    std::fstream file(
        this->_path, std::fstream::out | std::fstream::binary | std::fstream::app
    );
    if ( !file.is_open() ) {
        logger(3, ("Can't open file at " + this->_path + " to build map").c_str());
        file.close();
        return false;
    }
    logger(0, ("Opened file " + this->_path + " to build map").c_str());

    // -- Skip metadata --
    file.seekg(17);

    u_int32_t meta_size = 0;
    u_int32_t total_size = 8 + 7 * 4 + 1; // meta size x 2
    file.read(reinterpret_cast<char*>(&meta_size), 4);
    if ( !file ) {
        logger(3, "Error reading metadata size at SOM");
        return false;
    }

    // MEOW

    return true;

}
