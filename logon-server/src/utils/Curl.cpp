// Curl.cpp
#include "Curl.h"

// Static initialization of libcurl (optional, can be done once per application)
// For simplicity, we initialize and cleanup per Curl object.
// For multi-threaded applications, curl_global_init/cleanup should be used carefully.

/**
 * @brief Static write callback function for libcurl.
 * Appends received data to the responseData_ string.
 */
size_t Curl::writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    std::string* response = static_cast<std::string*>(userp);
    response->append(static_cast<char*>(contents), realsize);
    return realsize;
}

/**
 * @brief Static read callback function for libcurl (for PUT requests).
 * Reads data from the provided ReadData struct and copies it to the buffer.
 */
size_t Curl::readCallback(void* buffer, size_t size, size_t nitems, void* instream) {
    ReadData* readptr = static_cast<ReadData*>(instream);
    size_t buffer_size = size * nitems;
    size_t size_s = readptr->size;
    size_t copy_amount = min(size_s, buffer_size);

    if (copy_amount > 0) {
        memcpy(buffer, readptr->ptr, copy_amount);
        readptr->ptr += copy_amount;
        readptr->size -= copy_amount;
    }
    return copy_amount;
}

/**
 * @brief Curl class constructor.
 * Initializes curl globally and sets up the easy handle.
 */
Curl::Curl() : curl_(nullptr), res_(CURLE_OK), headersList_(nullptr) {
    // Initialize libcurl global features (thread-safe, can be called multiple times)
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // Initialize the easy handle
    curl_ = curl_easy_init();
    if (curl_) {
        // Set up error buffer
        errorBuffer_[0] = '\0';
        curl_easy_setopt(curl_, CURLOPT_ERRORBUFFER, errorBuffer_);

        // Set default write callback
        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &responseData_);

        // Set default verbose to off
        curl_easy_setopt(curl_, CURLOPT_VERBOSE, 0L);

        // Enable following redirects by default
        setFollowRedirects(true);
    } else {
        // Handle initialization error
        std::cerr << "Error: Could not initialize libcurl easy handle." << std::endl;
    }
}

/**
 * @brief Curl class destructor.
 * Cleans up the easy handle and global curl resources.
 */
Curl::~Curl() {
    if (curl_) {
        curl_easy_cleanup(curl_);
    }
    if (headersList_) {
        curl_slist_free_all(headersList_);
    }
    // Cleanup libcurl global features (should be called only once per application exit)
    curl_global_cleanup();
}

/**
 * @brief Sets the URL for the HTTP request.
 */
void Curl::setUrl(const std::string& url) {
    url_ = url;
    if (curl_) {
        curl_easy_setopt(curl_, CURLOPT_URL, url_.c_str());
    }
}

/**
 * @brief Adds a custom header to the request.
 */
void Curl::addHeader(const std::string& name, const std::string& value) {
    std::string header = name + ": " + value;
    headersList_ = curl_slist_append(headersList_, header.c_str());
    if (curl_) {
        curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headersList_);
    }
}

/**
 * @brief Sets the request method to GET.
 */
void Curl::setGet() {
    requestMethod_ = "GET";
    if (curl_) {
        curl_easy_setopt(curl_, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl_, CURLOPT_POST, 0L); // Ensure POST is off
        curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, nullptr); // Clear custom request
        curl_easy_setopt(curl_, CURLOPT_UPLOAD, 0L); // Ensure PUT is off
    }
}

/**
 * @brief Sets the request method to POST and provides the request body.
 */
void Curl::setPost(const std::string& postData) {
    requestMethod_ = "POST";
    postData_ = postData;
    if (curl_) {
        curl_easy_setopt(curl_, CURLOPT_POST, 1L);
        curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, postData_.c_str());
        curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, postData_.length());
        curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, nullptr); // Clear custom request
        curl_easy_setopt(curl_, CURLOPT_HTTPGET, 0L); // Ensure GET is off
        curl_easy_setopt(curl_, CURLOPT_UPLOAD, 0L); // Ensure PUT is off
    }
}

/**
 * @brief Sets the request method to POST and provides form fields.
 */
void Curl::setPostForm(const std::map<std::string, std::string>& formFields) {
    requestMethod_ = "POST";
    std::stringstream ss;
    bool first = true;
    for (const auto& pair : formFields) {
        if (!first) {
            ss << "&";
        }
        char* encodedKey = curl_easy_escape(curl_, pair.first.c_str(), pair.first.length());
        char* encodedValue = curl_easy_escape(curl_, pair.second.c_str(), pair.second.length());
        if (encodedKey && encodedValue) {
            ss << encodedKey << "=" << encodedValue;
        }
        curl_free(encodedKey);
        curl_free(encodedValue);
        first = false;
    }
    postData_ = ss.str();
    
    // Automatically add Content-Type for form data
    addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    setPost(postData_); // Reuse the setPost logic
}

/**
 * @brief Sets the request method to PUT and provides the request body.
 */
void Curl::setPut(const std::string& putData) {
    requestMethod_ = "PUT";
    putData_ = putData;
    if (curl_) {
        curl_easy_setopt(curl_, CURLOPT_UPLOAD, 1L); // Enable uploading
        curl_easy_setopt(curl_, CURLOPT_PUT, 1L);    // Set method to PUT

        // Setup for read callback
        ReadData rd;
        rd.ptr = putData_.c_str();
        rd.size = putData_.length();

        curl_easy_setopt(curl_, CURLOPT_READFUNCTION, readCallback);
        curl_easy_setopt(curl_, CURLOPT_READDATA, &rd);
        curl_easy_setopt(curl_, CURLOPT_INFILESIZE, static_cast<long>(putData_.length()));

        curl_easy_setopt(curl_, CURLOPT_POST, 0L); // Ensure POST is off
        curl_easy_setopt(curl_, CURLOPT_HTTPGET, 0L); // Ensure GET is off
        curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, nullptr); // Clear custom request
    }
}

/**
 * @brief Sets the request method to DELETE.
 */
void Curl::setDelete() {
    requestMethod_ = "DELETE";
    if (curl_) {
        curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "DELETE");
        curl_easy_setopt(curl_, CURLOPT_POST, 0L); // Ensure POST is off
        curl_easy_setopt(curl_, CURLOPT_HTTPGET, 0L); // Ensure GET is off
        curl_easy_setopt(curl_, CURLOPT_UPLOAD, 0L); // Ensure PUT is off
    }
}

/**
 * @brief Sets the request method to custom HTTP method.
 */
void Curl::setCustomRequest(const std::string& customMethod) {
    requestMethod_ = customMethod;
    if (curl_) {
        curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, customMethod.c_str());
        curl_easy_setopt(curl_, CURLOPT_POST, 0L); // Ensure POST is off
        curl_easy_setopt(curl_, CURLOPT_HTTPGET, 0L); // Ensure GET is off
        curl_easy_setopt(curl_, CURLOPT_UPLOAD, 0L); // Ensure PUT is off
    }
}

/**
 * @brief Sets the total timeout for the request in seconds.
 */
void Curl::setTimeout(long timeout_seconds) {
    if (curl_) {
        curl_easy_setopt(curl_, CURLOPT_TIMEOUT, timeout_seconds);
    }
}

/**
 * @brief Sets the timeout for the connection phase in seconds.
 */
void Curl::setConnectTimeout(long timeout_seconds) {
    if (curl_) {
        curl_easy_setopt(curl_, CURLOPT_CONNECTTIMEOUT, timeout_seconds);
    }
}

/**
 * @brief Enables or disables SSL certificate verification.
 */
void Curl::setSSLVerification(bool enable) {
    if (curl_) {
        curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, enable ? 1L : 0L);
        curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYHOST, enable ? 2L : 0L);
    }
}

/**
 * @brief Enables verbose output from libcurl to stderr.
 */
void Curl::setVerbose(bool enable) {
    if (curl_) {
        curl_easy_setopt(curl_, CURLOPT_VERBOSE, enable ? 1L : 0L);
    }
}

/**
 * @brief Sets the user agent string for the request.
 */
void Curl::setUserAgent(const std::string& userAgent) {
    if (curl_) {
        curl_easy_setopt(curl_, CURLOPT_USERAGENT, userAgent.c_str());
    }
}

/**
 * @brief Sets the HTTP proxy for the request.
 */
void Curl::setProxy(const std::string& proxyUrl) {
    if (curl_) {
        curl_easy_setopt(curl_, CURLOPT_PROXY, proxyUrl.c_str());
    }
}

/**
 * @brief Sets the username and password for HTTP basic authentication.
 */
void Curl::setBasicAuth(const std::string& username, const std::string& password) {
    if (curl_) {
        curl_easy_setopt(curl_, CURLOPT_USERNAME, username.c_str());
        curl_easy_setopt(curl_, CURLOPT_PASSWORD, password.c_str());
        curl_easy_setopt(curl_, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    }
}

/**
 * @brief Sets the username and password for HTTP digest authentication.
 */
void Curl::setDigestAuth(const std::string& username, const std::string& password) {
    if (curl_) {
        curl_easy_setopt(curl_, CURLOPT_USERNAME, username.c_str());
        curl_easy_setopt(curl_, CURLOPT_PASSWORD, password.c_str());
        curl_easy_setopt(curl_, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
    }
}

/**
 * @brief Enables following HTTP redirects.
 */
void Curl::setFollowRedirects(bool enable) {
    if (curl_) {
        curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, enable ? 1L : 0L);
    }
}

/**
 * @brief Sets the maximum number of redirects to follow.
 */
void Curl::setMaxRedirects(long maxRedirects) {
    if (curl_) {
        curl_easy_setopt(curl_, CURLOPT_MAXREDIRS, maxRedirects);
    }
}

/**
 * @brief Executes the configured HTTP request.
 */
bool Curl::perform() {
    if (!curl_) {
        std::cerr << "Error: Curl handle not initialized." << std::endl;
        return false;
    }

    responseData_.clear(); // Clear previous response data
    errorBuffer_[0] = '\0'; // Clear previous error message

    res_ = curl_easy_perform(curl_);

    if (res_ != CURLE_OK) {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res_);
        if (errorBuffer_[0] != '\0') {
            std::cerr << " - " << errorBuffer_;
        }
        std::cerr << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief Retrieves the response data (body) from the last successful request.
 */
const std::string& Curl::getResponseData() const {
    return responseData_;
}

/**
 * @brief Retrieves the HTTP response code from the last request.
 */
long Curl::getResponseCode() const {
    long http_code = 0;
    if (curl_) {
        curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &http_code);
    }
    return http_code;
}

/**
 * @brief Retrieves the error message from libcurl if an error occurred during the last request.
 */
const std::string& Curl::getErrorMessage() const {
    if (res_ != CURLE_OK) {
        // If curl_easy_perform returned an error, the errorBuffer_ might contain more details
        if (errorBuffer_[0] != '\0') {
            return std::string(errorBuffer_);
        } else {
            return std::string(curl_easy_strerror(res_));
        }
    }
    return ""; // No error
}