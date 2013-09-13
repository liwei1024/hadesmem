// Copyright (C) 2010-2013 Joshua Boyce.
// See the file COPYING for copying permission.

#pragma once

#include <type_traits>

#include <hadesmem/detail/warning_disable_prefix.hpp>
#include <boost/optional.hpp>
#include <hadesmem/detail/warning_disable_suffix.hpp>

#include <windows.h>
#include <tlhelp32.h>

#include <hadesmem/error.hpp>
#include <hadesmem/detail/smart_handle.hpp>
#include <hadesmem/detail/static_assert.hpp>

// TODO: Remove all the code duplication in First/Next wrappers. Looks like a 
// candidate for templates.

namespace hadesmem
{

namespace detail
{

inline detail::SmartSnapHandle CreateToolhelp32Snapshot(
  DWORD flags, 
  DWORD pid)
{
  detail::SmartSnapHandle snap;
  do
  {
    snap = ::CreateToolhelp32Snapshot(flags, pid);
  } while (!snap.IsValid() && ::GetLastError() == ERROR_BAD_LENGTH);

  if (!snap.IsValid())
  {
    DWORD const last_error = ::GetLastError();
    HADESMEM_DETAIL_THROW_EXCEPTION(Error() << 
      ErrorString("CreateToolhelp32Snapshot failed.") << 
      ErrorCodeWinLast(last_error));
  }

  return snap;
}


template <typename Entry, typename Func>
boost::optional<Entry> Toolhelp32Enum(
  Func func, 
  HANDLE snap, 
  std::string const& error)
{
  HADESMEM_DETAIL_STATIC_ASSERT(std::is_pod<Entry>::value);

  Entry entry;
  ::ZeroMemory(&entry, sizeof(entry));
  entry.dwSize = static_cast<DWORD>(sizeof(entry));
  if (!func(snap, &entry))
  {
    DWORD const last_error = ::GetLastError();
    if (last_error == ERROR_NO_MORE_FILES)
    {
      return boost::optional<Entry>();
    }

    HADESMEM_DETAIL_THROW_EXCEPTION(Error() << 
      ErrorString(error.c_str()) << 
      ErrorCodeWinLast(last_error));
  }

  return boost::optional<Entry>(entry);
}

inline boost::optional<MODULEENTRY32> Module32First(HANDLE snap)
{
  return Toolhelp32Enum<MODULEENTRY32, decltype(&::Module32First)>(
    &::Module32First, 
    snap, 
    "Module32First failed.");
}

inline boost::optional<MODULEENTRY32> Module32Next(HANDLE snap)
{
  return Toolhelp32Enum<MODULEENTRY32, decltype(&::Module32Next)>(
    &::Module32Next, 
    snap, 
    "Module32Next failed.");
}

inline boost::optional<PROCESSENTRY32> Process32First(HANDLE snap)
{
  return Toolhelp32Enum<PROCESSENTRY32, decltype(&::Process32First)>(
    &::Process32First, 
    snap, 
    "Process32First failed.");
}

inline boost::optional<PROCESSENTRY32> Process32Next(HANDLE snap)
{
  return Toolhelp32Enum<PROCESSENTRY32, decltype(&::Process32Next)>(
    &::Process32Next, 
    snap, 
    "Process32Next failed.");
}

inline boost::optional<THREADENTRY32> Thread32First(HANDLE snap)
{
  return Toolhelp32Enum<THREADENTRY32, decltype(&::Thread32First)>(
    &::Thread32First, 
    snap, 
    "Thread32First failed.");
}

inline boost::optional<THREADENTRY32> Thread32Next(HANDLE snap)
{
  return Toolhelp32Enum<THREADENTRY32, decltype(&::Thread32Next)>(
    &::Thread32Next, 
    snap, 
    "Thread32Next failed.");
}

}

}
