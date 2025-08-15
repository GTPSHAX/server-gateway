#pragma once

#include <BaseApp.h>

#include <curl/curl.h>
#include <string>
#include <map>
#include <iostream>
#include <sstream>

/**
 * @brief Utility class to simplify HTTP requests using libcurl.
 *
 * This class provides a high-level interface for making various types of HTTP
 * requests (GET, POST, PUT, DELETE) with support for custom headers,
 * request bodies, timeouts, and SSL verification.
 *
 * It wraps the complexities of libcurl, making it easier to perform common
 * web operations in C++.
 */
class Curl {
public:
    /**
     * @brief Constructor for the Curl class.
     * Initializes the libcurl handle.
     */
    Curl();

    /**
     * @brief Destructor for the Curl class.
     * Cleans up the libcurl handle and any allocated resources.
     */
    ~Curl();

    /**
     * @brief Sets the URL for the HTTP request.
     * @param url The URL string (e.g., "http://example.com/api/data").
     */
    void setUrl(const std::string& url);

    /**
     * @brief Adds a custom header to the request.
     * @param name The name of the header (e.g., "Content-Type").
     * @param value The value of the header (e.g., "application/json").
     */
    void addHeader(const std::string& name, const std::string& value);

    /**
     * @brief Sets the request method to GET.
     * This is the default method.
     */
    void setGet();

    /**
     * @brief Sets the request method to POST and provides the request body.
     * @param postData The string data to send in the POST request body.
     */
    void setPost(const std::string& postData);

    /**
     * @brief Sets the request method to POST and provides form fields.
     * This method automatically sets the Content-Type to application/x-www-form-urlencoded.
     * @param formFields A map of key-value pairs representing form fields.
     */
    void setPostForm(const std::map<std::string, std::string>& formFields);

    /**
     * @brief Sets the request method to PUT and provides the request body.
     * @param putData The string data to send in the PUT request body.
     */
    void setPut(const std::string& putData);

    /**
     * @brief Sets the request method to DELETE.
     */
    void setDelete();

    /**
     * @brief Sets the request method to custom HTTP method.
     * @param customMethod The custom HTTP method string (e.g., "PATCH").
     */
    void setCustomRequest(const std::string& customMethod);

    /**
     * @brief Sets the total timeout for the request in seconds.
     * If the operation doesn't complete within this time, it will abort.
     * @param timeout_seconds The timeout duration in seconds.
     */
    void setTimeout(long timeout_seconds);

    /**
     * @brief Sets the timeout for the connection phase in seconds.
     * This is the maximum time allowed for the connection to be established.
     * @param timeout_seconds The connection timeout duration in seconds.
     */
    void setConnectTimeout(long timeout_seconds);

    /**
     * @brief Enables or disables SSL certificate verification.
     * It is highly recommended to keep this enabled for production environments.
     * @param enable If true, SSL verification is enabled (default). If false, it's disabled.
     */
    void setSSLVerification(bool enable);

    /**
     * @brief Enables verbose output from libcurl to stderr.
     * Useful for debugging network issues.
     * @param enable If true, verbose output is enabled. If false, it's disabled (default).
     */
    void setVerbose(bool enable);

    /**
     * @brief Sets the user agent string for the request.
     * @param userAgent The user agent string (e.g., "MyAwesomeApp/1.0").
     */
    void setUserAgent(const std::string& userAgent);

    /**
     * @brief Sets the HTTP proxy for the request.
     * @param proxyUrl The URL of the proxy server (e.g., "http://myproxy.com:8080").
     */
    void setProxy(const std::string& proxyUrl);

    /**
     * @brief Sets the username and password for HTTP basic authentication.
     * @param username The username.
     * @param password The password.
     */
    void setBasicAuth(const std::string& username, const std::string& password);

    /**
     * @brief Sets the username and password for HTTP digest authentication.
     * @param username The username.
     * @param password The password.
     */
    void setDigestAuth(const std::string& username, const std::string& password);

    /**
     * @brief Enables following HTTP redirects.
     * @param enable If true, redirects are followed. If false, they are not.
     */
    void setFollowRedirects(bool enable);

    /**
     * @brief Sets the maximum number of redirects to follow.
     * Only effective if setFollowRedirects(true) is also called.
     * @param maxRedirects The maximum number of redirects.
     */
    void setMaxRedirects(long maxRedirects);

    /**
     * @brief Executes the configured HTTP request.
     * @return true if the request was successful (CURLcode == CURLE_OK), false otherwise.
     */
    bool perform();

    /**
     * @brief Retrieves the response data (body) from the last successful request.
     * @return A string containing the response body. Empty if no data or request failed.
     */
    const std::string& getResponseData() const;

    /**
     * @brief Retrieves the HTTP response code (e.g., 200, 404, 500) from the last request.
     * @return The HTTP response code, or 0 if no request was performed or an error occurred.
     */
    long getResponseCode() const;

    /**
     * @brief Retrieves the error message from libcurl if an error occurred during the last request.
     * @return A string containing the libcurl error message. Empty if no error.
     */
    const std::string& getErrorMessage() const;

private:
    CURL* curl_;                            ///< The libcurl easy handle.
    CURLcode res_;                          ///< The result of the last curl_easy_perform call.
    char errorBuffer_[CURL_ERROR_SIZE];     ///< Buffer for libcurl error messages.
    std::string url_;                       ///< The URL for the request.
    std::string responseData_;              ///< Stores the response body.
    std::string postData_;                  ///< Stores the POST request body.
    std::string putData_;                   ///< Stores the PUT request body.
    struct curl_slist* headersList_;        ///< Linked list for custom headers.
    std::string requestMethod_;             ///< Stores the HTTP request method (e.g., "GET", "POST").

    /**
     * @brief Static callback function for writing received data.
     * This function is used by libcurl to write the response body.
     * @param contents Pointer to the received data.
     * @param size Size of each data element.
     * @param nmemb Number of data elements.
     * @param userp Pointer to the user-defined data (in this case, the Curl object's responseData_).
     * @return The number of bytes actually taken care of.
     */
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp);

    /**
     * @brief Static callback function for reading data to send (for PUT requests).
     * This function is used by libcurl to read the request body for PUT.
     * @param buffer Pointer to the buffer to fill with data.
     * @param size Size of each data element.
     * @param nitems Number of data elements.
     * @param instream Pointer to the user-defined data (in this case, a ReadData struct).
     * @return The number of bytes actually placed in the buffer.
     */
    static size_t readCallback(void* buffer, size_t size, size_t nitems, void* instream);

    /**
     * @brief Structure to hold data for the read callback (for PUT requests).
     */
    struct ReadData {
        const char* ptr;    ///< Pointer to the start of the data.
        size_t size;        ///< Remaining size of the data.
    };
};
