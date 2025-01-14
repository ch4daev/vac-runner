#include "utils/utils.hpp"

#include <Windows.h>
#include <string>

typedef int (__cdecl* runfunc_t)(int code, void* input_buffer, int inputbuffer_size, void* outbuffer, int *outbuffer_size);

struct vac_packet {
    int code;
    int inputbuffer_size;
    int outbuffer_size;
    //here packet data
};

int main(int argc, char* argv[])
{
    utils::logger::info("%s \n", "Welcome to vac-runner!");

    if (argc < 3) {
        utils::logger::fatal("%s \n", "usage: vac-runner.exe <module> <packet>");
        return 0;
    }

    utils::logger::info("%s %s\n", "loading vac module ->", argv[1]);

    /*
        loading vac module
    */

    HMODULE vac_module = LoadLibraryA(argv[1]); 

    if (vac_module) {
        utils::logger::info("%s 0x%p\n", "vac module mapped at ->", vac_module);
    }
    else {
        utils::logger::fatal("%s %d\n", "vac module loading fail, with code ->", GetLastError());
        return false;
    }

    /*
        getting runfunc address
    */
    runfunc_t runfunc = reinterpret_cast<runfunc_t>(GetProcAddress(vac_module, "_runfunc@20"));

    if (runfunc) {
        utils::logger::info("%s 0x%p\n", "runfunc located at ->", runfunc);
    }
    else {
        utils::logger::fatal("%s %d\n", "failed get runfunc address, with code ->", GetLastError());
        return false;
    }

    utils::logger::info("%s %s\n", "loading vac packet ->", argv[2]);

    /*
        loading vac packet
    */

    size_t vac_packet_size = 0;
    void* vac_packet_buffer = utils::file::read_binary(argv[2], &vac_packet_size);

    if (vac_packet_buffer) {
        utils::logger::info("%s 0x%p\n", "vac packet loaded at ->", vac_packet_buffer);
    }
    else {
        utils::logger::fatal("%s 0x%p\n", "vac packet loading eror, with code ->", GetLastError());
        return false;
    }

    auto packet = reinterpret_cast<vac_packet*>(vac_packet_buffer);

    utils::logger::info("%s %d\n", "code ->", packet->code);
    utils::logger::info("%s %d %s\n", "input packet size ->", packet->inputbuffer_size, "bytes");
    utils::logger::info("%s %d %s\n", "output packet size ->", packet->outbuffer_size, "bytes");

    utils::logger::info("%s %d %s\n", "allocating", packet->outbuffer_size, "bytes for outbuffer");

    unsigned __int8* outbuffer = new unsigned __int8[packet->outbuffer_size];

    if (outbuffer) {
        utils::logger::info("%s 0x%p\n", "outbuffer allocated at ->", outbuffer);
    }
    else {
        utils::logger::fatal("%s %d\n", "outbuffer allocate fail, with code ->", GetLastError());
        delete[] vac_packet_buffer;
        return false;
    }

    utils::logger::info("%s \n", "calling runfunc");

    void* input_packet = reinterpret_cast<unsigned __int8*>(packet) + sizeof(vac_packet);

    int return_code = runfunc(packet->code, input_packet, packet->inputbuffer_size, outbuffer, &packet->outbuffer_size);

    utils::logger::info("%s %d\n", "module returned code ->", return_code);

    std::string outbuffer_name = "outbuffer_" + std::string(argv[2]);

    if (utils::file::write_binary(outbuffer, packet->outbuffer_size, outbuffer_name.c_str())) {
        utils::logger::info("%s %s\n", "outbuffer writted with name ->", outbuffer_name.c_str());
    }
    else {
        utils::logger::fatal("%s %d\n", "outbuffer write fail, with code ->", GetLastError());
    }

    delete[] vac_packet_buffer;
    delete[] outbuffer;
}