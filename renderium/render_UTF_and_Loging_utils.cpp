#ifndef RENDER_UTF8_AND_LOGGING
#define RENDER_UTF8_AND_LOGGING
#pragma execution_character_set("utf-8")

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <uchar.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

namespace render {
    std::ofstream logFile("log_renderium.log", std::ios::out);

    std::string codeToUTF8(char32_t code)
    {
        std::string ret;
        if (code <= 0x7F) {
            ret += code;
            return ret;
        }
        if (code <= 0x7FF) {
            ret += 0xC0 | (code >> 6);            /* 110xxxxx */
            ret += 0x80 | (code & 0x3F);          /* 10xxxxxx */
            return ret;
        }
        if (code <= 0xFFFF) {
            ret += 0xE0 | (code >> 12);           /* 1110xxxx */
            ret += 0x80 | ((code >> 6) & 0x3F);   /* 10xxxxxx */
            ret += 0x80 | (code & 0x3F);          /* 10xxxxxx */
            return ret;
        }
        if (code <= 0x10FFFF) {
            ret += 0xF0 | (code >> 18);           /* 11110xxx */
            ret += 0x80 | ((code >> 12) & 0x3F);  /* 10xxxxxx */
            ret += 0x80 | ((code >> 6) & 0x3F);   /* 10xxxxxx */
            ret += 0x80 | (code & 0x3F);          /* 10xxxxxx */
            return ret;
        }
        return ret;
    }
    std::string convertU32StringToUTF8(std::u32string UTF32String) {
        std::string ret;
        for (int_fast32_t i = 0; i < UTF32String.length(); i++)
        {
            if (UTF32String[i] <= 0x7F) {
                ret += UTF32String[i];
                return ret;
            }
            if (UTF32String[i] <= 0x7FF) {
                ret += 0xC0 | (UTF32String[i] >> 6);            /* 110xxxxx */
                ret += 0x80 | (UTF32String[i] & 0x3F);          /* 10xxxxxx */
            }
            if (UTF32String[i] <= 0xFFFF) {
                ret += 0xE0 | (UTF32String[i] >> 12);           /* 1110xxxx */
                ret += 0x80 | ((UTF32String[i] >> 6) & 0x3F);   /* 10xxxxxx */
                ret += 0x80 | (UTF32String[i] & 0x3F);          /* 10xxxxxx */
            }
            if (UTF32String[i] <= 0x10FFFF) {
                ret += 0xF0 | (UTF32String[i] >> 18);           /* 11110xxx */
                ret += 0x80 | ((UTF32String[i] >> 12) & 0x3F);  /* 10xxxxxx */
                ret += 0x80 | ((UTF32String[i] >> 6) & 0x3F);   /* 10xxxxxx */
                ret += 0x80 | (UTF32String[i] & 0x3F);          /* 10xxxxxx */
            }
            //ret += codeToUTF8(UTF32String[i]);
        }
        return ret;
    }
    void Log(std::string message, bool reportThread = true) {
        if (reportThread) {
            std::string ret;
            ret = "[";
            ret += std::to_string(time(NULL)) + "] " + message + "\n";
            logFile << ret;
            logFile.flush();
            return;
        }
        logFile << message << "\n";
        logFile.flush();
        return;
    }
    void ReportError(std::string message, bool programTerminating, bool ecceptionForNoErrorCompile = false) {
        std::cout << message << "\n";
        logFile << "=ERROR==============================================\n";
        logFile << message << "\n";
        logFile << "====================================================\n";
        logFile.flush();
        if (programTerminating)
        {
            if (ecceptionForNoErrorCompile && TRY_TO_RESOLVE_MINOR_ERRORS_INSTED_OF_CRASHING) {
                return;
            }
            std::cout << "Program Aborted\n";
            std::cout.flush();
            logFile.close();
            std::abort();
        }
    }
    void Log(std::u32string message, bool reportThread = true) {
        if (reportThread) {
            std::string ret;
            ret = "[";
            ret += std::to_string(time(NULL));
            ret += "] " + convertU32StringToUTF8(message) + "\n";
            logFile << ret;
            logFile.flush();
            return;
        }
        logFile << convertU32StringToUTF8(message) << "\n";
        logFile.flush();
        return;
    }
    void ReportError(std::u32string message, bool programTerminating, bool ecceptionForNoErrorCompile = false) {
        std::cout << convertU32StringToUTF8(message) << "\n";
        logFile << "=ERROR==============================================\n";
        logFile << convertU32StringToUTF8(message) << "\n";
        logFile << "====================================================\n";
        logFile.flush();
        if (programTerminating)
        {
            if (ecceptionForNoErrorCompile && TRY_TO_RESOLVE_MINOR_ERRORS_INSTED_OF_CRASHING) {
                return;
            }
            std::cout << "Program Aborted\n";
            std::cout.flush();
            logFile.close();
            std::abort();
        }
    }
}
#endif