#include "httpc.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wformat="

static size_t s_size_to_recieve = 0; 
static size_t s_content_length = 0; 

static size_t header_function(char* b, size_t size, size_t nitems, void *userdata) 
{
	size_t numbytes = size * nitems;
	std::string tmp_string(b, numbytes);
	tmp_string.erase(tmp_string.size() - 2);
	if(tmp_string.empty())
		return 0; // This will get us only the headers even if its a GET request
	std::vector<std::string> *data = static_cast<std::vector<std::string>*>(userdata);
	data->push_back(tmp_string);	
    return size * nitems;
}


static size_t write_function(void *ptr, size_t size, size_t nmemb, void *stream) {
    ((std::string*)stream)->append((char*)ptr, size * nmemb);
    return size * nmemb;
}

Result HTTPC::Init(u32 mem)
{
	this->socbuf = (void*)memalign(0x1000, 0x100000);
	if(socbuf == nullptr) return 0xDED26;
	return socInit((u32*)socbuf, 0x100000);
}

Result HTTPC::OpenContext(httpcContext *context, HTTPC_RequestMethod method, char* url, u32 use_defaultproxy)
{
	this->headers.clear();
	this->handle = curl_easy_init();
	curl_easy_setopt(this->handle, CURLOPT_URL, url);
	curl_easy_setopt(this->handle, CURLOPT_VERBOSE, 0L);	
	curl_easy_setopt(this->handle, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(this->handle, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
	curl_easy_setopt(this->handle, CURLOPT_HEADERFUNCTION, header_function);
	curl_easy_setopt(this->handle, CURLOPT_HEADERDATA, &this->headers);
	s_size_to_recieve = 0;
	
	switch(method)
	{
		case HTTPC_METHOD_GET:
		curl_easy_setopt(this->handle, CURLOPT_CUSTOMREQUEST, (char*)"GET"); //But we do not want the body right now
		break;
		case HTTPC_METHOD_HEAD:
		curl_easy_setopt(this->handle, CURLOPT_CUSTOMREQUEST, (char*)"HEAD"); 
		break;
		case HTTPC_METHOD_POST:
		curl_easy_setopt(this->handle, CURLOPT_CUSTOMREQUEST, (char*)"POST");
		break;
		case HTTPC_METHOD_PUT:
		curl_easy_setopt(this->handle, CURLOPT_CUSTOMREQUEST, (char*)"PUT");
		break;
		case HTTPC_METHOD_DELETE:
		curl_easy_setopt(this->handle, CURLOPT_CUSTOMREQUEST, (char*)"DELETE");
		break;
		default:
		return 0xDED53;
	}
	return 0;
}


Result HTTPC::AddRequestHeaderField(httpcContext *context, char *name, char *value)
{
	char *tmp_header = (char*)malloc((strlen(name) + strlen(value)) * sizeof(char*));
	sprintf(tmp_header, "%s: %s", name, value);
	this->header_chunk = curl_slist_append(this->header_chunk, tmp_header);
	free(tmp_header);
	return 0;
}

Result HTTPC::BeginRequest(httpcContext *context)
{
	curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->header_chunk);
	this->res = curl_easy_perform(this->handle);
	if(this->res == CURLE_WRITE_ERROR) this->res = CURLE_OK;
	return this->res; 
}

Result HTTPC::GetResponseHeader(httpcContext *context, char* _name, char* _value, u32 valuebuf_maxsize)
{
	for(int i = 0; i < this->headers.size(); i++)
	{
		if(this->headers[i].find(_name)!= std::string::npos)
		{
			size_t pos = this->headers[i].find(_name);
			this->headers[i].erase(pos, strlen(_name) + 2);
			strcpy(_value, this->headers[i].c_str());
			return 0;
		}
	}
	return 0xDED92;
}

Result HTTPC::GetResponseStatusCode(httpcContext *context, u32 *statuscode)
{
	if(this->res != CURLE_OK) 
		return 0xDED65;
	curl_easy_getinfo(this->handle, CURLINFO_RESPONSE_CODE, statuscode);
	return 0;
}

Result HTTPC::GetDownloadSizeState(httpcContext *context, u32 *downloadedsize, u32 *contentsize)
{
	char val[10];
	if(contentsize != nullptr)
	{
		this->GetResponseHeader(context, (char*)"Content-Length", val, 10);
		sscanf(val, "%d", contentsize);
	}
	if(downloadedsize != nullptr) *downloadedsize = s_size_to_recieve;
	return 0;
}

Result HTTPC::ReceiveData(httpcContext *context, u8 *buffer, u32 size)
{
	this->headers.clear();
	char *tmp_range = (char*)std::malloc(10);
	sprintf(tmp_range, "%d-%d", s_size_to_recieve, size); 
	curl_easy_setopt(this->handle, CURLOPT_RANGE, tmp_range);
	free(tmp_range);
	// We don't need to deal with headers any longer
	curl_easy_setopt(this->handle, CURLOPT_HEADERDATA, nullptr);
	curl_easy_setopt(this->handle, CURLOPT_HEADERFUNCTION, nullptr);
	
	curl_easy_setopt(this->handle, CURLOPT_HTTPGET, 1L); //Now get the body
	curl_easy_setopt(this->handle, CURLOPT_WRITEFUNCTION, write_function);
	std::string stream;
	curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &stream);
	this->res = curl_easy_perform(this->handle);
	std::memmove(buffer, stream.c_str(), size);
	s_size_to_recieve += size;
	return this->res;
}

Result HTTPC::SetSSLOpt(httpcContext *context, u32 option)
{
	switch(option)
	{
		case (1 << 8):
		break;
	}
	return 0;
}

Result HTTPC::CloseContext(httpcContext *context)
{
	//for(int i = 0; i < this->headers.size(); i++) std::cout << "Header: " << this->headers[i] << '\n';
	curl_slist_free_all(this->header_chunk);
	this->header_chunk = nullptr;
	curl_easy_cleanup(this->handle);
	return 0;
}

Result HTTPC::Exit(void)
{
	Result res = socExit();
	free(this->socbuf);
	return res;
}