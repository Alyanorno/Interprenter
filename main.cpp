#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>

const int num_keywords = 24;
namespace keyword { enum {
					temp = 0,
					leftParens,
					rightParens,
					leftClammer,
					rightClammer,
					equal,
					notEqual,
					superEqual,
					less,
					notLess,
					greater,
					notGreater,
					leadsTo,
					plus,
					minus,
					multiply,
					divide,
					where,
					comma,
					io,
					text,
					number,
					nil,
					mu,
					not_a_keyword = num_keywords,
				       }; }
static std::string keywords[ num_keywords ] = {
					"temp",
				   	"(",
				   	")",
				   	"[",
				   	"]",
				   	"=",
				   	"!=",
					"==",
				   	"<",
				   	"!<",
				   	">",
				   	"!>",
				   	"=>",
					"+",
					"-",
					"*",
					"/",
					"|",
					",",
					"io",
					"Text",
					"Number",
					"nil",
					"mu",
				       };
static int presedens[ num_keywords ] = {
					24,
					23,
					22,
					21,
					20,
					19,
					18,
					17,
					16,
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
std::vector<int> Calculate( std::vector<int>& input ) throw (std::string);
bool AddStatement( std::vector<int>& input ) throw (std::string);
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
			// Remove the arguments
			if( pos_arguments.size() > 0 ) {
				statements.erase( statements.begin() + pos_arguments.front(), statements.end() );
				pos_arguments.clear();
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
	if( !AddStatement( input ) ) {
		// Answer the question
		return Calculate( input );
	}
	std::vector<int> empty;
	return empty;
}

bool AddStatement( std::vector<int>& input ) throw (std::string)
{
	std::vector<int> temp;
	Statement statement;
	for( int i(0); i < input.size(); i++ )
	{
		std::vector<int> bracketPos;
		if( bracketPos.size() > 0 ) {
			int pos = bracketPos.front();
			int matchingBracket = input[ pos ];
			switch( input[ i ] )
			{
				case keyword::leftParens:
				case keyword::leftClammer:
					bracketPos.push_back( i );
					break;
				case keyword::rightParens: 
				case keyword::rightClammer:
					if( matchingBracket == input[i] + 1 ) {
						// Matching clammer
						break;
					} else if( matchingBracket
						== ( input[i] + 1
							== keyword::rightParens 
								? keyword::rightClammer 
								: keyword::rightParens ) ) {
						// If incompatible keyword found throw error
						throw std::string( "Incorrect matching bracket" );
					}
					bracketPos.pop_back();
					break;
				case keyword::not_a_keyword:
					i = i + input[ i + 1 ] + 1;
					break;
				default:
					break;
			}
		} else {
			switch( input[ i ] )
			{
				case keyword::equal:
				case keyword::notEqual:
				case keyword::less:
				case keyword::notLess:
				case keyword::greater:
				case keyword::notGreater:
				case keyword::leadsTo:
				case keyword::superEqual:
					// Its a statement construct, add new statement
					statement.connection = input[i];
					for( int l(0); l < i; l++ )
						statement.left.push_back( input[ l ] );

					if( input[ i ] == keyword::superEqual ) {
						// Uses temp to temporarly store values
						for( int l(i+1); l < input.size(); l++ )
							temp.push_back( input[ l ] );
						statement.right = Calculate( temp );
						temp.clear();
					} else {
						for( int l(i+1); l < input.size(); l++ )
							statement.right.push_back( input[ l ] );
					}
	
					// Special case when statement.left = io
					if( statement.left[0] == keyword::io ) {
						std::cout << std::endl << ToString( statement.right ) << std::endl;
					}
					statements.push_back( statement );
					return true;
				case keyword::leftParens:
				case keyword::leftClammer:
					bracketPos.push_back( i );
					break;
				case keyword::rightParens:
				case keyword::rightClammer:
					throw std::string( "No matching bracket" );
				case keyword::comma:
					throw std::string( "Ilegal keyword placement" );
				case keyword::not_a_keyword:
					i = i + input[ i + 1 ] + 1;
					break;
				default:
					break;
			}
		}
	}
	return false;
}

template < class T >
bool IsNumber( std::string in )
{
     std::istringstream stream( in );
     T t;
     stream >> t;
     return !t.fail();
}

template < class T >
float ToNumber( std::string in )
{
     std::istringstream stream( in );
     T t;
     stream >> t;
     return t;
}

std::vector<int> Find( std::vector<int>& in )
{
	std::vector<int> result;
	// TO DO: implement
	// TO DO: If none found found return -1
	return result;
}

std::vector<int> Calculate( std::vector<int>& input ) throw(std::string)
{
	// Check that the last and the first is a bracket and of the same type
	if( input[0] == keyword::leftParens ) {
		if( input[ input.size()-1 ] != keyword::rightParens ) {
			throw std::string( "Calculations must be enclosed in brackets of the same type" );
		}
	} else if( input[0] == keyword::leftClammer ) {
		if( input[ input.size()-1 ] != keyword::rightClammer ) {
			throw std::string( "Calculations must be enclosed in brackets of the same type" );
		}
	} else {
		throw std::string( "Calculations must be enclosed in brackets" );
	}

	std::vector<int> result = input;

	// Check for | if found add all the statements found on the right side
	for( int i(1); i < result.size() - 1; i++ )
		if( result[i] == keyword::where ) {
			pos_arguments.push_back( statements.size() );

			std::vector<int> temp;
			std::vector<int> bracketPos;
			for( int l(0); l < input.size(); l++ )
			{
				if( bracketPos.size() > 0 ) {
					int pos = bracketPos.front();
					int matchingBracket = input[ pos ];
					switch( input[ l ] )
					{
						case keyword::leftParens:
						case keyword::leftClammer:
							bracketPos.push_back( l );
							break;
						case keyword::rightParens: 
						case keyword::rightClammer:
							if( matchingBracket == input[l] + 1 ) {
								// Matching clammer
								break;
							} else if( matchingBracket
								== ( input[l] + 1
									== keyword::rightParens 
										? keyword::rightClammer 
										: keyword::rightParens ) ) {
								// If incompatible keyword found throw error
								throw std::string( "Incorrect matching bracket" );
							}
							bracketPos.pop_back();
							break;
						case keyword::not_a_keyword:
							l = l + input[ l + 1 ] + 1;
							break;
						default:
							break;
					}
				} else {
					switch( input[ l ] )
					{
						case keyword::leftParens:
						case keyword::leftClammer:
							bracketPos.push_back( i );
							break;
						case keyword::rightParens:
						case keyword::rightClammer:
							throw std::string( "No matching bracket" );
						case keyword::comma:
							// TO DO: check for of by one errors
							for( int k(i); k < l-1; k++ ) {
								temp.push_back( result[k] );
							}

							if( !AddStatement( temp ) )
								throw std::string( "Right of | must be statements" );
							temp.clear();

							result.erase( result.begin() + i, result.end() + l );
							temp.push_back( l - i );
							i -= temp[0];
							l -= temp[0];
							temp.pop_back();
							break;
						case keyword::not_a_keyword:
							l = l + input[ l + 1 ] + 1;
							break;
						default:
							break;
					}
				}
			}
			break;
		} else if( result[i] == keyword::not_a_keyword ) {
			i = i + input[ i + 1 ] + 1;
		}

	for( int i(1); i < result.size() - 1; i++ )
	{
		std::vector<int> presedensPos;
		std::vector<int> bracketPos;
		if( bracketPos.size() > 0 ) {
			int pos = bracketPos[ bracketPos.size() - 1 ];
			int matchingBracket = result[ pos ];
			switch( result[i] )
			{
				case keyword::leftParens:
				case keyword::leftClammer:
					bracketPos.push_back( i );
					break;
				case keyword::rightParens:
				case keyword::rightClammer:
					if( matchingBracket == result[i] + 1 ) {
						// Calculate results
						// Insert them into the vector
						std::vector<int> subPart;
						for( int k(pos); k < i+1; k++ )
							subPart.push_back( result[k] );
						result.erase( result.begin() + pos, result.begin() + i + 1 );
						subPart = Calculate( subPart );
						result.insert( result.begin() + pos, subPart.begin(), subPart.end() );
						break;
					} else if( matchingBracket
						== ( result[i] + 1
							== keyword::rightParens 
								? keyword::rightClammer 
								: keyword::rightParens ) ) {
						// If incompatible keyword found throw error
						throw std::string( "Incorrect matching bracket" );
					}
					bracketPos.pop_back();
					break;
				case keyword::not_a_keyword:
					i = i + result[ i + 1 ] + 1;
					break;
				default:
					break;
			}
		} else {
			std::vector<int> vt;
			float arg1, arg2;
			switch( result[i] )
			{
				case keyword::temp:
					// Used during the construction of the interprenter
					break;
				case keyword::leftParens:
				case keyword::leftClammer:
					bracketPos.push_back( i );
					break;
				case keyword::rightParens:
				case keyword::rightClammer:
					throw std::string( "No matching bracket" );
				case keyword::equal:
				case keyword::notEqual:
				case keyword::superEqual:
				case keyword::less:
				case keyword::notLess:
				case keyword::greater:
				case keyword::notGreater:
				case keyword::leadsTo:
				case keyword::io:
				case keyword::text:
				case keyword::number:
				case keyword::nil:
				case keyword::mu:
					throw std::string( "Ilegal keyword placement" );
				case keyword::plus:
				case keyword::minus:
				case keyword::multiply:
				case keyword::divide:
					// Check presedens
					if( presedens[ result[i] ] < presedens[ result[presedensPos.back()] ] ) {
						// TO DO: find arg1 and arg2 and remove them from result
						// TO DO: Add operator calculations
						switch( result[i] )
						{
							case keyword::plus:
								arg1 = arg1 + arg2;
								break;
							case keyword::minus:
								arg1 = arg1 - arg2;
								break;
							case keyword::multiply:
								arg1 = arg1 * arg2;
								break;
							case keyword::divide:
								arg1 = arg1 / arg2;
								break;
						}
						// TO DO: Add arg1 to result
					} else {
						presedensPos.push_back( i );
					}
					break;
				case keyword::comma:
					break;
				case keyword::not_a_keyword:
					for( int l(i+2); l < result[ i + 1 ]; l++ )
						vt.push_back( result[l] );
					vt = Find( vt );
					if( vt[0] != -1 )
					{
						// TO DO: Insert found into result
					}
					vt.clear();
					i = i + result[ i + 1 ] + 1;
					break;
				default:
					throw std::string( "Bug in interprenter, keyword: " 
						      + keywords[ result[i] ] 
						      + " doesn't exists" );
			}
		}
	}
	// TO DO: Change the result depending on the type of the brackets
	return result;
}

