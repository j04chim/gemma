#include <string>
#include <sys/types.h>
#include <vector>

#define VERSION_NUMBER 1


class Gemma {

    public:

        Gemma() = default;
        Gemma( std::string );
        bool open( std::string );
        bool createFile();
        std::string toString();
        bool buildMap();
        int getFieldInd( std::string );

        void setOwner( std::string );
        void setCreation( int );
        void setCompression( u_int8_t );
        void setCover( std::string );
        void setDescription( std::string );
        void setUrl( std::string );
        void addField( std::string );
        void setTitle( std::string );
        std::string getFields();

        int size();
        bool appendEntry( std::vector<std::string> );
        std::vector<std::string> getEntryAtInd( int );
        std::vector<std::string> getEntryAtAdress( u_int64_t );
        std::vector<std::string> getEntryWithField( int, std::string );
        std::vector<std::string> getEntryWithField( std::string, std::string );

    private:

        bool _readMetadata();
        u_int64_t _getAdressOfId( int );

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