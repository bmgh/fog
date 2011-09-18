// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Global.h>

#include <stdio.h>

FOG_CVAR_DECLARE uint32_t fog_failed;

namespace Fog {

// ============================================================================
// [Fog::Debug - Helpers]
// ============================================================================

static void Debug_class_method(FILE* file, const char* className, const char* methodName)
{
  if (className)
  {
    fputs(className, file);
    fputs("::", file);
  }

  if (methodName)
  {
    fputs(methodName, file);
    fputs("() - ", file);
  }
}

static void Debug_format(FILE* file, const char* fmt, va_list ap)
{
  vfprintf(file, fmt, ap);
}

static FILE* Debug_getFile()
{
  return stderr;
}

#if defined(FOG_OS_WINDOWS)
// When a Fog-App is build using WIN32, there is no standard input/output. So
// we can show a message-box before the application exits without info.
// This can help to solve some problems.
static void WinMessageBox(const char* title, const char* fileName, int line, const char* msg)
{
  char buffer[1024];

  _snprintf(buffer, 1024, "%s, line %d\n\n%s\n", fileName, line, msg ? msg : "");
  MessageBoxA(NULL, buffer, title, MB_OK);
}
#endif // FOG_OS_WINDOWS

// ============================================================================
// [Fog::Debug - Message]
// ============================================================================

void Debug::dbgFormat(const char* fmt, ...)
{
  FILE* file = Debug_getFile();

  va_list ap;
  va_start(ap, fmt);
    Debug_format(file, fmt, ap);
  va_end(ap);
}

void Debug::dbgFunc(const char* className, const char* methodName, const char* fmt, ...)
{
  FILE* file = Debug_getFile();

  va_list ap;
  va_start(ap, fmt);
    Debug_class_method(file, className, methodName);
    Debug_format(file, fmt, ap);
  va_end(ap);
}

// ============================================================================
// [Fog::Debug - Failure]
// ============================================================================

void Debug::failFormat(const char* fmt, ...)
{
  FILE* file = Debug_getFile();

  va_list ap;
  va_start(ap, fmt);
    fprintf(file, "*** APPLICATION FAILURE ***\n");
    Debug_format(file, fmt, ap);
  va_end(ap);

  exit(-1);
}

void Debug::failFunc(const char* className, const char* methodName, const char* fmt, ...)
{
  FILE* file = Debug_getFile();

  va_list ap;
  va_start(ap, fmt);
    fprintf(file, "*** APPLICATION FAILURE ***\n");
    Debug_class_method(file, className, methodName);
    Debug_format(file, fmt, ap);
  va_end(ap);

  exit(-1);
}

// ============================================================================
// [Fog::Debug - Assert]
// ============================================================================

void Debug::assertMessage(const char* fileName, int line, const char* msg)
{
  FILE* file = Debug_getFile();

  fprintf(file, "*** ASSERTION FAILURE *** (%s, line %d)\n", fileName, line);
  fputs(msg, file);

#if defined(FOG_OS_WINDOWS)
  WinMessageBox("ASSERTION FAILURE", fileName, line, msg);
#endif // FOG_OS_WINDOWS

  exit(-1);
}

void Debug::assertFormat(const char* fileName, int line, const char* fmt, ...)
{
  FILE* file = Debug_getFile();

  va_list ap;
  va_start(ap, fmt);
    fprintf(file, "*** ASSERTION FAILURE *** (%s, line %d)\n", fileName, line);
    vfprintf(file, fmt, ap);
  va_end(ap);

#if defined(FOG_OS_WINDOWS)
  char buffer[1024];
  va_start(ap, fmt);
    _vsnprintf(buffer, 1024, fmt, ap);
  va_end(ap);
  WinMessageBox("ASSERTION FAILURE", fileName, line, buffer);
#endif // FOG_OS_WINDOWS

  exit(-1);
}

// ============================================================================
// [Fog::Debug - Exit]
// ============================================================================

void Debug::exit(int code)
{
  fog_failed = 1;
  ::exit(code);
}

} // Fog namespace
