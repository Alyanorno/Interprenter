#include <iostream>
#include <string>
#include <vector>
#include <stack>

const int num_keywords = 9;
namespace keyword
	{ enum { temp = 0,
		 leftParens,
		 rightParens,
		 equal,
		 notEqual,
		 lessSign,
		 greaterSign,
		 leadsTo,
		 question,
		 not_a_keyword = num_keywords }; }
std::string keywords[ num_keywords ] = { "temp",
				    "(",
				    ")",
				    "=",
				    "!=",
				    "<",
				    ">",
				    "->",
				    "?" };

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
	std::vector< int > left;
	int connection;
	std::vector< int > right;
	operator std::string()
	{
		std::string result = "";
		for( int i(0); i < left.size(); i++ )
			if( left[i] == keyword::not_a_keyword ) {
				for( int l(0); l < left[i+1]; l++ )
					result.append( 1, (char)left[i+2+l] );
				result.append( " " );
				i += left[i+1] + 1;
			} else {
				result.append( keywords[ left[i] ] + " " );
			}
		if( connection != 0 )
			result.append( keywords[ connection ] + " " );
		for( int i(0); i < right.size(); i++ )
			if( right[i] == keyword::not_a_keyword ) {
				for( int l(0); l < right[i+1]; l++ )
					result.append( 1, (char)right[i+2+l] );
				result.append( " " );
				i += right[i+1] + 1;
			} else {
				result.append( keywords[ right[i] ] + " " );
			}
		return result;
	}
};
std::vector< Statement > statements;
std::vector< std::vector< int > > stack;

std::string Trim( std::string input );
void Parse( std::string input, std::vector<int>& result ) throw (std::string);
void Run( std::vector<int>& input, std::vector< Statement >& result ) throw (std::string);
void Calculate( std::vector<int>& input ) throw(std::string);
int main()
{
	std::string input = "";
	do
	{
		std::string line;
		getline( std::cin, line );
		line = Trim( line );
		if( line.size() == 0 )
			continue;
		if( line == "quit" )
			break;

		input.append( Trim( " " + line ) );

		if( input.size() < 2 || input.compare( input.size() - 2, 2, "++" ) != 0 )
		{
			try {
				std::vector<int> outParse;
				std::vector< Statement > outRun;
				Parse( input, outParse );
				Run( outParse, outRun );
				for( int i(0); i < outRun.size(); i++ )
					std::cout << (std::string)outRun[i] << std::endl;
			} catch( std::string error ) {
				std::cout << "Error: " << error << std::endl;
			}
			input.clear();
			stack.clear();
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

void Parse( std::string input, std::vector<int>& result ) throw (std::string)
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
}

void Run( std::vector<int>& input, std::vector< Statement >& result ) throw (std::string)
{
	for( int i( 0 ); i < input.size(); i++ )
		std::cout << input[i] << std::endl;
	if( input[ input.size() - 1 ] == keyword::question ) {
		// Answer the question
		bool foundAnswer = false;
		for( int i(0); i < statements.size(); i++ )
		{
			if( statements[i].left.size() == input.size() - 1 )
				for( int l(0); l < input.size() - 1; l++ )
					if( statements[i].left[l] != input[l] )
						break;
					else if( l == input.size() - 2 )
					{
						foundAnswer = true;
						// TO DO: Add arguments to stack
						Calculate( statements[i].right );
						// Return the stack as result
						for( int k(0); k < stack.size(); k++ )
							result.push_back( Statement( statements[i].left,
										     statements[i].connection, 
										     stack[k] ) );
					}
		}
		if( !foundAnswer ) {
			Calculate( input );
			std::vector<int> empty;
			for( int k(0); k < stack.size(); k++ )
				result.push_back( Statement( stack[k], 0, empty ) );
		}
	} else {
		// Add a new statement
		Statement statement;
		int deapthParens = 0;
		for( int i(0); i < input.size(); i++ )
		{
			switch( input[ i ] )
			{
				case keyword::equal:
				case keyword::notEqual:
				case keyword::lessSign:
				case keyword::greaterSign:
				case keyword::leadsTo:
					if( deapthParens != 0 )
						break;
					statement.connection = input[i];
					for( int l(0); l < i; l++ )
						statement.left.push_back( input[ l ] );
					for( int l(i+1); l < input.size(); l++ )
						statement.right.push_back( input[ l ] );
					i = input.size();
					break;
				case keyword::not_a_keyword:
					i = i + input[ i + 1 ] + 1;
					break;
				case keyword::leftParens:
					deapthParens++;
					break;
				case keyword::rightParens:
					deapthParens--;
					if( deapthParens < 0 )
						throw std::string( "No matching parens" );
					break;
			}
			if( i == input.size() - 1 )
				throw std::string( "No connection in statement" );
		}
		statements.push_back( statement );
		result.push_back( statement );
	}
}

void Calculate( std::vector<int>& input ) throw(std::string)
{
	// start calculating answer
	// if not a number check stack
	// create statement from stack and put on result
}

