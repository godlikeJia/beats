#include "pb2json.h"
#include <iostream>
#include <mutex>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/util/json_util.h>
 
using namespace std;
using namespace google::protobuf;
using namespace google::protobuf::compiler;
using namespace google::protobuf::util;

#define safe_delete(ptr)        \
    do {                        \
        if (nullptr != ptr) {   \
            delete ptr;         \
            ptr = nullptr;      \
        }                       \
    } while(0)

class PrintErrorCollector: public MultiFileErrorCollector {
    virtual void AddError(const std::string & filename, int line, int column, const std::string & message){
        printf("%s, %d, %d, %s\n", filename.c_str(), line, column, message.c_str());
    }
};

static DiskSourceTree* g_sourceTree;
static PrintErrorCollector* g_errorCollector;
static Importer*  g_importer;
static DynamicMessageFactory* g_factory;
static std::mutex g_mutex;

int init(const char* path, const char* file) {
    int ret = 0;
    do {
        g_sourceTree = new DiskSourceTree();
        if (nullptr == g_sourceTree) {
            ret = -1;
            break;
        }
        g_sourceTree->MapPath("", path);

        g_errorCollector = new PrintErrorCollector();
        if (nullptr == g_errorCollector) {
            ret = -2;
            break;
        }
        g_importer = new Importer(g_sourceTree, g_errorCollector);
        if (nullptr == g_importer) {
            cout << "new Importer failed" << endl;
            ret = -3;
        }
        if (nullptr == g_importer->Import(file)) {
            cout << "g_importer->Import failed. " << file << endl;
            ret -4;
        }

        g_factory = new DynamicMessageFactory();
        if (nullptr == g_factory) {
            ret = -5;
            break;
        }
    } while(0);

    if (0 != ret) {
        uninit();
    }

    return ret;
}

int uninit() {
    safe_delete(g_sourceTree);
    safe_delete(g_errorCollector);
    safe_delete(g_importer);
    safe_delete(g_factory);
}

int dynamic_encode(const char* msg_name, char** output, int* len) {
    const Descriptor *descriptor = g_importer->pool()->FindMessageTypeByName(msg_name);
    if (nullptr == descriptor) {
        cout << "FindMessageTypeByName return nullptr. " << msg_name << endl;
        return -1;
    }
    //cout << descriptor->DebugString();
 
    const Message *message = g_factory->GetPrototype(descriptor);
    Message *msg = message->New();

    const Reflection *reflection = msg->GetReflection();
    const FieldDescriptor *field = NULL;
    // for test
    field = descriptor->FindFieldByName("vid");
    reflection->SetString(msg, field, "vid_xxx");
    field = descriptor->FindFieldByName("duration");
    reflection->SetInt32(msg, field, 1111);

    string buff;
    msg->SerializeToString(&buff);

    *output = (char*)malloc(buff.size()+1);
    output[buff.size()] = '\0';
    memcpy(*output, &buff[0], buff.size());
    *len = buff.size();

    delete msg;
 
    return 0;
}

int dynamic_decode(const char* msg_name, const char* data, int len, char** output, int* output_len) {
    *output = nullptr;
    std::lock_guard<std::mutex> lock(g_mutex);
    const Descriptor *descriptor = g_importer->pool()->FindMessageTypeByName(msg_name);
    if (nullptr == descriptor) {
        cout << "FindMessageTypeByName return nullptr. " << msg_name << endl;
        return -1;
    }
    //cout << descriptor->DebugString();
 
    const Message *message = g_factory->GetPrototype(descriptor);
    if (nullptr == message) {
        cout << "g_factory->GetPrototype return nullptr. " << msg_name << endl;
        return -2;
    }
    Message *msg = message->New();
    if (nullptr == msg) {
        cout << "message->New return nullptr. " << msg_name << endl;
        return -3;
    }

    if (!msg->ParsePartialFromArray(data, len)) {
        cout << "ParsePartialFromArray failed.. " << msg_name 
            << " len: " << len << endl;
        delete msg;
        return -4;
    }

    string json;
    Status status = MessageToJsonString(*msg, &json);
    if (!status.ok()) {
        cout << "MessageToJsonString failed. " << status.error_code() << endl;
        delete msg;
        return -5;
    }
    delete msg;

    // cout << "decode: " << json << endl;
    *output = (char*)malloc(json.size());
    memcpy(*output, &json[0], json.size());
    *output_len = json.size();    

    return 0;
}

