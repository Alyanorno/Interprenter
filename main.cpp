#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>

const int num_keywords = 11;
namespace keyword { 		  enum {
					 temp = 0,
					 leftParens,
					 rightParens,
					 leftClammer,
					 rightClammer,
					 equal,
					 notEqual,
					 superEqual,
					 lessSign,
					 greaterSign,
					 leadsTo,
					 plus,
					 minus,
					 multiply,
					 divide,
					 not_a_keyword = num_keywords,
				       }; }
std::string keywords[ num_keywords ] = {
					 "temp",
				   	 "(",
				   	 ")",
				   	 "[",
				   	 "]",
				   	 "=",
				   	 "!=",
					 "==",
				   	 "<",
				   	 ">",
				   	 "->",
					 "+",
					 "-",
					 "*",
					 "/",
				       };
static int presedens[ num_keywords ] = {
					 15,
					 14,
					 13,
					 12,
					 11,
					 10,
					 9,
					 8,
					 7,
					 6,
					 5,
					 4,
					 3,
					 2,
					 1,
				       };

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
		if( deapth > 0 ) {
			switch( input[ i ] )
			{
				case keyword::leftParens:
				case keyword::leftClammer:
					++deapth;
					break;
				case keyword::rightParens:
				case keyword::rightClammer:
					--deapth;
					for( int l(i); l > 0; --l )
					{
						if( input[ l ] == input[i] ) {
							// Found matching clammer
							break;
						} else if( input[ l ]
							== ( input[i] 
								== keyword::leftParens 
									? keyword::leftClammer 
									: keyword::leftParens ) ) {
							// If incompatible keyword found throw error
							throw std::string( "Incorrect matching bracket" );
						} else if ( l == 0 ) {
							// If not found throw error
							throw std::string( "No matching bracket" );
						}
					}
					break;
				case keyword::not_a_keyword:
					i = i + input[ i + 1 ] + 1;
					break;
				default:
					break;
;
			}
		} else {
			switch( input[ i ] )
			{
				case keyword::equal:
				case keyword::notEqual:
				case keyword::lessSign:
				case keyword::greaterSign:
				case keyword::leadsTo:
					// Its a statement construct and add new statement
					IsStatement = true;
					statement.connection = input[i];
					for( int l(0); l < i; l++ )
						statement.left.push_back( input[ l ] );
					for( int l(i+1); l < input.size(); l++ )
						statement.right.push_back( input[ l ] );

					statements.push_back( statement );
					return empty;
				case keyword::superEqual:
					// Its a statement construct, calculate right side and add new statement
					IsStatement = true;
					statement.connection = input[i];
					for( int l(0); l < i; l++ )
						statement.left.push_back( input[ l ] );

					// Uses empty to temporarly store values
					for( int l(i+1); l < input.size(); l++ )
						empty.push_back( input[ l ] );
					statement.right = Calculate( empty );
					empty.clear();

					statements.push_back( statement );
					return empty;
				case keyword::leftParens:
				case keyword::leftClammer:
					++deapth;
					break;
				case keyword::rightParens:
				case keyword::rightClammer:
					throw std::string( "No matching bracket" );
				case keyword::not_a_keyword:
					i = i + input[ i + 1 ] + 1;
					break;
				default:
					break;
			}
		}
	}
	if( !IsStatement ) {
		// Answer the question
		return Calculate( input );
	}
	throw std::string( "Bug in interprenter, supposedly unreachable code... reached" );
}

std::vector<int> Calculate( std::vector<int>& input ) throw(std::string)
{
	// TO DO: assert that the last and the first is a bracket and of the same type
	std::vector<int> result = input;
	for( int i(1); i < result.size() - 1; i++ )
	{
		// TO DO: check for presedens
		int deapth = 0;
		if( deapth > 0 ) {
			switch( result[i] )
			{
				case keyword::leftParens:
				case keyword::leftClammer:
					++deapth;
					break;
				case keyword::rightParens:
				case keyword::rightClammer:
					// Search for matching bracket 
					for( int l(i-1); l > 0; l-- )
					{
						if( result[ l ] == result[i] ) {
							// Calculate results
							// Insert them into the vector
							std::vector<int> subPart;
							for( int k(l); k < i+1; k++ )
								subPart.push_back( result[k] );
							result.erase( result.begin() + l, result.begin() + i + 1 );
							subPart = Calculate( subPart );
							result.insert( result.begin() + l, subPart.begin(), subPart.end() );
							break;
						} else if( result[ l ]
							== ( result[i] 
								== keyword::leftParens 
									? keyword::leftClammer 
									: keyword::leftParens ) ) {
							// If incompatible keyword found throw error
							throw std::string( "Incorrect matching bracket" );
						} else if ( l == 0 ) {
							// If not found throw error
							throw std::string( "No matching bracket" );
						}
					}
					--deapth;
					break;
				case keyword::not_a_keyword:
					i = i + result[ i + 1 ] + 1;
					break;
				default:
					break;
			}
		} else {
			switch( result[i] )
			{
				case keyword::temp:
					break;
				case keyword::leftParens:
				case keyword::leftClammer:
					++deapth;
					break;
				case keyword::rightParens:
				case keyword::rightClammer:
					throw std::string( "No matching bracket" );
					break;
				case keyword::equal:
				case keyword::notEqual:
				case keyword::superEqual:
				case keyword::lessSign:
				case keyword::greaterSign:
				case keyword::leadsTo:
					// TO DO: Cast error
					break;
				case keyword::plus:
				case keyword::minus:
				case keyword::multiply:
				case keyword::divide:
					// TO DO: Add operator calculations
					break;
				case keyword::not_a_keyword:
					// TO DO: Search to find what it is
					i = i + result[ i + 1 ] + 1;
					break;
				default:
					throw std::string( "Bug in interprenter, keyword: " 
						      + keywords[ result[i] ] 
						      + " doesn't exists" );
			}
		}
	}
	return result;
}

