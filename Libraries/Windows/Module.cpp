#include "Windows.h"
#include "Module.h"

#include "Common/Types.h"

#include <dbghelp.h>

//
// All of this is from some MSDN sample about PE's (I think) -Geoff
//

#define GetImgDirEntryRVA( pNTHdr, IDE ) \
  (pNTHdr->OptionalHeader.DataDirectory[IDE].VirtualAddress)

#define GetImgDirEntrySize( pNTHdr, IDE ) \
  (pNTHdr->OptionalHeader.DataDirectory[IDE].Size)

template <class T> PIMAGE_SECTION_HEADER GetEnclosingSectionHeader(DWORD rva, T* pNTHeader)	// 'T' == PIMAGE_NT_HEADERS 
{
  PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pNTHeader);
  unsigned i;

  for ( i=0; i < pNTHeader->FileHeader.NumberOfSections; i++, section++ )
  {
    // This 3 line idiocy is because Watcom's linker actually sets the
    // Misc.VirtualSize field to 0.  (!!! - Retards....!!!)
    DWORD size = section->Misc.VirtualSize;
    if ( 0 == size )
      size = section->SizeOfRawData;

    // Is the RVA within this section?
    if ( (rva >= section->VirtualAddress) && 
      (rva < (section->VirtualAddress + size)))
      return section;
  }

  return 0;
}

template <class T> LPVOID GetPtrFromRVA( DWORD rva, T* pNTHeader, PBYTE imageBase ) // 'T' = PIMAGE_NT_HEADERS 
{
  PIMAGE_SECTION_HEADER pSectionHdr;
  INT delta;

  pSectionHdr = GetEnclosingSectionHeader( rva, pNTHeader );
  if ( !pSectionHdr )
    return 0;

  delta = (INT)(pSectionHdr->VirtualAddress-pSectionHdr->PointerToRawData);
  return (PVOID) ( imageBase + rva - delta );
}

bool Windows::ModuleContainsProc(const std::string& dll, const std::string& proc)
{
  bool found = false;

  // open the file object
  HANDLE fileHandle = CreateFile( dll.c_str(), FILE_READ_DATA, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
  if ( fileHandle == INVALID_HANDLE_VALUE )
  {
    return false;
  }

  // create memory mapping object
  HANDLE mappingObject = CreateFileMapping( fileHandle, NULL, PAGE_READONLY, 0, 0, NULL );
  if ( mappingObject == NULL )
  {
    return false;
  }

  // map to an address for dbghelp api usage
  void* file = MapViewOfFile( mappingObject, FILE_MAP_READ, 0, 0, 0 );
  if ( file == NULL )
  {
    return false;
  }

  // get NT header from PE
  IMAGE_NT_HEADERS* header = ImageNtHeader(file);

  // signature should be "PE"
  if ( !header || header->Signature != 0x4550 )
  {
    return false;
  }

  // retrieve the exports list from the PE header
  DWORD size;
  IMAGE_EXPORT_DIRECTORY* exports = (IMAGE_EXPORT_DIRECTORY*)ImageDirectoryEntryToData( file, FALSE, IMAGE_DIRECTORY_ENTRY_EXPORT, &size);

  // if we have header data
  if (header != NULL && exports != NULL)
  {
    // resolve the RVA address to a local address space pointer
    DWORD* functionNames = (DWORD*)GetPtrFromRVA( exports->AddressOfNames, header, (PBYTE)file);

    if ( !functionNames )
    {
      return false;
    }

    // for each exported function
    for ( u32 i=0; i<exports->NumberOfFunctions; i++ )
    {
      // get the export name for this function
      const char* name = (const char*)GetPtrFromRVA(functionNames[i], header, (PBYTE)file);

      if ( !name )
      {
        continue;
      }

      // check for our special function name
      if (!strcmp(name, proc.c_str()))
      {
        // we found it
        found = true;

        // quit searching
        break;
      }
    }
  }

  // close files
  CloseHandle(mappingObject);
  CloseHandle(fileHandle);

  return found;
}