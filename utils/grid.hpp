#pragma once

#include <fstream>
#include <vector>


template < typename TType >
class Grid
{
public:
    Grid( std::size_t width, std::size_t height, TType const& value );

    Grid( std::size_t width, std::size_t height, std::vector< TType > values );

    std::size_t getWidth() const;

    std::size_t getHeight() const;

    TType const& getValue( std::size_t x, std::size_t y ) const;

    void setValue( std::size_t x, std::size_t y, TType value );

private:
    std::size_t m_width;
    std::size_t m_height;
    std::vector< TType > m_values;
};


Grid< char > readGrid( std::istream& stream );


template < typename TType >
inline Grid< TType >::Grid( std::size_t width, std::size_t height, TType const& value )
    : m_width{ width }, m_height{ height }, m_values( m_width * m_height, value )
{
}

template < typename TType >
inline Grid< TType >::Grid( std::size_t width, std::size_t height, std::vector< TType > values )
    : m_width{ width }, m_height{ height }, m_values( std::move( values ) )
{
}

template < typename TType >
inline std::size_t Grid< TType >::getWidth() const
{
    return m_width;
}

template < typename TType >
inline std::size_t Grid< TType >::getHeight() const
{
    return m_height;
}

template < typename TType >
TType const& Grid< TType >::getValue( std::size_t x, std::size_t y ) const
{
    return m_values[ x + y * m_width ];
}

template < typename TType >
void Grid< TType >::setValue( std::size_t x, std::size_t y, TType value )
{
    m_values[ x + y * m_width ] = std::move( value );
}
