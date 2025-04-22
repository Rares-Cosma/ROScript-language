#include <string>

using namespace std;

class Type {
	public:
		bool is_float_numeral(string kwd){
			bool found_comma=false;
			string acc="0123456789";
			for (char i:kwd){
				if (acc.find(i) == string::npos){
					if (i=='.' && !found_comma) found_comma=true;
					else return false;
				}
			}
			if (found_comma) return true;
			return false;
		}
		bool is_integer_numeral(string kwd){
			string acc="0123456789";
			for (char i:kwd){
				if (acc.find(i) == string::npos){
					return false;
				}
			}
			return true;
		}
		bool is_string_numeral(string kwd){
			if (kwd[0]=='"' && kwd[kwd.size()-1]=='"') return true;
			return false;
		}
		string get_type(string kwd){
			if (is_integer_numeral(kwd)) return "int";
			else if (is_float_numeral(kwd)) return "float";
			else if (is_string_numeral(kwd)) return "string";
			else return "NDT"; // non determined type
		}
};	
