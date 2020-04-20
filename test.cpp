#include "test1/test1.h"
#include <iostream>
#include <string>
#include <sstream>
#include <windows.h>
#include <dbghelp.h>
#include "test.h"
using namespace std;

static bool PtrToString(uintptr_t ptr, char* buf, size_t size) {
  static constexpr char kHexCharacters[] = "0123456789abcdef";
  static constexpr int kHexBase = 16;

  // The addressible space of an N byte pointer is at most 2^(8N).
  // Since we are printing an address in hexadecimal, the number of hex
  // characters we must print (lets call this H) satisfies 2^(8N) = 16^H.
  // Therefore H = 2N.
  size_t num_hex_chars = 2 * sizeof(uintptr_t);

  // The buffer size also needs 4 extra bytes:
  // 2 bytes for 0x prefix,
  // 1 byte for a '\n' newline suffix, and
  // 1 byte for a '\0' null terminator.
  if (size < (num_hex_chars + 4)) {
    return false;
  }

  buf[0] = '0';
  buf[1] = 'x';

  // Convert the entire number to hex, going backwards.
  int start_index = 2;
  for (int i = num_hex_chars - 1 + start_index; i >= start_index; --i) {
    buf[i] = kHexCharacters[ptr % kHexBase];
    ptr /= kHexBase;
  }

  // Terminate the output with a newline and NULL terminator.
  int current_index = start_index + num_hex_chars;
  buf[current_index] = '\n';
  buf[current_index + 1] = '\0';

  return true;
}

static inline void SafePrintStackTracePointers() {
  static constexpr char begin_msg[] = "*** BEGIN STACK TRACE POINTERS ***\n";
  std::cout << begin_msg << std::endl;

  static constexpr int kMaxStackFrames = 64;
  void* trace[kMaxStackFrames];
  int num_frames = CaptureStackBackTrace(0, kMaxStackFrames, trace, NULL);

  for (int i = 0; i < num_frames; ++i) {
    char buffer[32] = "unsuccessful ptr conversion";
    PtrToString(reinterpret_cast<uintptr_t>(trace[i]), buffer, sizeof(buffer));
	std::cout << buffer << std::endl;
  }

  static constexpr char end_msg[] = "*** END STACK TRACE POINTERS ***\n\n";
  std::cout << end_msg << std::endl;
}

static bool SymbolsAreAvailableInit() {
  SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
  return SymInitialize(GetCurrentProcess(), NULL, true);
}

static bool SymbolsAreAvailable() {
  static bool kSymbolsAvailable = SymbolsAreAvailableInit();  // called once
  return kSymbolsAvailable;
}

std::string CurrentStackTrace() {
  // For reference, many stacktrace-related Windows APIs are documented here:
  // https://docs.microsoft.com/en-us/windows/win32/debug/about-dbghelp.
  HANDLE current_process = GetCurrentProcess();
  static constexpr int kMaxStackFrames = 64;
  void* trace[kMaxStackFrames];
  int num_frames = CaptureStackBackTrace(0, kMaxStackFrames, trace, NULL);

  //static mutex mu(tensorflow::LINKER_INITIALIZED);

  std::string stacktrace;
  for (int i = 0; i < num_frames; ++i) {
    const char* symbol = "(unknown)";
    if (SymbolsAreAvailable()) {
      char symbol_info_buffer[sizeof(SYMBOL_INFO) +
                              MAX_SYM_NAME * sizeof(TCHAR)];
      SYMBOL_INFO* symbol_ptr =
          reinterpret_cast<SYMBOL_INFO*>(symbol_info_buffer);
      symbol_ptr->SizeOfStruct = sizeof(SYMBOL_INFO);
      symbol_ptr->MaxNameLen = MAX_SYM_NAME;

      // Because SymFromAddr is not threadsafe, we acquire a lock.
      //mutex_lock lock(mu);
      if (SymFromAddr(current_process, reinterpret_cast<DWORD64>(trace[i]), 0,
                      symbol_ptr)) {
        symbol = symbol_ptr->Name;
      }
    }

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "0x%p\t%s", trace[i], symbol);
    stacktrace += buffer;
    stacktrace += "\n";
  }

  return stacktrace;
}

void fun3(int32_t c)
{
	SafePrintStackTracePointers();
	std::cout << CurrentStackTrace() << std::endl;
}

void fun2(float b)
{
    int32_t c = 3;
	fun4();
}

void fun1(int a)
{
	float b = 5.0;
	fun2(b);
}


int main(int argc, char *argv[])
{
    fun1(5);
}