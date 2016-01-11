#pragma once

#include <map>

template<class K, class R, class Compare = std::less<K>>
class ResourceManager
{
protected:
    std::map<K, std::pair<R*, uint32_t>, Compare> _resources;
    virtual R* loadResource( const K& key ) = 0;
    
public:
    virtual ~ResourceManager() 
    {
        for(auto iterator = _resources.begin(); iterator != _resources.end(); ++iterator) {
            delete iterator->second.first;
        }
    }
    
    virtual R* request( const K& key )
    {    
        auto search = _resources.find( key );
        if( search == _resources.end() )
        {
            std::pair<R*, uint32_t> c;
            c.first = loadResource( key );
            c.second = 1;
            _resources.insert( std::pair<K, std::pair<R*, uint32_t>>(key, c) );
            return c.first;
        }
        else
        {
            std::pair<R*, uint32_t> c = search->second;
            ++search->second.second;
            return c.first;
        }
    }
    
    virtual void release( R* val )
    {    
        for(auto iterator = _resources.begin(); iterator != _resources.end(); ++iterator) {
            if( iterator->second.first == val )
            {
                iterator->second.second--;
                if( iterator->second.second == 0 )
                {
                    delete iterator->second.first;
                    _resources.erase( iterator );
                }
                break;
            }
        }
    }
    
    virtual void release( const K& key )
    {    
        for(auto iterator = _resources.begin(); iterator != _resources.end(); ++iterator) {
            if( iterator->first == key )
            {
                iterator->second.second--;
                if( iterator->second.second == 0 )
                {
                    delete iterator->second.first;
                    _resources.erase( iterator );
                }
                break;
            }
        }
    }
};