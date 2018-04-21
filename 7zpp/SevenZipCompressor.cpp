#include "stdafx.h"
#include "SevenZipCompressor.h"
#include "GUIDs.h"
#include "FileSys.h"
#include "ArchiveUpdateCallback.h"
#include "OutStreamWrapper.h"
#include "PropVariant.h"
#include "UsefulFunctions.h"


namespace SevenZip
{

using namespace intl;


const TString SearchPatternAllFiles = _T( "*" );

SevenZipCompressor::SevenZipCompressor(const SevenZipLibrary& library, const TString& archivePath)
	: SevenZipArchive(library, archivePath)
	, m_absolutePath(false)
{
}

SevenZipCompressor::~SevenZipCompressor()
{
}


bool SevenZipCompressor::AddDirectory( const TString& directory, bool includeSubdirs /*= true*/)
{
	if(m_absolutePath)
		return AddFilesToList(directory, SearchPatternAllFiles, _T(""), includeSubdirs);
	else
		return AddFilesToList(directory, SearchPatternAllFiles, FileSys::GetPath(directory), includeSubdirs);
}

bool SevenZipCompressor::AddFiles( const TString& directory, const TString& searchFilter, bool includeSubdirs /*= true*/)
{
	if(m_absolutePath)
		return AddFilesToList(directory, searchFilter, _T(""), includeSubdirs);
	else
		return AddFilesToList(directory, searchFilter, directory, includeSubdirs);
}

bool SevenZipCompressor::AddAllFiles( const TString& directory, bool includeSubdirs /*= true*/)
{
	if(m_absolutePath)
		return AddFilesToList(directory, SearchPatternAllFiles, _T(""), includeSubdirs);
	else
		return AddFilesToList(directory, SearchPatternAllFiles, directory, includeSubdirs);
}

bool SevenZipCompressor::AddFile(const TString& filePath)
{
	std::vector< FilePathInfo > files = FileSys::GetFile(filePath, m_absolutePath);

	if ( files.empty() )
	{
		return false;
	}

	m_fileList.insert(m_fileList.end(), files.begin(), files.end());

	return true;
}

bool SevenZipCompressor::DoCompress(ProgressCallback* callback /*= nullptr*/)
{
	if (m_fileList.empty())
	{
		return false;
	}

	if (!CheckValidFormat())
	{
		return false;
	}

	CComPtr< IOutArchive > archiver = UsefulFunctions::GetArchiveWriter(m_library, m_compressionFormat);
	if (!archiver)
	{
		// compression not supported
		return false;
	}

	SetCompressionProperties(archiver);

	//Set full outputFilePath including ending
	m_archivePath += UsefulFunctions::EndingFromCompressionFormat(m_compressionFormat);

	CComPtr< OutStreamWrapper > outFile = new OutStreamWrapper(OpenArchiveStream());
	CComPtr< ArchiveUpdateCallback > updateCallback = new ArchiveUpdateCallback(m_fileList, m_archivePath, callback);

	HRESULT hr = archiver->UpdateItems(outFile, (UInt32)m_fileList.size(), updateCallback);

	if (callback)
	{
		callback->OnDone(m_archivePath);	//Todo: give full path support
	}

	// returning S_FALSE also indicates error
	return (hr == S_OK) ? true : false;
}

bool SevenZipCompressor::CheckValidFormat()
{
	if (m_fileList.size() > 1 &&
		(   m_compressionFormat == SevenZip::CompressionFormat::BZip2
		 || m_compressionFormat == SevenZip::CompressionFormat::GZip)
		)
	{
		// Not supported by compressing format
		return false;
	}

	return true;
}

CComPtr< IStream > SevenZipCompressor::OpenArchiveStream()
{
	CComPtr< IStream > fileStream = FileSys::OpenFileToWrite(m_archivePath);
	if ( fileStream == NULL )
	{
		return nullptr;
	}
	return fileStream;
}

bool SevenZipCompressor::AddFilesToList( const TString& directory, const TString& searchPattern, const TString& pathPrefix, bool recursion)
{
	if ( !FileSys::DirectoryExists( directory ) )
	{
		return false;	//Directory does not exist
	}

	if ( FileSys::IsDirectoryEmptyRecursive( directory ) )
	{
		return false;	//Directory \"%s\" is empty" ), directory.c_str()
	}

	std::vector< FilePathInfo > files = FileSys::GetFilesInDirectory( directory, searchPattern, pathPrefix, recursion );
	if (files.empty())
	{
		return false;
	}

	m_fileList.insert(m_fileList.end(), files.begin(), files.end());

	return true;
}

bool SevenZipCompressor::SetCompressionProperties( IUnknown* outArchive )
{
	if (!outArchive)
	{
		return false;
	}

	const size_t numProps = 1;
	const wchar_t* names[numProps] = { L"x" };
	CPropVariant values[numProps] = { static_cast< UInt32 >( m_compressionLevel.GetValue() ) };

	CComPtr< ISetProperties > setter;
	outArchive->QueryInterface( IID_ISetProperties, reinterpret_cast< void** >( &setter ) );
	if ( setter == nullptr )
	{
		return false;	//Archive does not support setting compression properties
	}

	HRESULT hr = setter->SetProperties( names, values, numProps );

	// returning S_FALSE also indicates error
	return (hr == S_OK) ? true : false;
}

}
