template< typename T >
void Helium::Tokenize( const tstring& str, std::vector< T >& tokens, const tstring delimiters )
{
    tregex splitPattern(delimiters); 

    std::tr1::sregex_token_iterator i(str.begin(), str.end(), splitPattern, -1); 
    std::tr1::sregex_token_iterator end; 

    for(; i != end; i++)
    {
        if( (*i).matched)
        {
            T temp; 
            tstringstream inStream(*i); 
            inStream >> temp; 

            tokens.push_back(temp);
        }
    }
}

template< typename T >
void Helium::Tokenize( const tstring& str, std::set< T >& tokens, const tstring delimiters )
{
    std::tr1::regex splitPattern(delimiters); 

    std::tr1::sregex_token_iterator i(str.begin(), str.end(), splitPattern, -1); 
    std::tr1::sregex_token_iterator end; 

    for(; i != end; i++)
    {
        if( (*i).matched)
        {
            T temp; 
            tstringstream inStream(*i); 
            inStream >> temp; 

            tokens.push_back(temp);
        }
    }
}

template<>
inline void Helium::Tokenize( const tstring& str, std::vector< tstring >& tokens, const tstring delimiters )
{
    tregex splitPattern(delimiters); 

    tsregex_token_iterator i(str.begin(), str.end(), splitPattern, -1); 
    tsregex_token_iterator end; 

    for(; i != end; i++)
    {
        if( (*i).matched)
        {
            tokens.push_back(*i); 
        }
    }
}

template<>
inline void Helium::Tokenize( const tstring& str, std::set< tstring >& tokens, const tstring delimiters )
{
    tregex splitPattern(delimiters); 

    tsregex_token_iterator i(str.begin(), str.end(), splitPattern, -1); 
    tsregex_token_iterator end; 

    for(; i != end; i++)
    {
        if( (*i).matched )
        {
            tokens.insert( *i ); 
        }
    }
}