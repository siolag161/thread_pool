#include <iostream>
#include <curl/curl.h>
#include <cstring>

#include "thread_pool.hpp"
using namespace std;

size_t write_buffer(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

void download_from_url(const char* url, const char* fname) {
    CURL *curl;
    FILE *fp;
    CURLcode res;
    curl = curl_easy_init();
    if (curl){
        printf("downloading from %s to %s\n", url, fname);
        fp = fopen(fname, "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_buffer);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
        printf("----------done from %s to %s----------\n", url, fname);
    }
}


int main() {
    std::vector<std::string> urls {
            "http://www.gutenberg.org/cache/epub/76/pg76.txt",
            "http://www.gutenberg.org/cache/epub/345/pg345.txt",
            "http://www.gutenberg.org/cache/epub/4300/pg4300.txt",
            "http://www.gutenberg.org/cache/epub/98/pg98.txt",
            "http://www.gutenberg.org/cache/epub/1080/pg1080.txt"
    };

    std::vector<std::string> output {
            "./out/huckleberry.txt",
            "./out/dracula.txt",
            "./out/ulysses.txt",
            "./out/tale_two_cities.txt",
            "./out/modest_proposal"
    };

    auto start = std::chrono::steady_clock::now();
    ThreadPool pool(urls.size());
    for (int i = 0; i < urls.size(); i++) {
        pool.enqueue(download_from_url, urls[i].c_str(), output[i].c_str());
    }
    auto end = std::chrono::steady_clock::now();
    std::cout << "Elapsed = " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << std::endl;
    return 0;
}