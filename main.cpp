#include <iostream>
#include <string>
#include <vector>

using namespace std;

enum { plus, minus, multiply, division };

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
	

	return input;
}

vector<int>* Parse( string input, vector<int>& result ) throw (string)
{
	return &result;
}

string Run( vector<int>* input ) throw (string)
{
	return "Running";
}

