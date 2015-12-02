#include <internal/config.h>
#include <iostream>

#include <unistd.h>

Config::Config()
{
    _keyFile = g_key_file_new();
}

Config::~Config()
{
    g_key_file_free ( _keyFile );
}

bool Config::loadFile ( const std::string& path )
{
    if ( !g_key_file_load_from_file ( _keyFile, path.c_str(), G_KEY_FILE_NONE, &_error ) )
    {
        g_error ( _error->message );
        return false;
    }
    return true;
}

void Config::setGroup ( const std::string& str )
{
    _curGrp = str;
}

std::string Config::getString ( const std::string& name )
{
    std::string res = g_key_file_get_string ( _keyFile, _curGrp.c_str(), name.c_str(), &_error );
    if ( !_error )
    {
        return res;
    }

    std::cerr << _error->message << std::endl;
    return "";
}

bool Config::getBool( const std::string& name )
{

    bool res = g_key_file_get_boolean ( _keyFile, _curGrp.c_str(), name.c_str(), &_error );
    if ( !_error )
    {
        return res;
    }

    std::cerr << _error->message << std::endl;
    return false;;
}

double Config::getDouble( const std::string& name )
{
    double res = g_key_file_get_double ( _keyFile, _curGrp.c_str(), name.c_str(), &_error );
    if ( !_error )
    {
        return res;
    }

    std::cerr << _error->message << std::endl;
    return 0;
}

int Config::getInt ( const std::string& name )
{
    int res = g_key_file_get_integer ( _keyFile, _curGrp.c_str(), name.c_str(), &_error );
    if ( !_error )
    {
        return res;
    }

    std::cerr << _error->message << std::endl;
    return 0;
}

