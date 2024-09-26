#include "Hooks.h"
#include <detours/detours.h>

namespace Hooks
{
    typedef void(WINAPI* pFunc)(void* a1, const char* command);
    inline pFunc originalFunction;

    inline void replacementFunction(void* a1, const char* command)
    {
        std::string newCommand = command;
        if (command) {
            int i = newCommand.length();
            while (newCommand[i]) {
                i--;
                newCommand[i] = putchar(tolower(newCommand[i]));
            }
            size_t begin = newCommand.find_first_of(" ", 0) + 1;
            while (begin && begin < newCommand.size()) {
                size_t end = newCommand.find(" ", begin);
                if (end == newCommand.npos || !end) {
                    end = newCommand.size();
                }
                std::string  substr  = newCommand.substr(begin, end - begin);
                RE::TESForm* newForm = RE::TESForm::LookupByEditorID(substr);
                size_t       exclusion = newCommand.find("help", begin - 5);
                if (newForm && exclusion >= begin) {
                    char buffer[8];
                    char *stringID = itoa(newForm->GetFormID(), buffer, 16);
                    logger::info("Replaced {} with {}", substr, stringID);
                    newCommand.replace(begin, end - begin, stringID);
                }
                else {
                    begin = end + 1;
                }
            }
        }
        return originalFunction(a1, newCommand.data());
    }

    inline void Install() noexcept
    {
        const auto targetAddress = REL::ID(166307).address();
        const auto funcAddress = &replacementFunction;
        originalFunction = (pFunc)targetAddress;
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)originalFunction, (PBYTE)&replacementFunction);
        if (DetourTransactionCommit() == NO_ERROR)
            logger::info("Installed test hook on execute command set at {0:x} with replacement from address {0:x}",
                         targetAddress, (void*)funcAddress);
        else
            logger::warn("Failed to install test hook on papyrus global set");
    }
} // namespace Hooks
