/**
 * MIT License
 * Copyright (c) 2026 Joachim Rey
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <string>
#include <sys/types.h>
#include <vector>

#define VERSION_NUMBER 1

/**
 * @author Joachim Rey <joachim@meowf.fr>
 * @class Gemma
 * This Object manage a Gemma file. It gives higher level access to its
 * content while still being fairly barebone.
 *
 * You can open an existing file with:
 * Gemma g("file.gemma");
 * g.open();
 *
 * You can create a new Gemma file with:
 * Gemma g("file.gemma");
 * g.setOwner( "Name" );
 * g.createFile();
 *
 * Entries and metadata can't be modified after file creation. If you
 * want to modify something, you have to create a new file and copy
 * the modified content in it.
 *
 * You might want to use buildMap() to reference the file content before
 * using it.
 *
 * The file can contain a maximum of 2^64 and each entry has a maximum size of 2^32.
 */
class Gemma {

    public:

        /**
         * @fn Gemma
         * @brief Gemma contructor.
         * @param std::string Path to the file to create/open.
         */
        Gemma( std::string );

        /**
         * @fn setOwner
         * @brief Set the owner value in the object.
         * @param std::string New value.
         */
        void setOwner( std::string );

        /**
         * @fn setCreation
         * @brief Set the creation date value in the object.
         * @param u_int32_t New value.
         */
        void setCreation( u_int32_t );

        /**
         * @fn setCompression
         * @brief Set the compression value in the object.
         * @param u_int8_t New value.
         */
        void setCompression( u_int8_t );

        /**
         * @fn setCover
         * @brief Set the cover value in the object.
         * @param std::string New value.
         */
        void setCover( std::string );

        /**
         * @fn setDescription
         * @brief Set the description value in the object.
         * @param std::string New value.
         */
        void setDescription( std::string );

        /**
         * @fn setUrl
         * @brief Set the URL value in the object.
         * @param std::string New value.
         */
        void setUrl( std::string );

        /**
         * @fn addField
         * @brief Append a field in the object.
         * @param std::string Value to append.
         */
        void addField( std::string );

        /**
         * @fn setTitle
         * @brief Set the title value in the object.
         * @param std::string New value.
         */
        void setTitle( std::string );

        /**
         * @fn getFields
         * @brief Return the fields of the object.
         */
        std::vector<std::string> getFields();

        /**
         * @fn getTitle
         * @brief Return the title of the object.
         */
        std::string getTitle();

        /**
         * @fn getOwner
         * @brief Return the owner of the object.
         */
        std::string getOwner();

        /**
         * @fn getPath
         * @brief Return the path of the object.
         */
        std::string getPath();

        /**
         * @fn getCreation
         * @brief Return the creation date of the object.
         */
        u_int32_t getCreation();

        /**
         * @fn getDescription
         * @brief Return the description of the object.
         */
        std::string getDescription();

        /**
         * @fn getUrl
         * @brief Return the URL of the object.
         */
        std::string getUrl();

        /**
         * @fn getCompression
         * @brief Return the compression of the object.
         */
        u_int8_t getCompression();

        /**
         * @fn getCover
         * @brief Return the cover of the object.
         */
        std::string getCover();

        /**
         * @fn open
         * @brief Check the header of the file and load the metadata.
         */
        bool open();

        /**
         * @fn createFile
         * @brief Create/erase the file with the metadata of the object
         */
        bool createFile();

        /**
         * @fn toString
         * @brief Return the object's data as a MD string.
         */
        std::string toString();

        /**
         * @fn buildMap
         * @brief Reference the entries of the file for faster lookup and cache the size
         */
        bool buildMap();

        /**
         * @fn size
         * @brief Return the cached number of entries or calculate it.
         */
        int size();

        /**
         * @fn appendEntry
         * @brief Add a new entry at the end of the file.
         * @param std::vector<std::string> List of values.
         */
        bool appendEntry( std::vector<std::string> );

        /**
         * @fn getEntryAtInd
         * @brief Return the entry at indice n. Use the file map if it exist or fallback on reading the file to find the entry adress.
         * @param int Entry index.
         */
        std::vector<std::string> getEntryAtInd( int );

        /**
         * @fn getEntryAtAdress
         * @brief Return the entry at the given adress in the file.
         * @param u_int64_t Adress in the file.
         */
        std::vector<std::string> getEntryAtAdress( u_int64_t );

        /**
         * @fn getEntriesLike
         * @brief Return all the entries which field at the given index match the given RegEx rule.
         * @param int Index in field list.
         * @param std::string RegEx rule to match.
         */
        std::vector<std::vector<std::string>> getEntriesLike( int, std::string );

    private:

        /**
         * @fn _readMetadata
         * @brief Load the metadata of the file.
         */
        bool _readMetadata();

        /**
         * @fn _getAdressOfId
         * @brief Return the adress of an entry from either the file map or by reading the file.
         */
        u_int64_t _getAdressOfId( int );

        /**
         * @fn _getFormatedFields
         * @brief Return the fields of the object CSV style.
         */
        std::string _getFormatedFields();

        std::string _path;
        std::string _owner;
        u_int32_t _creation;
        std::string _title;
        std::string _description;
        std::string _url;
        u_int8_t _compression;
        std::string _cover;
        std::vector<std::string> _fields;
        std::vector<u_int64_t> _map;
        bool _is_open = false;
        int _size = -1;

};
