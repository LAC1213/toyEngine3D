#ifndef INTERNAL_CONFIG_H
#define INTERNAL_CONFIG_H

#include <string>
#include <map>

#include <glib.h>

class Config
{
public:
    Config();
    ~Config();

    bool loadFile ( const std::string& path );

    void setGroup( const std::string& str );

    bool getBool( const std::string& name ) ;
    std::string getString( const std::string& name ) ;
    double getDouble( const std::string& name ) ;
    int getInt( const std::string& name ) ;

    /*
        void setDefault ( const std::string& name, bool val );
        void setDefault ( const std::string& name, int val );
        void setDefault ( const std::string& name, double val );
        void setDefault ( const std::string& name, std::string val ); */

private:
    /*        value_map<std::string> strings;
            value_map<int> ints;
            value_map<bool> bools;
            value_map<double> doubles; */

    std::string _curGrp;
    GKeyFile * _keyFile;
    GError * _error;
};

#endif //INTERNAL_CONFIG_H
