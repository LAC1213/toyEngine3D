#pragma once

#include <map>

/** Manages shared resources where copying is expensive like GPU objects( shaders, textures, meshdata, etc.).
 *  The minimal implementation needs to provide a loadResource() function.
 */
template<class K, class R, class Compare = std::less<K>>
class ResourceManager
{
protected:
    std::map<K, std::pair<R*, uint32_t>, Compare> _resources;
    /** Gets called when somebody request a resource that can't be found in _resources.
     */
    virtual R* loadResource( const K& key ) = 0;
    
public:
    /** Destructor, deletes all resources which didn't get released.
     */
    virtual ~ResourceManager() 
    {
        for(auto iterator = _resources.begin(); iterator != _resources.end(); ++iterator) {
            delete iterator->second.first;
        }
    }
    
    /** Requests a Resource. If it doesn't exist, create with loadResource().
     * @param key used to identify resource
     */
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
    
    /** Increments usage count for a resource.
     * @param ptr resource to increment usage count
     */
    virtual void take( R* ptr )
    {
        for(auto iterator = _resources.begin(); iterator != _resources.end(); ++iterator) {
            if( iterator->second.first == ptr )
            {
                iterator->second.second++;
                return;
            }
        }
    }
    
    /** Release by Pointer obtained from request().
     * When everybody who requested a resource releases it the memory gets freed with delete.
     * @param ptr from request()
     */
    virtual void release( R* ptr )
    {    
        for(auto iterator = _resources.begin(); iterator != _resources.end(); ++iterator) {
            if( iterator->second.first == ptr )
            {
                iterator->second.second--;
                if( iterator->second.second == 0 )
                {
                    delete ptr;
                    _resources.erase( iterator );
                }
                break;
            }
        }
    }
    
    /** Release by key which you used to call request()
     * When everybody who requested a resource releases it the memory gets freed with delete.
     * @param key
     */
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