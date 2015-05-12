/**
 * httpclient.h adapted from restclient.h, written by Daniel Schauenberg
 * https://github.com/mrtazz/restclient-cpp/blob/0df677338a22632b8ebd727f215e86fa233ec9ca/include/restclient.h
 * 
 */

#ifndef HTTPCLIENT_H__
#define HTTPCLIENT_H__

#include <curl/curl.h>
#include <string>
#include <map>
#include <cstdlib>
#include <algorithm>
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>
#include <util/foreach.h>
#include <vector>

using boost::property_tree::ptree;

class HttpClient
{
  public:
    /**
     * public data definitions
     */
    typedef std::map<std::string, std::string> headermap;

    /** response struct for queries */
    typedef struct
    {
      int code;
      std::string body;
      headermap headers;
    } response;
    /** struct used for uploading data */
    typedef struct
    {
      const char* data;
      size_t length;
    } upload_object;

    HttpClient();

    ~HttpClient();

    /** public methods */
    // Auth
    void clearAuth();
    void setAuth(const std::string& user,const std::string& password);
    // HTTP GET
    response get(const std::string& url) const;
    // HTTP POST
    response post(const std::string& url, const std::string& ctype,
                         const std::string& data) const;
    // HTTP PUT
    response put(const std::string& url, const std::string& ctype,
                        const std::string& data) const;
    // HTTP DELETE
    response del(const std::string& url) const;
	
	/**
	 * Return a property tree representing the contents of a JSON response generated by an HTTP
	 * GET request at the given url.
	 * @param url the url with which to make the GET request
	 * @return a property tree representing the JSON generated by this request. Will return null
	 * if JSON parsing is impossible.
	 */
	boost::shared_ptr<ptree> getPropertyTree(const std::string& url);
	
	/**
	 * Return a property tree representing the contents of a JSON response generated by an HTTP
	 * POST request at the given url.
	 * @param url the url with which to make the POST request
	 * @param data the data to post
	 * @return a property tree representing the JSON generated by this request. Will return null
	 * if JSON parsing is impossible.
	 */
	boost::shared_ptr<ptree> postPropertyTree(const std::string& url,
		const std::string& data);
	
	
	/**
	 * Push all values from a given property tree element into a vector. This method is designed to
	 * handle the case of a JSON array, and requires that get_value<T> will not return an error
	 * for any key-value pair in the property tree at this level.
	 * @param pt a property tree to parse as an array
	 * @param vect the vector into which the property tree values will be pushed
	 * @return the number of elements that were pushed into the vector
	 */
	template <typename T>
	static unsigned int ptreeVector(const ptree& pt, std::vector<T>& vect)
	{
		unsigned int count = 0;
		vect.reserve(vect.size() + pt.size());
		for (ptree::value_type v : pt)
		{
			vect.push_back(v.second.get_value<T>());
			++count;
		}
		return count;
	}
	
	/**
	 * Check a property tree for django errors. This method will log errors to [HttpClient]
	 * @param pt the property tree to check
	 * @return true if an error was detected, false if all is ok
	 */
	static bool checkDjangoError(const boost::shared_ptr<ptree> pt);
	
	static bool ptreeHasChild(const boost::shared_ptr<ptree> pt, const std::string& childName);

  private:
    // writedata callback function
    static size_t write_callback(void *ptr, size_t size, size_t nmemb,
                                 void *userdata);

    // header callback function
    static size_t header_callback(void *ptr, size_t size, size_t nmemb,
				  void *userdata);
    // read callback function
    static size_t read_callback(void *ptr, size_t size, size_t nmemb,
                                void *userdata);
    static const char* user_agent;

    std::string _user_pass;
    CURL* _curl;

    // trim from start
    static inline std::string &ltrim(std::string &s) {
      s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
      return s;
    }

    // trim from end
    static inline std::string &rtrim(std::string &s) {
      s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
      return s;
    }

    // trim from both ends
    static inline std::string &trim(std::string &s) {
      return ltrim(rtrim(s));
    }

	static boost::shared_ptr<ptree> parsePtree(const response& res, const std::string& url);

	static void handleNon200(const response& res, const std::string& url);

    static bool checkCurlError(
        const                 CURLcode res,
        char*                 curlErrorBuffer,
        HttpClient::response& ret);

};

#endif  // HTTPCLIENT_H__
