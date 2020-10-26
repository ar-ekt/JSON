#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <conio.h>
using namespace std;

// -1: Nothong ,  0       , 1      , 2       , 3       , 4        , 5
string types[] = {"Object", "Array", "String", "Number", "Boolean", "Null"};

void show_menu(){
	system("cls");
	
	string json_typo = 	"\n\n\n\n\n"
						"\n\t\t\t\t    88888888 .d8888b.   .d88888b.  888b    888 "
						"\n\t\t\t\t       \"88b d88P  Y88b d88P\" \"Y88b 8888b   888 "
						"\n\t\t\t\t        888 Y88b.      888     888 88888b  888 "
						"\n\t\t\t\t        888  \"Y888b.   888     888 888Y88b 888 "
						"\n\t\t\t\t        888     \"Y88b. 888     888 888 Y88b888 "
						"\n\t\t\t\t        888       \"888 888     888 888  Y88888 "
						"\n\t\t\t\t        88P Y88b  d88P Y88b. .d88P 888   Y8888 "
						"\n\t\t\t\t        888  \"Y8888P\"   \"Y88888P\"  888    Y888 "
						"\n\t\t\t\tY88b  .d88P                                    "
						"\n\t\t\t\t\"Y888888P\"  ";
	
	cout << json_typo << "-> ";
}

void show_help(){
	string help_text = 	"\nhelp  --------------------  "	"show available commands and their works"
						"\n\nexit  --------------------  " 	"exit the program"
						"\n\nset <file_path>  ---------  " 	"prepare the program to work with given json file"
						"\n\nval <path>  --------------  " 	"show the value of element in given path"
						"\n\ntype <path>  -------------  " 	"show the type of element in given path"
						"\n\n** you can access Object elements using their keys and Array elements by their indexes."
						"\n   between each single address of element must use '.' (ex: first_key.second_key.some_index)";
	
	cout << help_text;
}

bool is_whitespace(char letter){
	if(letter == ' ' || letter == '\t' || letter == '\n' || letter == '\r' || letter == '\0')
		return true;
	return false;
}

bool is_valid_for_key(char letter){
	if('a' <= letter && letter <= 'z')	return true;
	if('A' <= letter && letter <= 'Z')	return true;
	if('0' <= letter && letter <= '9')	return true;
	if(letter == '-')					return true;
	if(letter == '_')					return true;
	return false;
}

bool is_valid_after_backslash(char letter){
	char valid[] = {'"', '\\', '/', 'b', 'f', 'n', 'r', 't', 'u'};
	for(int i=0; i<9; i++)
		if(letter == valid[i])
			return true;
	return false;
}

bool is_valid_number(string value){
	int value_length = value.length();
	
	int i=0;
	
	if(i < value_length && value[i] == '-'){
		if(value_length == 1)
			return false;
		i++;
	}
	
	if(i < value_length && value[i] == '0'){
		i++;
		if(i < value_length && '0' <= value[i] && value[i] <= '9')
			return false;
	}else if(i < value_length && '1' <= value[i] && value[i] <= '9'){
		while(i < value_length && '0' <= value[i] && value[i] <= '9') i++;
	}else{
		return false;
	}
	
	if(i < value_length && value[i] == '.'){
		i++;
		if(!(i < value_length && '0' <= value[i] && value[i] <= '9')) return false;
		while(i < value_length && '0' <= value[i] && value[i] <= '9') i++;
	}
	
	if(i < value_length && (value[i] == 'e' || value[i] == 'E')){
		i++;
		if(i < value_length && (value[i] == '-' || value[i] == '+')) i++;
		if(!(i < value_length && '0' <= value[i] && value[i] <= '9')) return false;
		while(i < value_length && '0' <= value[i] && value[i] <= '9') i++;
	}
	
	if(i != value_length){
		return false;
	}
	
	return true;
}

bool is_type_json(string file_name){
	int length = file_name.length();
	int i = 1;
	string end = "";
	while(i <= 5 && i < length)
		end = file_name[length - i++] + end;
	return (end == ".json");
}

int split(string str, string lst[]){
	size_t pos = 0;
	int len = 0;
	
	while((pos = str.find(".")) != std::string::npos){
	    lst[len++] = str.substr(0, pos);
	    str.erase(0, pos+1);
	}
	lst[len++] = str;
	
	return len;
}

bool is_decimal(string str){
    std::string::const_iterator it = str.begin();
    while (it != str.end() && isdigit(*it)) ++it;
    return !str.empty() && it == str.end();
}

void result(string message=""){
	cout << "\n\t\t\t\t> " << message << endl;
}

void warning(string message="", bool reached=false, char letter=' '){
	cout << "\n\t\t\t\t>> Warning: " << message;
	if(reached) cout << ", reached '" << letter << "'";
	cout << endl;
}

struct Object;
struct Array;

struct Element{
	string key;
	string value;
	int type;
	bool in_Object;
	Object *value_obj;
	Array *value_arr;
	
	Element(){}
	
	Element(string value_, int type_, bool in_Object_, string key_=""){
		key = key_;
		type = type_;
		value = value_;
		in_Object = in_Object_;
	}
	
	Element(Object &value_, int type_, bool in_Object_, string key_=""){
		key = key_;
		type = type_;
		value_obj = &value_;
		in_Object = in_Object_;
	}
	
	Element(Array &value_, int type_, bool in_Object_, string key_=""){
		key = key_;
		type = type_;
		value_arr = &value_;
		in_Object = in_Object_;
	}
	
	string get_val(){
		if(type == 0 || type == 1)
			return types[type];
		else
			return value;
	}
	
	string get_key(){
		return key;
	}
	
	string get_type(){
		return types[type];
	}
};

struct Object{
	struct Element list[1000];
	int len;
	
	Object(){
		len = 0;
	}
	
	void add(string key_, int type_, string value_){
		list[len++] = Element(value_, type_, true, key_);
	}
	
	void add(string key_, int type_, Object &value_){
		list[len++] = Element(value_, type_, true, key_);
	}
	
	void add(string key_, int type_, Array &value_){
		list[len++] = Element(value_, type_, true, key_);
	}
	
	int search(string key){
		for(int i=0; i<len; i++)
			if(key == list[i].get_key())
				return i;
		return -1;
	}
	
	string get_val(int index){
		return list[index].get_val();
	}
	
	string get_key(int index){
		return list[index].get_key();
	}
	
	string get_type(int index){
		return list[index].get_type();
	}
	
	int extract(string json_);
	
	int read_json(char file_name[]);
	
	bool find(string path_lst[], int path_len, int step, Element *element);
};

struct Array{
	struct Element list[1000];
	int len;
	
	Array(){
		len = 0;
	}
	
	void add(string value_, int type_){
		list[len++] = Element(value_, type_, false);
	}
	void add(Object &value_, int type_){
		list[len++] = Element(value_, type_, false);
	}
	
	void add(Array &value_, int type_){
		list[len++] = Element(value_, type_, false);
	}
	
	string get_val(int index){
		return list[index].get_val();
	}
	
	string get_type(int index){
		return list[index].get_type();
	}
	
	int extract(string json_);
	
	bool find(string path_lst[], int path_len, int step, Element *element);
};

int Object::extract(string json_){
	int json_length = json_.length();
	
	int mode = 0;
	/* 	0 : wait for a '{'
		1 : wait for start of key
		2 : key
		3 : wait for ':'
	   	4 : wait for start of value
	   	5 : value
	   	6 : wait for ',' and start again from step 1 or wait for '}'
	*/
	
	int value_mode = 0;
	/*	0 : Object
		1 : Array
		2 : String
		3 : anything else
	*/
	
	char letter;
	string key;
	string val;
	
	int brac_c;				// brace and bracket counter
	bool backslash_c;		// backslash checker
	bool string_mode_brac;
	
	int i;
	for(i=0; i<json_length; i++){
		letter = json_[i];
		
		if(mode == 0){
			if(letter == '{'){
				mode = 1;
			}else if(!is_whitespace(letter)){
				warning("expected '{'", true, letter);
				return 1;
			}
		}
		
		else if(mode == 1){
			if(letter == '"'){
				key = "";
				mode = 2;
			}else if(!is_whitespace(letter)){
				warning("expected key", true, letter);
				return 1;
			}
		}
		
		else if(mode == 2){
			if(letter == '"'){
				mode = 3;
			}else if(is_valid_for_key(letter)){
				key += letter;
			}else{
				warning("invalid character in key", true, letter);
				return 1;
			}
		}
		
		else if(mode == 3){
			if(letter == ':'){
				mode = 4;
			}else if(!is_whitespace(letter)){
				warning("expected ':'", true, letter);
				return 1;
			}
		}
		
		else if(mode == 4){
			if(letter == '{'){
				value_mode = 0;
				string_mode_brac = false;
				backslash_c = false;
				brac_c = 1;
			}else if(letter == '['){
				value_mode = 1;
				string_mode_brac = false;
				backslash_c = false;
				brac_c = 1;
			}else if(letter == '"'){
				value_mode = 2;
				backslash_c = false;
			}else if(!is_whitespace(letter)){
				value_mode = 3;
			}else{
				continue;
			}
			val = letter;
			mode = 5;
		}
		
		else if(mode == 5){
			if(value_mode == 0){
				val += letter;
				if(string_mode_brac){
					if(letter == '\\'){
						backslash_c = !backslash_c;
					}else if(letter == '"' && !backslash_c){
						string_mode_brac = false;
					}
				}else if(letter == '{'){
					brac_c++;
				}else if(letter == '}'){
					brac_c--;
					if(brac_c == 0){
						Object *sub_object = new Object();
						if(sub_object->extract(val) == 1)
							return 1;
						add(key, 0, *sub_object);
						mode = 6;
					}
				}
			}else if(value_mode == 1){
				val += letter;
				if(string_mode_brac){
					if(letter == '\\'){
						backslash_c = !backslash_c;
					}else if(letter == '"' && !backslash_c){
						string_mode_brac = false;
					}
				}else if(letter == '['){
					brac_c++;
				}else if(letter == ']'){
					brac_c--;
					if(brac_c == 0){
						Array *sub_array = new Array();
						if(sub_array->extract(val) == 1)
							return 1;
						add(key, 1, *sub_array);
						mode = 6;
					}
				}
			}else if(value_mode == 2){
				val += letter;
				if(letter == '\\'){
					backslash_c = !backslash_c;
				}else if(backslash_c){
					if(is_valid_after_backslash(letter)){
						backslash_c = false;
					}else{
						warning("unknown escape sequence", true, letter);
						return 1;
					}
				}else if(letter == '"'){
					add(key, 2, val);
					mode = 6;
				}
			}else{
				if(is_whitespace(letter) || letter == ',' || letter == '}'){
					if(val == "true"){
						add(key, 4, val);
					}else if(val == "false"){
						add(key, 4, val);
					}else if(val == "null"){
						add(key, 5, val);
					}else if(is_valid_number(val)){
						add(key, 3, val);
					}else{
						warning("unknown value");
						return 1;
					}
					
					if(letter == ','){
						mode = 1;
					}else if(letter == '}'){
						break;
					}else{
						mode = 6;
					}
				}else{
					val += letter;
				}
			}
		}
		
		else if(mode == 6){
			if(letter == ','){
				mode = 1;
			}else if(letter == '}'){
				break;
			}else if(!is_whitespace(letter)){
				warning("expected ',' or '}'", true, letter);
				return 1;
			}
		}
	}
	
	for(i+=1; i<json_length; i++){
		letter = json_[i];
		if(!is_whitespace(letter)){
			warning("expected data has ended", true, letter);
			return 1;
		}
	}
	
	return 0;
}

int Array::extract(string json_){
	int json_length = json_.length();
	
	int mode = 0;
	/* 	0 : wait for a '['
		1 : wait for start of value
		2 : value
	   	3 : wait for ',' and start again from step 1 or wait for ']'
	*/
	
	int value_mode = 0;
	/*	0 : Object
		1 : Array
		2 : String
		3 : anything else
	*/
	
	char letter;
	string val;
	
	int brac_c;			// brace and bracket counter
	bool backslash_c;		// backslash checker
	bool string_mode_brac;
	
	int i;
	for(i=0; i<json_length; i++){
		letter = json_[i];
		
		if(mode == 0){
			if(letter == '['){
				mode = 1;
			}else if(!is_whitespace(letter)){
				warning("expected '['", true, letter);
				return 1;
			}
		}
		
		else if(mode == 1){
			if(letter == '{'){
				value_mode = 0;
				string_mode_brac = false;
				backslash_c = false;
				brac_c = 1;
			}else if(letter == '['){
				value_mode = 1;
				string_mode_brac = false;
				backslash_c = false;
				brac_c = 1;
			}else if(letter == '"'){
				value_mode = 2;
				backslash_c = false;
			}else if(!is_whitespace(letter)){
				value_mode = 3;
			}else{
				continue;
			}
			val = letter;
			mode = 2;
		}
		
		else if(mode == 2){
			if(value_mode == 0){
				val += letter;
				if(string_mode_brac){
					if(letter == '\\'){
						backslash_c = !backslash_c;
					}else if(letter == '"' && !backslash_c){
						string_mode_brac = false;
					}
				}else if(letter == '{'){
					brac_c++;
				}else if(letter == '}'){
					brac_c--;
					if(brac_c == 0){
						Object *sub_object = new Object();
						if(sub_object->extract(val) == 1)
							return 1;
						add(*sub_object, 0);
						mode = 3;
					}
				}
			}else if(value_mode == 1){
				val += letter;
				if(string_mode_brac){
					if(letter == '\\'){
						backslash_c = !backslash_c;
					}else if(letter == '"' && !backslash_c){
						string_mode_brac = false;
					}
				}else if(letter == '['){
					brac_c++;
				}else if(letter == ']'){
					brac_c--;
					if(brac_c == 0){
						Array *sub_array = new Array();
						if(sub_array->extract(val) == 1)
							return 1;
						add(*sub_array, 1);
						mode = 3;
					}
				}
			}else if(value_mode == 2){
				val += letter;
				if(letter == '\\'){
					backslash_c = !backslash_c;
				}else if(backslash_c){
					if(is_valid_after_backslash(letter)){
						backslash_c = false;
					}else{
						warning("unknown escape sequence", true, letter);
						return 1;
					}
				}else if(letter == '"'){
					add(val, 2);
					mode = 3;
				}
			}else{
				if(is_whitespace(letter) || letter == ',' || letter == ']'){
					if(val == "true"){
						add(val, 4);
					}else if(val == "false"){
						add(val, 4);
					}else if(val == "null"){
						add(val, 5);
					}else if(is_valid_number(val)){
						add(val, 3);
					}else{
						warning("unknown value");
						return 1;
					}
					
					if(letter == ','){
						mode = 1;
					}else if(letter == ']'){
						break;
					}else{
						mode = 3;
					}
				}else{
					val += letter;
				}
			}
		}
		
		else if(mode == 3){
			if(letter == ','){
				mode = 1;
			}else if(letter == ']'){
				break;
			}else if(!is_whitespace(letter)){
				warning("expected ',' or ']'", true, letter);
				return 1;
			}
		}
	}
	
	for(i+=1; i<json_length; i++){
		letter = json_[i];
		if(!is_whitespace(letter)){
			warning("expected data has ended", true, letter);
			return 1;
		}
	}
	
	return 0;
}

int Object::read_json(char file_name[]){
	ifstream file(file_name);
	string json;
	string line;
	
	if(!is_type_json(file_name)){
		warning("type of file is not .json");
		return 1;
	}
	
	if(file.is_open()){
		while(getline(file, line)){
			json += line + "\n";
		}
		file.close();
	}
	else{
		warning("file not found");
		return 1;
	}
	
	return extract(json);
}

bool Object::find(string path_lst[], int path_len, int step, Element *element){
	int index = search(path_lst[step]);
	if(index == -1){}
	else if(step == path_len-1){
		*element = list[index];
		return true;
	}else if(list[index].type == 0){
		return list[index].value_obj->find(path_lst, path_len, step+1, element);
	}else if(list[index].type == 1){
		return list[index].value_arr->find(path_lst, path_len, step+1, element);
	}
	
	return false;
}

bool Array::find(string path_lst[], int path_len, int step, Element *element){
	if(!is_decimal(path_lst[step])){
		return false;
	}
	
	stringstream str2num(path_lst[step]);
	int index;
	str2num >> index;
	
	if(index > len){}
	else if(step == path_len-1){
		*element = list[index];
		return true;
	}else if(list[index].type == 0){
		return list[index].value_obj->find(path_lst, path_len, step+1, element);
	}else if(list[index].type == 1){
		return list[index].value_arr->find(path_lst, path_len, step+1, element);
	}
	
	return false;
}

int main(){
	
	int status;
	int path_len;
	bool set = false;
	
	char path[1000];
	string path_lst[100];
	string command;
	
	Object *object = new Object();
	Object *temp_object;
	Element temp_element;
	
	while(1){
		
		show_menu();
		
		cin >> command;
		
		if(command == "exit"){
			result("bye bye");
			break;
		}
		
		else if(command == "help"){
			show_help();
		}
		
		else if(command == "set"){
			cin >> path;
			
			temp_object = new Object();
			status = temp_object->read_json(path);
			if(status == 0){
				object = temp_object;
				result("data set successfully");
				set = true;
			}
		}
		
		else if(command == "val" || command == "type"){
			cin >> path;
			path_len = split(path, path_lst);
			status = object->find(path_lst, path_len, 0, &temp_element);
			
			if(!set){
				warning("no data set");
			}else if(status){
				if(command == "val")
					result(temp_element.get_val());
				else
					result(temp_element.get_type());
			}else{
				warning("cannot find the given path");
			}
		}
		
		else{
			warning("unknown command");
		}
		
		cin.ignore(100, '\n');
		getch();
	}
	
	return 0;
}
