//: C26:CGImap.h
// Tools for extracting and decoding data from
// from CGI GETs and POSTs.

#ifndef CGIPARSER_H
#define CGIPARSER_H


#include <string>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>



#include "../config.h"


using namespace std;

class CGIpair : public pair<string, string> {
public:
  CGIpair() {}
  CGIpair(string name, string value) {
    first = decodeURLString(name);
    second = decodeURLString(value);
  }
  // Automatic type conversion for boolean test:
  operator bool() const {
    return (first.length() != 0);
  }
private:
	static string decodeURLString(string URLstr);
	static char translateHex(char hex);
};

// Parses any CGI query and turns it into an
// STL vector of CGIpair which has an associative
// lookup operator[] like a map. A vector is used
// instead of a map because it keeps the original
// ordering of the fields in the Web page form.
class CGImap : public vector<CGIpair> {
  string gq;
  int index;
  // Prevent assignment and copy-construction:
  void operator=(CGImap&);
  CGImap(CGImap&);
public:
  CGImap(string query): index(0), gq(query){
    CGIpair p;
    while((p = nextPair()) != 0)
      push_back(p);
  }
  // Look something up, as if it were a map:
  string operator[](const string& key) {
    iterator i = begin();
    while(i != end()) {
      if((*i).first == key)
        return (*i).second;
      i++;
    }
    return string(); // Empty string == not found
  }
  void dump(ostream& o, string nl = "<br>") {
    for(iterator i = begin(); i != end(); i++) {
      o << (*i).first << " = "
        << (*i).second << nl;
    }
  }
private:
  // Produces name-value pairs from the query
  // string. Returns an empty Pair when there's
  // no more query string left:
  CGIpair nextPair() {
    if(gq.length() == 0)
      return CGIpair(); // Error, return empty
    if(gq.find('=') == -1)
      return CGIpair(); // Error, return empty
    string name = gq.substr(0, gq.find('='));
    gq = gq.substr(gq.find('=') + 1);
    string value = gq.substr(0, gq.find('&'));
    gq = gq.substr(gq.find('&') + 1);
    return CGIpair(name, value);
  }
};

// Helper class for getting POST data:
class Post : public string {
public:
  Post() {
    // For a CGI "POST," the server puts the
	// length of the content string in the
	// environment variable CONTENT_LENGTH:
	char* clen = getenv("CONTENT_LENGTH");
	if(clen == 0) {
	  cout << "Zero CONTENT_LENGTH, Make sure "
		"this is a POST and not a GET" << endl;
	  return;
	}
	int len = atoi(clen);
	char* s = new char[len];
	cin.read(s, len); // Get the data
	append(s, len); // Add it to this string
	delete []s;
  }
}; ///:~


#endif
