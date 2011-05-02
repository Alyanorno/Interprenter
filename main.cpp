#include <iostream>
#include <string>
#include <vector>
#include <stack>

using namespace std;

const int num_keywords = 6;
namespace keyword
	{ enum { temp = 0,
		 leftParens,
		 equal,
		 lessSign,
		 greaterSign,
		 question,
		 not_a_keyword = num_keywords }; }
string keywords[ num_keywords ] = { "temp",
				    "(",
				    "=",
				    "<",
				    ">",
				    "?" };

struct Statement
{
	Statement() : isTrue( true ), isFalse( false ) {}
	vector< int > left;
	int connection;
	vector< int > right;
	bool isTrue;
	bool isFalse;
	operator string()
	{
		string result = "";
		for( int i(0); i < left.size(); i++ )
			if( left[i] == keyword::not_a_keyword ) {
				for( int l(0); l < left[i+1]; l++ )
					result.append( 1, (char)left[i+2+l] );
				i += left[i+1] + 1;
			} else {
				result.append( keywords[ left[i] ] );
			}
		result.append( " " + keywords[ connection ] + " " );
		for( int i(0); i < right.size(); i++ )
			if( right[i] == keyword::not_a_keyword ) {
				for( int l(0); l < right[i+1]; l++ )
					result.append( 1, (char)right[i+2+l] );
				i += right[i+1] + 1;
			} else {
				result.append( keywords[ right[i] ] );
			}
		return result;
	}
};
vector< Statement > statements;

string Trim( string input );
vector<int>& Parse( string input, vector<int>& result ) throw (string);
string Run( vector<int>& input ) throw (string);
int main()
{
	string input = "";
	vector<int> result;
	do
	{
		string line;
		getline( cin, line );
		line = Trim( line );
		if( line.size() == 0 )
			continue;
		if( line == "quit" )
			break;

		input.append( Trim( " " + line ) );

		if( input.size() < 2 || input.compare( input.size() - 2, 2, "++" ) != 0 )
		{
			try
				{ cout << Run( Parse( input, result ) ) << endl; }
			catch( string error )
				{ cout << "Error: " << error << endl; }
			input.clear();
			result.clear();
		}
		else
			input.erase( input.size() - 2 );
	}
	while( true );
}

string Trim( string input )
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

vector<int>& Parse( string input, vector<int>& result ) throw (string)
{
	vector< string > words;
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
	return result;
}

string Run( vector<int>& input ) throw (string)
{
	for( int i( 0 ); i < input.size(); i++ )
		cout << input[i] << endl;
	if( input[ input.size() - 1 ] == keyword::question ) {
		// Answer the question
		for( int i(0); i < statements.size(); i++ )
		{
			if( statements[i].left.size() != input.size() - 1 )
				continue;
			for( int l(0); l < input.size() - 1; l++ )
				if( statements[i].left[l] != input[l] )
					continue;
			// Found statement
			return (string)statements[i];
		}
		return "No answer defined";
	} else {
		// Add a new statement
		Statement statement;
		int connection;
		for( int i(0); i < input.size(); i++ )
		{
			switch( input[ i ] )
			{
				#define SET_STATEMENT( KEYWORD ) \
					statement.connection = keyword::##KEYWORD; \
					for( int l(0); l < i; l++ ) \
						statement.left.push_back( input[ l ] ); \
					for( int l(i+1); l < input.size(); l++ ) \
						statement.right.push_back( input[ l ] ); \
					i = input.size();
				case keyword::equal:
					SET_STATEMENT( equal );
					break;
				case keyword::lessSign:
					SET_STATEMENT( lessSign );
					break;
				case keyword::greaterSign:
					SET_STATEMENT( greaterSign );
					break;
				case keyword::not_a_keyword:
					i = i + input[ i + 1 ] + 1;
					break;
				#undef SET_STATEMENT
			}
			if( i == input.size() - 1 )
				throw string( "No connection in statement" );
		}
		statements.push_back( statement );
	}
	return "Running";
}

