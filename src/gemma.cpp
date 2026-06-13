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

    file.close();

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
    file.seekg(17, std::fstream::beg);

    u_int32_t meta_size = 0;
    u_int32_t total_size = 4 + 7 * 4 + 1; // meta size x 2
    file.read(reinterpret_cast<char*>(&meta_size), 4);
    if ( !file ) {
        logger(3, "Error reading metadata size at SOM");
        file.close();
        return false;
    }

    // -- Owner --
    u_int32_t owner_size;
    file.read(reinterpret_cast<char*>(&owner_size), 4);
    if ( !file ) {
        logger(3, "Error reading owner size");
        file.close();
        return false;
    }
    total_size += owner_size;

    this->_owner = std::string(owner_size, '\0');
    file.read(this->_owner.data(), owner_size);
    if ( !file ) {
        logger(3, "Error reading owner data");
        file.close();
        return false;
    }

    // -- Title --
    u_int32_t title_size;
    file.read(reinterpret_cast<char*>(&title_size), 4);
    if ( !file ) {
        logger(3, "Error reading title size");
        file.close();
        return false;
    }
    total_size += title_size;

    this->_title = std::string(title_size, '\0');
    file.read(this->_title.data(), title_size);
    if ( !file ) {
        logger(3, "Error reading title data");
        file.close();
        return false;
    }

    // -- Description --
    u_int32_t desc_size;
    file.read(reinterpret_cast<char*>(&desc_size), 4);
    if ( !file ) {
        logger(3, "Error reading description size");
        file.close();
        return false;
    }
    total_size += desc_size;

    this->_description = std::string(desc_size, '\0');
    file.read(this->_description.data(), desc_size);
    if ( !file ) {
        logger(3, "Error reading description data");
        file.close();
        return false;
    }

    // -- URL --
    u_int32_t url_size;
    file.read(reinterpret_cast<char*>(&url_size), 4);
    if ( !file ) {
        logger(3, "Error reading url size");
        file.close();
        return false;
    }
    total_size += url_size;

    this->_url = std::string(url_size, '\0');
    file.read(this->_url.data(), url_size);
    if ( !file ) {
        logger(3, "Error reading url data");
        file.close();
        return false;
    }

    // -- Cover --
    u_int32_t cover_size;
    file.read(reinterpret_cast<char*>(&cover_size), 4);
    if ( !file ) {
        logger(3, "Error reading cover size");
        file.close();
        return false;
    }
    total_size += cover_size;

    this->_cover = std::string(cover_size, '\0');
    file.read(this->_cover.data(), cover_size);
    if ( !file ) {
        logger(3, "Error reading cover data");
        file.close();
        return false;
    }

    // -- Fields --
    u_int32_t fields_size;
    file.read(reinterpret_cast<char*>(&fields_size), 4);
    if ( !file ) {
        logger(3, "Error reading fields size");
        file.close();
        return false;
    }
    total_size += fields_size;

    std::string fields = std::string(fields_size, '\0');
    file.read(fields.data(), fields_size);
    if ( !file ) {
        logger(3, "Error reading fields data");
        file.close();
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
        file.close();
        return false;
    }

    // -- Compression --
    file.read(reinterpret_cast<char*>(&this->_compression), 1);
    if ( !file ) {
        logger(3, "Error reading compression type");
        file.close();
        return false;
    }

    file.close();

    // Size check
    if ( meta_size == total_size ) return true;
    logger(3, ("Wrong metadata size, SOM: " + std::to_string(meta_size) +
                ", Total size: " + std::to_string(total_size)).c_str());
    return false;

}

bool Gemma::createFile() {

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
    u_int32_t meta_size = 8 * 4 + 1;
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

    file.sync();
    file.close();
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

    if ( e.size() != this->_fields.size() ) {
        logger(3, "Number of data to append is different of number of field defined in metadata");
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

    file.sync();
    file.close();
    return true;

}

std::vector<std::string> Gemma::getEntryAtInd( int i ) {

    return this->getEntryAtAdress( this->_getAdressOfId( i ) );

}

std::vector<std::string> Gemma::getEntryAtAdress( u_int64_t i ) {

    std::vector<std::string> result;

    if (!this->_is_open) {
        logger(3, "Tried to get entry at adress x when no file was open");
        return result;
    }

    std::fstream file(
        this->_path, std::fstream::in | std::fstream::binary
    );
    if ( !file.is_open() ) {
        logger(3, ("Can't open file at " + this->_path + " to get entry").c_str());
        file.close();
        return result;
    }
    logger(0, ("Opened file " + this->_path + " to get entry").c_str());

    file.seekg(17, std::fstream::beg);
    u_int32_t meta_size = 0;
    file.read(reinterpret_cast<char*>(&meta_size), 4);
    if ( !file ) {
        logger(3, "Error reading metadata size at SOM");
        file.close();
        return result;
    }

    if ( meta_size + 17 > i ) {
        logger(3, ("Entry adress too low: " + std::to_string(meta_size + 17)).c_str());
        file.close();
        return result;
    }

    // -- goto entry --
    file.seekg(i, std::fstream::beg);

    // -- Get size --
    int64_t entry_size = 0;
    file.read(reinterpret_cast<char*>(&entry_size), 4);
    if ( !file ) {
        logger(3, ("Error reading size of entry " + std::to_string(i)).c_str());
        file.close();
        return result;
    }

    // -- Read attributes --
    entry_size -= 4;
    int ind = 0;
    while ( entry_size > 0 ) {
        u_int32_t attribute_size = 0;
        file.read(reinterpret_cast<char*>(&attribute_size), 4);
        if ( !file ) {
            logger(3, ("Error reading size of att " + this->_fields.at(ind) + " for entry " + std::to_string(i)).c_str());
            file.close();
            return result;
        }
        entry_size -= attribute_size + 4;

        std::string attribute = std::string(attribute_size, '\0');
        file.read(attribute.data(), attribute_size);
        if ( !file ) {
            logger(3, ("Error reading att " + this->_fields.at(ind) + " for entry " + std::to_string(i)).c_str());
            file.close();
            return result;
        }

        result.push_back(attribute);

        ++ind;
    }

    file.close();
    return result;

}

bool Gemma::buildMap() {

    if (!this->_is_open) {
        logger(3, "Tried to build map when no file was open");
        return false;
    }

    std::fstream file(
        this->_path, std::fstream::in | std::fstream::binary
    );
    if ( !file.is_open() ) {
        logger(3, ("Can't open file at " + this->_path + " to build map").c_str());
        file.close();
        return false;
    }
    logger(0, ("Opened file " + this->_path + " to build map").c_str());

    // -- Skip metadata --
    file.seekg(17, std::fstream::beg);

    u_int32_t meta_size = 0;
    file.read(reinterpret_cast<char*>(&meta_size), 4);
    if ( !file ) {
        logger(3, "Error reading metadata size at SOM");
        file.close();
        return false;
    }
    file.seekg(meta_size + 17, std::fstream::beg);

    // -- reference all entries --
    u_int64_t position = 17 + meta_size;
    while ( file.peek() != EOF ) {

        u_int32_t entry_size = 0;
        file.read(reinterpret_cast<char*>(&entry_size), 4);
        if ( !file ) {
            logger(3, ("Error reading entry size at " + std::to_string(position)).c_str());
            file.close();
            return false;
        }

        logger(0, ("Found entry at " + std::to_string(entry_size)).c_str());

        this->_map.push_back(position);
        position += entry_size;

        file.seekg(position, std::fstream::beg);

    }

    this->_size = this->_map.size();

    file.close();
    return true;

}

u_int64_t Gemma::_getAdressOfId( int i ) {

    if (!this->_is_open) {
        logger(3, "Tried to get adress of indice when no file was open");
        return false;
    }

    if (i < 0) {
        logger(3, "Entry index must be positive!");
        return 0;
    }

    if (this->_map.size() > 0 && this->_map.size() > i)
        return this->_map.at(i);

    logger(1, "File map does not seem to have been created, falling back to manual entry search");

    std::fstream file(
        this->_path, std::fstream::in | std::fstream::binary
    );
    if ( !file.is_open() ) {
        logger(3, ("Can't open file at " + this->_path + " to build map").c_str());
        file.close();
        return 0;
    }
    logger(0, ("Opened file " + this->_path + " to build map").c_str());

    // -- Skip metadata --
    file.seekg(17, std::fstream::beg);

    u_int32_t meta_size = 0;
    file.read(reinterpret_cast<char*>(&meta_size), 4);
    if ( !file ) {
        logger(3, "Error reading metadata size at SOM");
        file.close();
        return 0;
    }
    file.seekg(meta_size + 17, std::fstream::beg);

    // -- Jump until correct entry --
    u_int64_t position = 17 + meta_size;
    int indice = 0;
    while ( file.peek() != file.eof() ) {

        u_int32_t entry_size = 0;
        file.read(reinterpret_cast<char*>(&entry_size), 4);
        if ( !file ) {
            logger(3, ("Error reading entry size at " + std::to_string(position)).c_str());
            file.close();
            return false;
        }

        this->_map.push_back(position);
        position += entry_size;

        file.seekg(position, std::fstream::beg);

        if (indice == i) return position;

        ++indice;

    }

    file.close();
    return 0;

}

int Gemma::size() {

    if (!this->_is_open) {
        logger(3, "Tried to get size when no file was open");
        return 0;
    }

    if ( this->_size != -1 ) return this->_size;

    std::fstream file(
        this->_path, std::fstream::in | std::fstream::binary
    );
    if ( !file.is_open() ) {
        logger(3, ("Can't open file at " + this->_path + " to get size").c_str());
        file.close();
        return 0;
    }
    logger(0, ("Opened file " + this->_path + " to get size").c_str());

    // -- Skip metadata --
    file.seekg(17, std::fstream::beg);

    u_int32_t meta_size = 0;
    file.read(reinterpret_cast<char*>(&meta_size), 4);
    if ( !file ) {
        logger(3, "Error reading metadata size at SOM");
        file.close();
        return 0;
    }
    file.seekg(meta_size + 17, std::fstream::beg);

    // -- reference all entries --
    u_int64_t position = 17 + meta_size;
    int entry_nb = 0;
    while ( file.peek() != EOF) {

        u_int32_t entry_size = 0;
        file.read(reinterpret_cast<char*>(&entry_size), 4);
        if ( !file ) {
            logger(3, ("Error reading entry size at " + std::to_string(position)).c_str());
            file.close();
            return false;
        }

        position += entry_size;
        ++entry_nb;

        file.seekg(position, std::fstream::beg);

    }

    file.close();
    return entry_nb;

}
