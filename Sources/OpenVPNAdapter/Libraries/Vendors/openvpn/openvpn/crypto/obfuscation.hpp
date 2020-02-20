//
//  obfuscation.hpp
//
//  Created by EHome on 2019/11/5.
//
//

#ifndef OPENVPN_OBFUSCATION_H
#define OPENVPN_OBFUSCATION_H

#include <memory>
#include <string.h>
#include <openvpn/buffer/buffer.hpp>

//static char * key = "GENEE3KVH4HIPFM8";
static std::string key = "";

namespace openvpn {

void obscure_setkey(std::string newKey)
{
    key = newKey;
}

void data_xor(unsigned char* data, int len)
{
    size_t key_len = key.length();
    int i;
    
    for(i=0; i<len; i++)
    {
        data[i] ^= key[i%key_len];
    }
}

void obscure_encrypt(BufferAllocated& buf)
{
    if (key.length() == 0) {
        return;
    }
    unsigned char * data = buf.data();
    size_t size = buf.size();
    /* add random padding on head of small packet */
    if (buf.size() < 128) {
        srand( (unsigned)time(NULL) );
        int padding = 3 + rand() % 10;
        
        memmove(data + padding, data, size);
        memset(data, 0, padding);
        size += padding;
        buf.set_size(size);
    }
    
    data_xor(data, size);
}

void obscure_decrypt(Buffer& buf)
{
    if (key.length() == 0) {
        return;
    }
    unsigned char * data = buf.data();
    size_t size = buf.size();
    data_xor(data, size);
    
    /* remove padding on head */
    int padding = 0;
    int i;
    for(i=0; i<size; i++)
    {
        if (data[i] != 0x00)
        {
            padding = i;
            break;
        }
    }
    
    if (padding > 0)
    {
        memmove(data, data + padding, size - padding);
        buf.set_size(buf.size() - padding);
    }
}
}
#endif
