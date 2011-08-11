#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>

const int num_keywords = 10;
namespace keyword
	{ enum { temp = 0,
		 leftParens,
		 rightParens,
		 leftClammer,
		 rightClammer,
		 equal,
		 notEqual,
		 lessSign,
		 greaterSign,
		 leadsTo,
		 not_a_keyword = num_keywords }; }
std::string keywords[ num_keywords ] = { "temp",
				   	 "(",
				   	 ")",
				   	 "[",
				   	 "]",
				   	 "=",
				   	 "!=",
				   	 "<",
				   	 ">",
				   	 "->" };

struct Statement
{
	Statement() : connection(0) {}
	Statement( std::vector<int>& _left, int _connection, std::vector<int>& _right )
		: connection( _connection )
	{
		for( int i(0); i < left.size(); i++ )
			left.push_back( _left[i] );
		for( int i(0); i < right.size(); i++ )
			right.push_back( _right[i] );
	}
	std::vector< int > left, right;
	int connection;
};
std::vector< int > pos_arguments;
std::vector< Statement > statements;

std::string Trim( std::string input );
std::string ToString( std::vector<int>& in );
std::vector<int> Parse( std::string input );
std::vector<int> Run( std::vector<int>& input ) throw (std::string);
std::vector<int> Calculate( std::vector<int>& input ) throw(std::string);
int main()
{
	std::string input = "";
	do
	{
		std::string line;
		getline( std::cin, line );
		if( !line.size() )
			continue;
		line = Trim( line );
		if( line.size() == 0 )
			continue;
		if( line == "quit" )
			break;

		input.append( Trim( " " + line ) );

		if( input.size() < 2 || input.compare( input.size() - 2, 2, "++" ) != 0 )
		{
			try {
				std::cout << ToString( Run( Parse( input ) ) );
			} catch( std::string error ) {
				std::cout << "Error: " << error << std::endl;
			}
			input.clear();
		}
		else
			input.erase( input.size() - 2 );
	}
	while( true );
}

std::string Trim( std::string input )
{
	// Remove comments
	for( int i(0); i < input.size() - 1; i++ )
		if( input.compare( i+1, 2, "--" ) == 0 )
			input.erase( i+1 );

	// Remove extra whitespaces
	for( int i(0); input[i] == ' '; i++ )
		input.erase( i--, 1 );
	for( int i( input.size() - 1 ); input[i] == ' '; i-- )
		input.erase( i, 1 );
	for( int i(0); i < input.size() - 1; i++ )
		if( input[i] == ' ' && input[i+1] == ' ' )
			input.erase( i--, 1 );

	return input;
}

std::string ToString( std::vector<int>& in )
{
	std::string result;
	for( int i(0); i < in.size(); i++ )
		if( in[i] == keyword::not_a_keyword ) {
			for( int l(i+2); l < i+2 + in[i+1]; l++ )
				result.append( 1, (char)in[l] );
			i += 2 + in[i+1];
		} else {
			result.append( keywords[i] );
		}
	if( in.size() != 0 )
		result.append( 1, '\n' );
	return result;
}

std::vector<int> Parse( std::string input )
{
	std::vector< std::string > words;
	for( int i(0); i < input.size(); i++ )
		if( input[i] == ' ' )
		{
			words.push_back( input.substr( 0, i ) );
			input.erase( 0, i + 1 );
			i = -1;
		}
		else if( i == input.size() - 1 )
			words.push_back( input.substr( 0, i + 1 ) );
	
	std::vector< int > result;
	for( int i(0); i < words.size(); i++ )
	{
		bool found = false;
		for( int l(0); l < num_keywords; l++ )
			if( words[i].compare( keywords[l] ) == 0 )
			{
				// Add the numerical representation of the keyword to result
				result.push_back( l );
				found = true;
				break;
			}
		if( !found )
		{
			// Not a keyword
			result.push_back( keyword::not_a_keyword );
			result.push_back( words[i].size() );
			for( int k(0); k < words[i].size(); k++ )
				result.push_back( (int)words[i][k] );
		}
	}
	return result;
}

std::vector<int> Run( std::vector<int>& input ) throw (std::string)
{
	// If its a statement add new statement
	// else calculate the answer
	bool IsStatement = false;
	std::vector<int> empty;
	Statement statement;
	for( int i(0); i < input.size(); i++ )
	{
		int deapth = 0;
		switch( input[ i ] )
		{
			case keyword::equal:
			case keyword::notEqual:
			case keyword::lessSign:
			case keyword::greaterSign:
			case keyword::leadsTo:
				if( deapth )
					break;
				// Its a statement construct and add new statement
				IsStatement = true;
				statement.connection = input[i];
				for( int l(0); l < i; l++ )
					statement.left.push_back( input[ l ] );
				for( int l(i+1); l < input.size(); l++ )
					statement.right.push_back( input[ l ] );
				i = input.size();
				statements.push_back( statement );
	
				return empty;
			case keyword::not_a_keyword:
				i = i + input[ i + 1 ] + 1;
				break;
			case keyword::leftParens:
			case keyword::leftClammer:
				++deapth;
				break;
			case keyword::rightParens:
				// Search for matching keyword
				for( int l(i); l > 0; --l )
				{
					if( input[ l ] == keyword::leftParens ) {
						// Found matching parens
						break;
					} else if( input[ l ] == keyword::leftClammer ) {
						// If incompatible keyword found throw error
						throw std::string( "Incorrect matching bracket" );
					} else if ( l == 0 ) {
						// If not found throw error
						throw std::string( "No matching bracket" );
					}
				}
				--deapth;
				break;
			case keyword::rightClammer:
				// Search for matching keyword
				for( int l(i); l > 0; --l )
				{
					if( input[ l ] == keyword::leftClammer ) {
						// Found matching clammer
						break;
					} else if( input[ l ] == keyword::leftParens ) {
						// If incompatible keyword found throw error
						throw std::string( "Incorrect matching bracket" );
					} else if ( l == 0 ) {
						// If not found throw error
						throw std::string( "No matching bracket" );
					}
				}
				--deapth;
				break;
		}
		if( i == input.size() - 1 )
			throw std::string( "No connection in statement" );
	}
	if( !IsStatement ) {
		// Answer the question
		return Calculate( input );
	}
}

std::vector<int> Calculate( std::vector<int>& input ) throw(std::string)
{
	std::vector<int> result;
	for( int i(0); i < input.size(); i++ ) {
		switch( input[i] )
		{
			case keyword::temp:
				break;
			case keyword::leftParens:
				break;
			case keyword::leftClammer:
				break;
			case keyword::rightParens:
				// Search for matching keyword
				for( int l(i); l > 0; --l )
				{
					if( input[ l ] == keyword::leftParens ) {
						// Calculate results
						// Insert them into the vector
					} else if( input[ l ] == keyword::leftClammer ) {
						// If incompatible keyword found throw error
						throw std::string( "Incorrect matching bracket" );
					} else if ( l == 0 ) {
						// If not found throw error
						throw std::string( "No matching bracket" );
					}
				}
				break;
			case keyword::rightClammer:
				// Search for matching keyword
				for( int l(i); l > 0; --l )
				{
					if( input[ l ] == keyword::leftClammer ) {
						// Calculate results
						// Insert them into the vector
					} else if( input[ l ] == keyword::leftParens ) {
						// If incompatible keyword found throw error
						throw std::string( "Incorrect matching bracket" );
					} else if ( l == 0 ) {
						// If not found throw error
						throw std::string( "No matching bracket" );
					}
				}
				break;
			case keyword::equal:
				break;
			case keyword::notEqual:
				break;
			case keyword::lessSign:
				break;
			case keyword::greaterSign:
				break;
			case keyword::leadsTo:
				break;
			case keyword::not_a_keyword:
				// Search to find what it is
				i = i + input[ i + 1 ] + 1;
				break;
			default:
				throw std::string( "Bug in interprenter, keyword: " 
					      + keywords[ input[i] ] 
					      + " doesn't exists" );
				break;
		}
	}
	return result;
}

