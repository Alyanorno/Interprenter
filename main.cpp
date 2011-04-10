#include <iostream>
#include <string>
#include <vector>
#include <cassert>

using namespace std;

const int num_keywords = 1;
enum { not_a_keyword, temp };
string keywords[ num_keywords ] = { "temp" };

string Trim( string input );
vector<int>* Parse( string input, vector<int>& result ) throw (string);
string Run( vector<int>* input ) throw (string);
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

		input.append( " " + line );

		if( input.size() < 2 || input.compare( input.size() - 2, 2, "++" ) != 0 )
		{
			try
				{ cout << Run( Parse( input, result ) ) << endl; }
			catch( string error )
				{ cout << "Error: " << error << endl; }
			input.clear();
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
	for( int i(0); i < input.size() - 1; i++ )
		if( input[i] == ' ' && input[i+1] == ' ' )
			input.erase( i--, 1 );

	cout << input << endl;

	return input;
}

vector<int>* Parse( string input, vector<int>& result ) throw (string)
{
	int num_words = 1;
	for( int i(0); i < input.size(); i++ )
		if( input[i] == ' ' )
			num_words++;
	
	string* words = new string[ num_words ];

	for( int i(0), l(0); i < input.size(); i++ )
		if( input[i] != ' ' )
			words[l] += input[i];
		else
			l++;

	for( int i(0); i < num_words; i++ )
	{
		for( int l(0); l < num_keywords; l++ )
		{
			if( words[i].compare( keywords[l] ) == 0 )
			{
				// Add the numerical representation of keyword to result
				#define IF_ADD_KEYWORD( KEYWORD ) \
					else if( keywords[l] == #KEYWORD ) \
					result.push_back( KEYWORD )
				if( false );
				IF_ADD_KEYWORD( temp );
				else
				{
					// Should never happen
					throw("Bug in interprenter, keyword: \"" + words[i] + "\" doesn't exist");
				}
				#undef IF_ADD_KEYWORD
				break;
			}

			// Not a keyword
			result.push_back( not_a_keyword );
			result.push_back( words[i].size() );
			for( int k(0); k < words[i].size(); k++ )
				result.push_back( (int)words[i][k] );
		}
	}
	return &result;
}

string Run( vector<int>* input ) throw (string)
{
	return "Running";
}

