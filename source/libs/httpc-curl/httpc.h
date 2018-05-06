#pragma once

#include <3ds.h>
#include <cctype>
#include <cstring>
#include <string>
#include <inttypes.h>
#include <curl/curl.h>
#include <iostream>
#include <malloc.h>
#include <vector>


class HTTPC {
	public:
	Result Init(u32 mem);
	Result Exit(void);
	Result OpenContext(httpcContext *context, HTTPC_RequestMethod method, char *url, u32 use_defaultproxy);
	Result CloseContext(httpcContext *context);
	Result GetResponseStatusCode(httpcContext *context, u32 *statuscode);
	Result AddRequestHeaderField(httpcContext *context, char *name, char *url);
	Result BeginRequest(httpcContext *context);
	Result GetResponseHeader(httpcContext *context, char* _name, char* value, u32 valuebuf_maxsize);
	Result ReceiveData(httpcContext *context, u8 *buffer, u32 size);
	Result GetDownloadSizeState(httpcContext *context, u32 *downloadedsize, u32 *contentsize);
	Result SetSSLOpt(httpcContext *context, u32 option);
	private:
	void *socbuf = nullptr;
	CURL *handle;
	CURLcode res;
	struct curl_slist *header_chunk = nullptr;
	u32 downloaded_size;
	std::vector <std::string> headers;
};

extern HTTPC httpc;